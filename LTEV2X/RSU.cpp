#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Event.h"
#include"Global.h"


using namespace std;

int newCount = 0;//��¼��̬�����Ķ���Ĵ���

int deleteCount = 0;//��¼ɾ����̬��������Ĵ���

cRSU::cRSU() {}

void cRSU::initialize(sRSUConfigure &t_RSUConfigure){
	m_RSUId = t_RSUConfigure.wRSUID;
	m_fAbsX = c_RSUTopoRatio[m_RSUId * 2 + 0] * c_wide;
	m_fAbsY = c_RSUTopoRatio[m_RSUId * 2 + 1] * c_length;
	RandomUniform(&m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU��");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_RSUId, m_fAbsX, m_fAbsY);
	//printf("m_fAbsX=%f,m_fAbsY=%f\n",m_fAbsX,m_fAbsY);



	//���߹���Ԫ������ʼ��
	m_DRAClusterNum = c_RSUClusterNum[m_RSUId];
	//WRONG
	/*  EMERGENCY  */
	m_DRAEmergencyPatternIsAvailable = vector<bool>(gc_DRAEmergencyTotalPatternNum, true);
	m_DRAEmergencyScheduleInfoTable = vector<sDRAScheduleInfo*>(gc_DRAEmergencyTotalPatternNum);
	m_DRAEmergencyTransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(gc_DRAEmergencyTotalPatternNum);
	/*  EMERGENCY  */


	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);
	m_DRAPatternIsAvailable = vector<vector<bool>>(m_DRAClusterNum, vector<bool>(gc_DRATotalPatternNum, true));
	m_DRAScheduleInfoTable = vector<vector<sDRAScheduleInfo*>>(m_DRAClusterNum, vector<sDRAScheduleInfo*>(gc_DRATotalPatternNum, nullptr));
	m_DRATransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(gc_DRATotalPatternNum, list<sDRAScheduleInfo*>(0, nullptr));
	//WRONG
}


int cRSU::DRAGetClusterIdx(int TTI) {
	int roundATTI = TTI%gc_DRA_NTTI; //��TTIӳ�䵽[0-gc_DRA_NTTI)�ķ�Χ
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_DRAClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


int cRSU::getMaxIndex(const std::vector<double>&clusterSize) {
	double max = 0;
	int dex = -1;
	for (int i = 0; i < static_cast<int>(clusterSize.size()); i++) {
		if (clusterSize[i] > max) {
			dex = i;
			max = clusterSize[i];
		}
	}
	return dex;
}

int cRSU::getClusterIdxOfVeUE(int VeUEId) {
	int dex = -1;
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		for (int Id : m_DRAClusterVeUEIdList[clusterIdx])
			if (Id == VeUEId) return clusterIdx;
	}
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)���ó����ڵ�ǰRSU��");
}


list<tuple<int, int>> cRSU::buildScheduleIntervalList(int TTI, const sEvent& event, std::tuple<int, int, int>ClasterTTI) {
	list<tuple<int,int>> scheduleIntervalList;
	int eventId = event.eventId;
	int occupiedTTI = event.message.bitNum / gc_BitNumPerRB / gc_DRA_FBNumPerPatternType[event.message.messageType];
	int begin = std::get<0>(ClasterTTI),
		end = std::get<1>(ClasterTTI),
		len = std::get<2>(ClasterTTI);

	//��ǰʱ�̵����TTI
	int roundTTI = TTI%gc_DRA_NTTI;

	//��RSU��һ�ֵ��ȵ���ʼTTI����һ���صĿ�ʼʱ�̣�
	int nextTurnBeginTTI = TTI - roundTTI + gc_DRA_NTTI;

	//��ǰһ�ֵ�����ʣ����õ�ʱ϶����
	int remainTTI = end - roundTTI + 1;

	//������ǰһ�ֵ��ȿ���ʱ϶�����Ĳ���
	int overTTI = occupiedTTI - remainTTI;


	if (overTTI <= 0) scheduleIntervalList.push_back(std::tuple<int, int>(TTI, TTI + occupiedTTI - 1));
	else {
		scheduleIntervalList.push_back(std::tuple<int, int>(TTI, TTI + remainTTI - 1));
		int n = overTTI / len;
		for (int i = 0; i < n; i++) scheduleIntervalList.push_back(std::tuple<int, int>(nextTurnBeginTTI + i*gc_DRA_NTTI + begin, nextTurnBeginTTI + begin + len - 1 + i*gc_DRA_NTTI));
		if (overTTI%len != 0) scheduleIntervalList.push_back(std::tuple<int, int>(nextTurnBeginTTI + n*gc_DRA_NTTI + begin, nextTurnBeginTTI + begin + n*gc_DRA_NTTI + overTTI%len - 1));
	}
	return scheduleIntervalList;
}


std::list<std::tuple<int, int>> cRSU::buildEmergencyScheduleInterval(int TTI, const sEvent& event) {
	std::list<std::tuple<int, int>> scheduleIntervalList;
	std::tuple<int, int> scheduleInterval;
	int eventId = event.eventId;
	int occupiedTTI = event.message.bitNum / gc_BitNumPerRB / gc_DRAEmergencyFBNumPerPattern;
	get<0>(scheduleInterval) = TTI;
	get<1>(scheduleInterval) = TTI + occupiedTTI - 1;
	scheduleIntervalList.push_back(scheduleInterval);
	return scheduleIntervalList;
}


void cRSU::DRAInformationClean() {
	m_DRAAdmitEventIdList.clear();
	m_DRAEmergencyAdmitEventIdList.clear();
}


void cRSU::DRAGroupSizeBasedTDM(cVeUE *systemVeUEVec) {
	//�������������RSU����һ����ʱ
	if (m_VeUEIdList.size() == 0) {
		/*-----------------------ATTENTION-----------------------
		* ����ֵΪ(0,-1,0)�ᵼ�»�ȡ��ǰ�ر��ʧ�ܣ����������ط���Ҫ����
		* ���ֱ�Ӹ�ÿ���ض���ֵΪ�������䣬����Ҳû���κ����ã����������������
		*------------------------ATTENTION-----------------------*/
		m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, gc_DRA_NTTI-1, gc_DRA_NTTI));
		return;
	}

	//��ʼ��
	m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, 0, 0));

	//����ÿ��TTIʱ϶��Ӧ��VeUE��Ŀ(double)��!!!������!!��
	double VeUESizePerTTI = static_cast<double>(m_VeUEIdList.size()) / static_cast<double>(gc_DRA_NTTI);

	//clusterSize�洢ÿ���ص�VeUE��Ŀ(double)��!!!������!!��
	std::vector<double> clusterSize(m_DRAClusterNum, 0);

	//��ʼ��clusterSize
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++)
		clusterSize[clusterIdx] = static_cast<double>(m_DRAClusterVeUEIdList[clusterIdx].size());

	//���ȸ�������һ�����Ĵط���һ��TTI
	int basicNTTI = 0;
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		//����ô���������һ��VeUE��ֱ�ӷ����һ����λ��ʱ����Դ
		if (m_DRAClusterVeUEIdList[clusterIdx].size() != 0) {
			get<2>(m_DRAClusterTDRInfo[clusterIdx]) = 1;
			clusterSize[clusterIdx] -= VeUESizePerTTI;
			basicNTTI++;
		}
	}

	//���˸���Ϊ�յĴط����һ��TTI�⣬ʣ��ɷ����TTI����
	int remainNTTI = gc_DRA_NTTI - basicNTTI;
	
	//����ʣ�µ���Դ�飬ѭ������һʱ϶�������ǰ������ߵĴأ�����֮�󣬸��Ķ�Ӧ�ı�������ȥ��ʱ϶��Ӧ��VeUE����
	while (remainNTTI > 0) {
		int dex = getMaxIndex(clusterSize);
		if (dex == -1) throw Exp("������û�з����ʱ����Դ������ÿ�����ڵ�VeUE�Ѿ�Ϊ����");
		get<2>(m_DRAClusterTDRInfo[dex])++;
		remainNTTI--;
		clusterSize[dex] -= VeUESizePerTTI;
	}

	//��ʼ�������䷶Χ��������
	get<0>(m_DRAClusterTDRInfo[0]) = 0;
	get<1>(m_DRAClusterTDRInfo[0]) = get<0>(m_DRAClusterTDRInfo[0]) + get<2>(m_DRAClusterTDRInfo[0]) - 1;
	for (int clusterIdx = 1;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		get<0>(m_DRAClusterTDRInfo[clusterIdx]) = get<1>(m_DRAClusterTDRInfo[clusterIdx - 1]) + 1;
		get<1>(m_DRAClusterTDRInfo[clusterIdx]) = get<0>(m_DRAClusterTDRInfo[clusterIdx]) + get<2>(m_DRAClusterTDRInfo[clusterIdx]) - 1;
	}


	//����������д���RSU�ڵ�ÿһ������
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;++clusterIdx) {
		for (int VeUEId : m_DRAClusterVeUEIdList[clusterIdx])
			systemVeUEVec[VeUEId].m_ScheduleInterval = tuple<int, int>(get<0>(m_DRAClusterTDRInfo[clusterIdx]), get<1>(m_DRAClusterTDRInfo[clusterIdx]));
	}
}


void cRSU::DRAProcessEventList(int TTI,const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList) {
	int clusterIdx = DRAGetClusterIdx(TTI);//��ǰ�ɴ������ݵĴر��
	for (int eventId : systemEventTTIList[TTI]) {
		sEvent event = systemEventVec[eventId];
		int VeUEId = event.VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//��ǰ�¼���Ӧ��VeUE���ڵ�ǰRSU�У���������
			continue;
		}
		else {//��ǰ�¼���Ӧ��VeUE�ڵ�ǰRSU��
			if (systemEventVec[eventId].message.messageType == EMERGENCY) {//���ǽ����¼�
				/*  EMERGENCY  */
				pushToEmergencyAdmitEventIdList(eventId);

				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 21, m_RSUId, -1, -1);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 21, eventId, m_RSUId, -1, -1);
				/*  EMERGENCY  */
			}
			else {//һ�����¼������������¼�����������ҵ���¼�
				if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//��ǰʱ���¼������е�VeUEǡ��λ�ڸ�RSU�ĸô��ڣ���ӵ���ǰ��������
					//�����¼�ѹ���������
					pushToAdmitEventIdList(eventId);

					//���¸��¼�����־
					systemEventVec[eventId].addEventLog(TTI, 1, m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 1, eventId, m_RSUId, -1, -1);
				}
				else {//���򣬵�ǰ�¼��ڴ�ʱ�����������䣬Ӧת��ȴ�����
					  //�����¼�ѹ��ȴ�����
					pushToWaitEventIdList(eventId);

					//���¸��¼�����־
					systemEventVec[eventId].addEventLog(TTI, 2, m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 2, eventId, m_RSUId, -1, -1);
				}
			}	
		}
	}
}


void cRSU::DRAProcessScheduleInfoTableWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList) {
	/*  EMERGENCY  */
	for (int patternIdx = 0;patternIdx < gc_DRAEmergencyTotalPatternNum;patternIdx++) {
		if (m_DRAEmergencyScheduleInfoTable[patternIdx] == nullptr) {//��ǰEmergencyPattern���¼�����
			continue;
		}
		else {
			int eventId = m_DRAEmergencyScheduleInfoTable[patternIdx]->eventId;
			int VeUEId = systemEventVec[eventId].VeUEId;
			if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
				pushToSwitchEventIdList(eventId, systemDRASwitchEventIdList);

				//���ͷŸõ�����Ϣ����Դ
				delete m_DRAEmergencyScheduleInfoTable[patternIdx];
				deleteCount++;
				m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;

				//�ͷ�Pattern��Դ
				m_DRAEmergencyPatternIsAvailable[patternIdx] = true;

				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 23, m_RSUId, -1, patternIdx);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 23, eventId, m_RSUId, -1, patternIdx);
			}
			else {//�ó������ڵ�ǰRSU��
				//doing nothing 
				continue;
			}
		}
	}
	/*  EMERGENCY  */
	
	//��ʼ���� m_DRAScheduleInfoTable
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++) {
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			if (m_DRAScheduleInfoTable[clusterIdx][patternIdx] == nullptr) {//��ǰPattern�����¼��ڴ���
				continue;
			}
			else {
				int eventId = m_DRAScheduleInfoTable[clusterIdx][patternIdx]->eventId;
				int VeUEId = systemEventVec[eventId].VeUEId;
				if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
					pushToSwitchEventIdList(eventId, systemDRASwitchEventIdList);

					//���ͷŸõ�����Ϣ����Դ
					delete m_DRAScheduleInfoTable[clusterIdx][patternIdx];
					deleteCount++;
					m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

					//�ͷ�Pattern��Դ
					m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

					//���¸��¼�����־
					systemEventVec[eventId].addEventLog(TTI, 3, m_RSUId, clusterIdx, patternIdx);
					
					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 3, eventId, m_RSUId, clusterIdx, patternIdx);
				}
				else {
					if (getClusterIdxOfVeUE(VeUEId) != clusterIdx) {//RSU�ڲ������˴��л�������ӵ��ȱ���ȡ����ѹ��ȴ�����
						pushToWaitEventIdList(eventId);

						//���ͷŸõ�����Ϣ����Դ
						delete m_DRAScheduleInfoTable[clusterIdx][patternIdx];
						deleteCount++;
						m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

						//�ͷ�Pattern��Դ
						m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

						//���¸��¼�����־
						systemEventVec[eventId].addEventLog(TTI, 4, m_RSUId, clusterIdx, patternIdx);
						
						//��¼TTI��־
						DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 4, eventId, m_RSUId, clusterIdx, patternIdx);
					}
					else {//��û�з���RSU�л���Ҳû�з���RSU�ڴ��л���ʲôҲ����
						//doing nothing
						continue;
					}
				}
			}
		}
	}
}


void cRSU::DRAProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec,std::list<int> &systemDRASwitchEventIdList) {
	/*  EMERGENCY  */
	for (int eventId : m_DRAEmergencyWaitEventIdList) {
		pushToEmergencyAdmitEventIdList(eventId);//���¼�ѹ���������

		//���¸��¼�����־
		systemEventVec[eventId].addEventLog(TTI, 26, m_RSUId, -1, -1);

		//��¼TTI��־
		DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 26, eventId, m_RSUId, -1, -1);
	}

	//���ڻ�ȫ��ѹ��Emergency�����������ֱ����վ����ˣ�����Ҫһ����ɾ��
	m_DRAEmergencyWaitEventIdList.clear();
	/*  EMERGENCY  */


	//��ʼ���� m_DRAWaitEventIdList
	int clusterIdx = DRAGetClusterIdx(TTI);//���ڵ����еĴر��
	list<int>::iterator it = m_DRAWaitEventIdList.begin();
	while(it!= m_DRAWaitEventIdList.end()){
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//���¼���ǰ���Խ��е���
			pushToAdmitEventIdList(eventId);//��ӵ�RSU����Ľ���������
			it = m_DRAWaitEventIdList.erase(it);//����ӵȴ�������ɾ��

			//���¸��¼�����־
			systemEventVec[eventId].addEventLog(TTI, 6, m_RSUId, -1, -1);

			//��¼TTI��־
			DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 6, eventId, m_RSUId, -1, -1);
		}
		else {//���¼���ǰ�����Խ��е���
			it++;
			continue; //�����ȴ�����ǰTTI��������
		}
	}
}


void cRSU::DRAProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList) {
	/*  EMERGENCY  */
	//��ʼ����m_DRAEmergencyWaitEventIdList
	list<int>::iterator it = m_DRAEmergencyWaitEventIdList.begin();
	while (it != m_DRAEmergencyWaitEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
			pushToSwitchEventIdList(eventId, systemDRASwitchEventIdList);//������ӵ�System�����RSU�л�������
			it = m_DRAEmergencyWaitEventIdList.erase(it);

			//���¸��¼�����־
			systemEventVec[eventId].addEventLog(TTI, 25, m_RSUId, -1, -1);

			//��¼TTI��־
			DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 25, eventId, m_RSUId, -1, -1);
		}
		else {//��Ȼ���ڵ�ǰRSU��Χ��
			it++;
			continue; //�������ڵ�ǰRSU��Emergency�ȴ�����
		}
	}
	/*  EMERGENCY  */
	
	
	//��ʼ���� m_DRAWaitEventIdList
	it = m_DRAWaitEventIdList.begin();
	while (it != m_DRAWaitEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
			pushToSwitchEventIdList(eventId, systemDRASwitchEventIdList);//������ӵ�System�����RSU�л�������
			it = m_DRAWaitEventIdList.erase(it);//����ӵȴ�������ɾ��

			//���¸��¼�����־
			systemEventVec[eventId].addEventLog(TTI, 5, m_RSUId, -1, -1);

			//��¼TTI��־
			DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 5, eventId, m_RSUId, -1, -1);
		}
		else {//��Ȼ���ڵ�ǰRSU��Χ��
			it++;
			continue; //�������ڵ�ǰRSU�ĵȴ�����
		}
	}
}


void cRSU::DRAProcessSwitchListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec,std::list<int> &systemDRASwitchEventIdList) {
	list<int>::iterator it = systemDRASwitchEventIdList.begin();
	int clusterIdx = DRAGetClusterIdx(TTI);//��ǰ�ɴ������ݵĴر��
	while (it != systemDRASwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//���л������е��¼���Ӧ��VeUE�������ڵ�ǰ�أ���������
			it++;
			continue;
		}
		else {//���л������е�VeUE�����ڵ�RSU
			/*  EMERGENCY  */
			if (systemEventVec[eventId].message.messageType == EMERGENCY) {//���ڽ����¼�
				pushToEmergencyAdmitEventIdList(eventId);
				it = systemDRASwitchEventIdList.erase(it);

				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 27, m_RSUId, -1, -1);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 27, eventId, m_RSUId, -1, -1);
			}
			/*  EMERGENCY  */

			else {//�ǽ����¼�
				if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//���л������е�VeUEǡ��λ�ڸ�RSU�ĵ�ǰ���ڣ���ӵ���ǰ��������
					pushToAdmitEventIdList(eventId);
					it = systemDRASwitchEventIdList.erase(it);

					//���¸��¼�����־
					systemEventVec[eventId].addEventLog(TTI, 7, m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 7, eventId, m_RSUId, -1, -1);
				}
				else {//�����ڵ�ǰ�أ�ת��ȴ�����
					pushToWaitEventIdList(eventId);
					it = systemDRASwitchEventIdList.erase(it);

					//���¸��¼�����־
					systemEventVec[eventId].addEventLog(TTI, 8, m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 8, eventId, m_RSUId, -1, -1);
				}
			}
			
		}
	}
}


void cRSU::DRASelectBasedOnP13(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec) {
}

void cRSU::DRASelectBasedOnP23(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec) {
}

void cRSU::DRASelectBasedOnP123(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec) {
	/*  EMERGENCY  */

	vector<int> curAvaliableEmergencyPatternIdx(gc_DRAPatternTypeNum);//��ǰ���õ�EmergencyPattern���

	for (int patternIdx = 0;patternIdx < gc_DRAEmergencyTotalPatternNum;patternIdx++) {
		if (m_DRAEmergencyPatternIsAvailable[patternIdx]) {
			curAvaliableEmergencyPatternIdx.push_back(patternIdx);
		}
	}

	for (int eventId : m_DRAEmergencyAdmitEventIdList) {
		int VeUEId = systemEventVec[eventId].VeUEId;

		//Ϊ��ǰ�û��ڿ��õ�EmergencyPattern�������ѡ��һ����ÿ���û��������ѡ�����EmergencyPattern��
		int patternIdx = systemVeUEVec[VeUEId].RBEmergencySelectBasedOnP2(curAvaliableEmergencyPatternIdx);
		
		if (patternIdx == -1) {//�޶�ӦPattern���͵�pattern��Դ����
			pushToEmergencyWaitEventIdList(eventId);
			
			//���¸��¼�����־
			systemEventVec[eventId].addEventLog(TTI, 31, m_RSUId, -1, -1);

			//��¼TTI��־
			DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 31, eventId, m_RSUId, -1, -1);

			continue;
		}

		//����Դ���Ϊռ��
		m_DRAEmergencyPatternIsAvailable[patternIdx] = false;

		//��������Ϣѹ��m_DRAEmergencyTransimitEventIdList��
		list<tuple<int, int>> scheduleIntervalList = buildEmergencyScheduleInterval(TTI, systemEventVec[eventId]);
		pushToEmergencyTransmitScheduleInfoList(new sDRAScheduleInfo(eventId, VeUEId, m_RSUId, patternIdx, scheduleIntervalList),patternIdx);
		newCount++;
	}
	pullFromEmergencyScheduleInfoTable();
	/*  EMERGENCY  */

	
	//��ʼ�����Emergency���¼�
	int roundATTI = TTI%gc_DRA_NTTI;//��TTIӳ�䵽[0-gc_DRA_NTTI)�ķ�Χ

	int clusterIdx = DRAGetClusterIdx(TTI);

	/*
	* ��ǰTTI���õ�Pattern����
	* �±�����Pattern����ı��
	* ÿ���ڲ�vector���������Pattern���õ�Pattern���
	*/
	vector<vector<int>> curAvaliablePatternIdx(gc_DRAPatternTypeNum);

	for (int patternTypeIdx = 0; patternTypeIdx < gc_DRAPatternTypeNum; patternTypeIdx++) {
		for (int patternIdx : gc_DRAPatternIdxTable[patternTypeIdx]) {
			if (m_DRAPatternIsAvailable[clusterIdx][patternIdx]) {
				curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
			}
		}
	}

	for (int eventId : m_DRAAdmitEventIdList) {//�����ô��ڽ��������е��¼�

		int VeUEId = systemEventVec[eventId].VeUEId;

		//Ϊ��ǰ�û��ڿ��õĶ�Ӧ���¼����͵�Pattern�������ѡ��һ����ÿ���û��������ѡ�����Pattern��
		int patternIdx = systemVeUEVec[VeUEId].RBSelectBasedOnP2(curAvaliablePatternIdx, systemEventVec[eventId].message.messageType);
		
		if (patternIdx == -1) {//���û��������Ϣ����û��patternʣ����
			pushToWaitEventIdList(eventId);

			//���¸��¼�����־
			systemEventVec[eventId].addEventLog(TTI, 11, m_RSUId, -1, -1);

			//��¼TTI��־
			DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 11, eventId, m_RSUId, -1, -1);
			continue;
		}

		//����Դ���Ϊռ��
		m_DRAPatternIsAvailable[clusterIdx][patternIdx] = false;

		//��������Ϣѹ��m_DRATransimitEventIdList��
		list<tuple<int,int>> scheduleIntervalList = buildScheduleIntervalList(TTI, systemEventVec[eventId], m_DRAClusterTDRInfo[clusterIdx]);
		pushToTransmitScheduleInfoList(new sDRAScheduleInfo(eventId, VeUEId, m_RSUId, patternIdx, scheduleIntervalList), patternIdx);
		newCount++;
	}

	//�����ȱ��е�ǰ���Լ���������û�ѹ�봫��������
	pullFromScheduleInfoTable(TTI);
}

void cRSU::DRADelaystatistics(int TTI,std::vector<sEvent>& systemEventVec) {
	/*
	* �ú���Ӧ����DRASelectBasedOnP..֮�����
	* ��ʱ�¼����ڵȴ����������������ߵ��ȱ��У��ǵ�ǰ�أ�
	*/
	
	//����ȴ�����
	/*  EMERGENCY  */
	for(int eventId:m_DRAEmergencyWaitEventIdList)
		systemEventVec[eventId].queuingDelay++;
	/*  EMERGENCY  */	
	for (int eventId : m_DRAWaitEventIdList)
		systemEventVec[eventId].queuingDelay++;

	//����˿����ڽ�Ҫ����Ĵ�������
	/*  EMERGENCY  */
	for (int patternIdx = 0;patternIdx < gc_DRAEmergencyTotalPatternNum;patternIdx++)
		for (sDRAScheduleInfo* &p : m_DRAEmergencyTransimitScheduleInfoList[patternIdx])
			systemEventVec[p->eventId].sendDelay++;
	/*  EMERGENCY  */
	for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
		for (sDRAScheduleInfo* &p : m_DRATransimitScheduleInfoList[patternIdx])
			systemEventVec[p->eventId].sendDelay++;
	}

	//����˿�λ�ڵ��ȱ��У��������ڵ�ǰ�ص��¼�
	int curClusterIdx = DRAGetClusterIdx(TTI);
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++) {
		if (clusterIdx == curClusterIdx) continue;
		for (sDRAScheduleInfo *p : m_DRAScheduleInfoTable[clusterIdx]) {
			if (p == nullptr) continue;
			systemEventVec[p->eventId].queuingDelay++;
		}		
	}
}

void cRSU::DRAConflictListener(int TTI, std::vector<sEvent>& systemEventVec) {
	/*  EMERGENCY  */
	for (int patternIdx = 0;patternIdx < gc_DRAEmergencyTotalPatternNum;patternIdx++) {
		list<sDRAScheduleInfo*> &lst = m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
		if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
			for (sDRAScheduleInfo* &info : lst) {
				//���¸��¼�����־
				systemEventVec[info->eventId].addEventLog(TTI, 30, m_RSUId, -1, patternIdx);

				//���Ƚ��¼�ѹ��ȴ��б�
				pushToEmergencyWaitEventIdList(info->eventId);

				//���¸��¼�����־
				systemEventVec[info->eventId].addEventLog(TTI, 29, m_RSUId, -1, -1);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 29, info->eventId, m_RSUId, -1, -1);

				//�ͷŵ�����Ϣ������ڴ���Դ
				delete info;
				info = nullptr;
				deleteCount++;
			}

			//�ͷ�Pattern��Դ
			m_DRAEmergencyPatternIsAvailable[patternIdx] = true;
		}
		else if (lst.size() == 1) {
			sDRAScheduleInfo *info = *lst.begin();

			//���¸��¼�����־
			systemEventVec[info->eventId].addEventLog(TTI, 30, m_RSUId, -1, patternIdx);

			//ά��ռ������
			tuple<int, int> &scheduleInterval = *(info->occupiedIntervalList.begin());

			++get<0>(scheduleInterval);
			if (get<0>(scheduleInterval) > get<1>(scheduleInterval)) {//�Ѿ�������ϣ�����Դ�ͷ�

				//���¸��¼�����־
				systemEventVec[info->eventId].addEventLog(TTI, 0, m_RSUId, -1, patternIdx);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 0, info->eventId, m_RSUId, -1, patternIdx);

				//�ͷŵ�����Ϣ������ڴ���Դ
				delete *lst.begin();
				deleteCount++;

				//�ͷ�Pattern��Դ
				m_DRAEmergencyPatternIsAvailable[patternIdx] = true;
			}
			else {//��δ�������
				pushToEmergencyScheduleInfoTable(patternIdx, *lst.begin());
				*lst.begin() = nullptr;
			}
		}
		else {//û�н����¼��ڸ�pattern�ϴ���
			//doting nothing
			continue;
		}
		lst.clear();
	}
	/*  EMERGENCY  */


	int clusterIdx = DRAGetClusterIdx(TTI);
	for (int patternIdx = 0;patternIdx < gc_DRATotalPatternNum;patternIdx++) {
		list<sDRAScheduleInfo*> &lst = m_DRATransimitScheduleInfoList[patternIdx];
		if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
			for (sDRAScheduleInfo* &info : lst) {
				//���¸��¼�����־
				systemEventVec[info->eventId].addEventLog(TTI, 10, m_RSUId, clusterIdx, patternIdx);

				//���Ƚ��¼�ѹ��ȴ��б�
				pushToWaitEventIdList(info->eventId);
	
				//���¸��¼�����־
				systemEventVec[info->eventId].addEventLog(TTI, 9, m_RSUId, -1, -1);
				
				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 9, info->eventId, m_RSUId, -1, -1);

				//�ͷŵ�����Ϣ������ڴ���Դ
				delete info;
				info = nullptr;
				deleteCount++;
			}
			//�ͷ�Pattern��Դ
			m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

		}
		else if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
			
			sDRAScheduleInfo *info = *lst.begin();

			//���¸��¼�����־
			systemEventVec[info->eventId].addEventLog(TTI, 10, m_RSUId, clusterIdx, patternIdx);

			//ά��ռ������
			list<tuple<int, int>> &OIList = info->occupiedIntervalList;

			tuple<int, int> &firstInterval = *OIList.begin();
			get<0>(firstInterval)++;
			if (get<0>(firstInterval) > get<1>(firstInterval)) 
				OIList.erase(OIList.begin());//ɾ����һ������
					
			if(OIList.size()==0){//˵���������Ѿ��������

				//���¸��¼�����־
				systemEventVec[info->eventId].addEventLog(TTI,0, m_RSUId,clusterIdx,patternIdx);
				
				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 0, info->eventId, m_RSUId, clusterIdx, patternIdx);

				//�ͷŵ�����Ϣ������ڴ���Դ
				delete *lst.begin();
				deleteCount++;

				//�ͷ�Pattern��Դ
				m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;
			}
			else {//��������δ���꣬����ѹ��m_DRAScheduleInfoTable
				pushToScheduleInfoTable(clusterIdx, patternIdx, *lst.begin());
				*lst.begin() = nullptr;
			}
		}
		else {//û���¼��ڸ�pattern�ϴ���
			//do nothing
		}
		//������󣬽���pattern�ϵ�������գ���ʱҪ��������ǿգ�Ҫ������nullptrָ�룩
		lst.clear();
	}
}













