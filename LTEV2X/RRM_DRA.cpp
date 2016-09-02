/*
* =====================================================================================
*
*       Filename:  RRM_DRA.cpp
*
*    Description:  DRAģ��
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<tuple>
#include<vector>
#include<list>
#include<sstream>
#include<iomanip>
#include"RRM_DRA.h"
#include"Exception.h"

using namespace std;
string sDRAScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ eventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


std::string sDRAScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ ";
	ss << "[ eventId = " << left << setw(3) << eventId << " , VeUEId = " << left << setw(3) << VeUEId << " ]";
	ss << " : occupy Interval = { ";
	for (tuple<int, int> t : occupiedIntervalList) {
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] , ";
	}
	ss << "} }";
	return ss.str();
}


string VeUEAdapterDRA::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << m_HoldObj.m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << m_HoldObj.m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << m_HoldObj.m_ClusterIdx;
	ss << " , ScheduleInterval = [" << left << setw(3) << get<0>(m_ScheduleInterval) << "," << left << setw(3) << get<1>(m_ScheduleInterval) << "] }";
	return ss.str();
}


RSUAdapterDRA::RSUAdapterDRA(cRSU& _RSU) :m_HoldObj(_RSU) {

	//WRONG
	/*  EMERGENCY  */
	m_DRAEmergencyPatternIsAvailable = vector<bool>(gc_DRAEmergencyTotalPatternNum, true);
	m_DRAEmergencyScheduleInfoTable = vector<sDRAScheduleInfo*>(gc_DRAEmergencyTotalPatternNum);
	m_DRAEmergencyTransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(gc_DRAEmergencyTotalPatternNum);
	/*  EMERGENCY  */



	m_DRAPatternIsAvailable = vector<vector<bool>>(m_HoldObj.m_DRAClusterNum, vector<bool>(gc_DRATotalPatternNum, true));
	m_DRAScheduleInfoTable = vector<vector<sDRAScheduleInfo*>>(m_HoldObj.m_DRAClusterNum, vector<sDRAScheduleInfo*>(gc_DRATotalPatternNum, nullptr));
	m_DRATransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(gc_DRATotalPatternNum, list<sDRAScheduleInfo*>(0, nullptr));
}


int RSUAdapterDRA::DRAGetClusterIdx(int TTI) {
	int roundATTI = TTI%gc_DRA_NTTI; //��TTIӳ�䵽[0-gc_DRA_NTTI)�ķ�Χ
	for (int clusterIdx = 0; clusterIdx < m_HoldObj.m_DRAClusterNum; clusterIdx++) 
		if (roundATTI <= get<1>(m_DRAClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


string RSUAdapterDRA::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	//������Ϣ
	ss << indent << "RSU[" << m_HoldObj.m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//��ʼ��ӡVeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < m_HoldObj.m_DRAClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx]) {
			if (cnt % 10 == 0)
				ss << indent << "            [ ";
			ss << left << setw(3) << RSUId << " , ";
			if (cnt % 10 == 9)
				ss << " ]" << endl;
			cnt++;
		}
		if (cnt != 0 && cnt % 10 != 0)
			ss << " ]" << endl;
		ss << indent << "        " << "}" << endl;
	}
	ss << indent << "    " << "}" << endl;

	//��ʼ��ӡclusterInfo
	ss << indent << "    " << "clusterInfo :" << endl;
	ss << indent << "    " << "{" << endl;

	ss << indent << "        ";
	for (const tuple<int, int, int>&t : m_DRAClusterTDRInfo)
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << endl;
	ss << indent << "    " << "}" << endl;


	//������Ϣ
	ss << indent << "}" << endl;
	return ss.str();
}




RRM_DRA::RRM_DRA(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, eDRAMode m_DRAMode) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList), m_DRAMode(m_DRAMode) {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAdapterVec.push_back(VeUEAdapterDRA(m_VeUEAry[VeUEId]));
	}
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAdapterVec.push_back(RSUAdapterDRA(m_RSUAry[RSUId]));
	}
}


void RRM_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//��Դ������Ϣ���:����ÿ��RSU�ڵĽ��������
	DRAInformationClean();

	//  WRONG
	//���ݴش�С����ʱ����Դ�Ļ���
	DRAGroupSizeBasedTDM(clusterFlag);
	//  WRONG

	//������������
	DRAUpdateAdmitEventIdList(clusterFlag);

	//��ǰm_TTI��DRA�㷨
	switch (m_DRAMode) {
	case P13:
		DRASelectBasedOnP13();
		break;
	case P23:
		DRASelectBasedOnP23();
		break;
	case P123:
		DRASelectBasedOnP123();
		break;
	}
	//д�������Ϣ
	DRAWriteScheduleInfo(g_OutDRAScheduleInfo);

	//ͳ��ʱ����Ϣ
	DRADelaystatistics();

	//֡����ͻ
	DRAConflictListener();
}


void RRM_DRA::DRAInformationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];
		_RSUAdapterDRA.m_DRAAdmitEventIdList.clear();
		_RSUAdapterDRA.m_DRAEmergencyAdmitEventIdList.clear();
	}
}


void RRM_DRA::DRAGroupSizeBasedTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];
		
		//�������������RSU����һ����ʱ
		if (_RSUAdapterDRA.m_HoldObj.m_VeUEIdList.size() == 0) {
			/*-----------------------ATTENTION-----------------------
			* ����ֵΪ(0,-1,0)�ᵼ�»�ȡ��ǰ�ر��ʧ�ܣ����������ط���Ҫ����
			* ���ֱ�Ӹ�ÿ���ض���ֵΪ�������䣬����Ҳû���κ����ã����������������
			*------------------------ATTENTION-----------------------*/
			_RSUAdapterDRA.m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSUAdapterDRA.m_HoldObj.m_DRAClusterNum, tuple<int, int, int>(0, gc_DRA_NTTI - 1, gc_DRA_NTTI));
			continue;
		}

		//��ʼ��
		_RSUAdapterDRA.m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSUAdapterDRA.m_HoldObj.m_DRAClusterNum, tuple<int, int, int>(0, 0, 0));

		//����ÿ��TTIʱ϶��Ӧ��VeUE��Ŀ(double)��!!!������!!��
		double VeUESizePerTTI = static_cast<double>(_RSUAdapterDRA.m_HoldObj.m_VeUEIdList.size()) / static_cast<double>(gc_DRA_NTTI);

		//clusterSize�洢ÿ���ص�VeUE��Ŀ(double)��!!!������!!��
		std::vector<double> clusterSize(_RSUAdapterDRA.m_HoldObj.m_DRAClusterNum, 0);

		//��ʼ��clusterSize
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++)
			clusterSize[clusterIdx] = static_cast<double>(_RSUAdapterDRA.m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx].size());

		//���ȸ�������һ�����Ĵط���һ��TTI
		int basicNTTI = 0;
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++) {
			//����ô���������һ��VeUE��ֱ�ӷ����һ����λ��ʱ����Դ
			if (_RSUAdapterDRA.m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx].size() != 0) {
				get<2>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) = 1;
				clusterSize[clusterIdx] -= VeUESizePerTTI;
				basicNTTI++;
			}
		}

		//���˸���Ϊ�յĴط����һ��TTI�⣬ʣ��ɷ����TTI����
		int remainNTTI = gc_DRA_NTTI - basicNTTI;

		//����ʣ�µ���Դ�飬ѭ������һʱ϶�������ǰ������ߵĴأ�����֮�󣬸��Ķ�Ӧ�ı�������ȥ��ʱ϶��Ӧ��VeUE����
		while (remainNTTI > 0) {
			int dex = DRAGetMaxIndex(clusterSize);
			if (dex == -1) throw Exp("������û�з����ʱ����Դ������ÿ�����ڵ�VeUE�Ѿ�Ϊ����");
			get<2>(_RSUAdapterDRA.m_DRAClusterTDRInfo[dex])++;
			remainNTTI--;
			clusterSize[dex] -= VeUESizePerTTI;
		}

		//��ʼ�������䷶Χ��������
		get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[0]) = 0;
		get<1>(_RSUAdapterDRA.m_DRAClusterTDRInfo[0]) = get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[0]) + get<2>(_RSUAdapterDRA.m_DRAClusterTDRInfo[0]) - 1;
		for (int clusterIdx = 1; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++) {
			get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) = get<1>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx - 1]) + 1;
			get<1>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) = get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) + get<2>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) - 1;
		}


		//����������д���RSU�ڵ�ÿһ������
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSUAdapterDRA.m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx])
				m_VeUEAdapterVec[VeUEId].m_ScheduleInterval = tuple<int, int>(get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]), get<1>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]));
		}
	}

	writeClusterPerformInfo(g_OutClasterPerformInfo);
}


void RRM_DRA::DRAUpdateAdmitEventIdList(bool clusterFlag) {
	//���ȣ�����System������¼���������
	DRAProcessEventList();
	//��Σ������ǰTTI������λ�ø��£���Ҫ������ȱ�
	if (clusterFlag) {
		if (m_DRASwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
		//����RSU����ĵ�������
		DRAProcessScheduleInfoTableWhenLocationUpdate();

		//����RSU����ĵȴ�����
		DRAProcessWaitEventIdListWhenLocationUpdate();

		//����System������л�����
		DRAProcessSwitchListWhenLocationUpdate();
		if (m_DRASwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
	}

	//Ȼ�󣬴���RSU����ĵȴ�����
	DRAProcessWaitEventIdList();
}


void RRM_DRA::DRAProcessEventList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);//��ǰ�ɴ������ݵĴر��
		for (int eventId : m_EventTTIList[m_TTI]) {
			sEvent event = m_EventVec[eventId];
			
			int VeUEId = event.VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//��ǰ�¼���Ӧ��VeUE���ڵ�ǰRSU�У���������
				continue;
			}
			else {//��ǰ�¼���Ӧ��VeUE�ڵ�ǰRSU��
				if (m_EventVec[eventId].message.messageType == EMERGENCY) {//���ǽ������¼�
					/*  EMERGENCY  */
					//�����¼�ѹ������¼��ȴ�����
					_RSUAdapterDRA.DRAPushToEmergencyWaitEventIdList(eventId);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
					/*  EMERGENCY  */
				}
				else {//�ǽ������¼�
				    //�����¼�ѹ��ȴ�����
					_RSUAdapterDRA.DRAPushToWaitEventIdList(eventId);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
				}
			}
		}
	}
}


void RRM_DRA::DRAProcessScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			if (_RSUAdapterDRA.m_DRAEmergencyScheduleInfoTable[patternIdx] == nullptr) {//��ǰEmergencyPattern���¼�����
				continue;
			}
			else {
				int eventId = _RSUAdapterDRA.m_DRAEmergencyScheduleInfoTable[patternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
					_RSUAdapterDRA.DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);

					//���ͷŸõ�����Ϣ����Դ
					delete _RSUAdapterDRA.m_DRAEmergencyScheduleInfoTable[patternIdx];
					deleteCount++;
					_RSUAdapterDRA.m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;

					//�ͷ�Pattern��Դ
					_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] = true;

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);
				}
				else {//�ó������ڵ�ǰRSU��
					  //doing nothing 
					continue;
				}
			}
		}
		/*  EMERGENCY  */

		//��ʼ���� m_DRAScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
				if (_RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx] == nullptr) {//��ǰPattern�����¼��ڴ���
					continue;
				}
				else {
					int eventId = _RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx]->eventId;
					int VeUEId = m_EventVec[eventId].VeUEId;
					if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
						_RSUAdapterDRA.DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);

						//���ͷŸõ�����Ϣ����Դ
						delete _RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx];
						deleteCount++;
						_RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

						//�ͷ�Pattern��Դ
						_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

						//���¸��¼�����־
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

						//��¼TTI��־
						DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);
					}
					else {
						if (_RSUAdapterDRA.DRAGetClusterIdxOfVeUE(VeUEId) != clusterIdx) {//RSU�ڲ������˴��л�������ӵ��ȱ���ȡ����ѹ��ȴ�����
							_RSUAdapterDRA.DRAPushToWaitEventIdList(eventId);

							//���ͷŸõ�����Ϣ����Դ
							delete _RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx];
							deleteCount++;
							_RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

							//�ͷ�Pattern��Դ
							_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

							//���¸��¼�����־
							m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

							//��¼TTI��־
							DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);
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
}


void RRM_DRA::DRAProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		//��ʼ����m_DRAEmergencyWaitEventIdList
		list<int>::iterator it = _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList.begin();
		while (it != _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
				_RSUAdapterDRA.DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);//������ӵ�System�����RSU�л�������
				it = _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList.erase(it);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
			}
			else {//��Ȼ���ڵ�ǰRSU��Χ��
				it++;
				continue; //�������ڵ�ǰRSU��Emergency�ȴ�����
			}
		}
		/*  EMERGENCY  */


		//��ʼ���� m_DRAWaitEventIdList
		it = _RSUAdapterDRA.m_DRAWaitEventIdList.begin();
		while (it != _RSUAdapterDRA.m_DRAWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
				_RSUAdapterDRA.DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);//������ӵ�System�����RSU�л�������
				it = _RSUAdapterDRA.m_DRAWaitEventIdList.erase(it);//����ӵȴ�������ɾ��

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
			}
			else {//��Ȼ���ڵ�ǰRSU��Χ��
				it++;
				continue; //�������ڵ�ǰRSU�ĵȴ�����
			}
		}
	}
}


void RRM_DRA::DRAProcessSwitchListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		list<int>::iterator it = m_DRASwitchEventIdList.begin();
		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);//��ǰ�ɴ������ݵĴر��
		while (it !=m_DRASwitchEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//���л������е��¼���Ӧ��VeUE�������ڵ�ǰ�أ���������
				it++;
				continue;
			}
			else {//���л������е�VeUE�����ڵ�RSU
				  /*  EMERGENCY  */
				if (m_EventVec[eventId].message.messageType == EMERGENCY) {//���ڽ����¼�
					//ת��ȴ�����
					_RSUAdapterDRA.DRAPushToEmergencyWaitEventIdList(eventId);

					//��Switch���н���ɾ��
					it = m_DRASwitchEventIdList.erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
				}
				/*  EMERGENCY  */

				else {//�ǽ����¼�
					//ת��ȴ�����
					_RSUAdapterDRA.DRAPushToWaitEventIdList(eventId);

					//��Switch���н���ɾ��
					it = m_DRASwitchEventIdList.erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
				}
			}
		}
	}
}


void RRM_DRA::DRAProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		for (int eventId : _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList) {
			_RSUAdapterDRA.DRAPushToEmergencyAdmitEventIdList(eventId);//���¼�ѹ���������

		    //���¸��¼�����־
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

			//��¼TTI��־
			DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
		}

		//���ڻ�ȫ��ѹ��Emergency�����������ֱ����վ����ˣ�����Ҫһ����ɾ��
		_RSUAdapterDRA.m_DRAEmergencyWaitEventIdList.clear();
		/*  EMERGENCY  */


		//��ʼ���� m_DRAWaitEventIdList
		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);//���ڵ����еĴر��
		list<int>::iterator it = _RSUAdapterDRA.m_DRAWaitEventIdList.begin();
		while (it != _RSUAdapterDRA.m_DRAWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_ClusterIdx == clusterIdx) {//���¼���ǰ���Խ��е���
				_RSUAdapterDRA.DRAPushToAdmitEventIdList(eventId);//��ӵ�RSU����Ľ���������
				it = _RSUAdapterDRA.m_DRAWaitEventIdList.erase(it);//����ӵȴ�������ɾ��

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
			}
			else {//���¼���ǰ�����Խ��е���
				it++;
				continue; //�����ȴ�����ǰTTI��������
			}
		}
	}
}


void RRM_DRA::DRASelectBasedOnP13() {
	
}


void RRM_DRA::DRASelectBasedOnP23() {

}


void RRM_DRA::DRASelectBasedOnP123() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		//WRONG
		//vector<int> curAvaliableEmergencyPatternIdx(gc_DRAPatternTypeNum);//��ǰ���õ�EmergencyPattern���
		vector<int> curAvaliableEmergencyPatternIdx;//��ǰ���õ�EmergencyPattern���

		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			if (_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx]) {
				curAvaliableEmergencyPatternIdx.push_back(patternIdx);
			}
		}

		for (int eventId : _RSUAdapterDRA.m_DRAEmergencyAdmitEventIdList) {
			int VeUEId = m_EventVec[eventId].VeUEId;

			//Ϊ��ǰ�û��ڿ��õ�EmergencyPattern�������ѡ��һ����ÿ���û��������ѡ�����EmergencyPattern��
			int patternIdx = m_VeUEAdapterVec[VeUEId].RBEmergencySelectBasedOnP2(curAvaliableEmergencyPatternIdx);

			if (patternIdx == -1) {//�޶�ӦPattern���͵�pattern��Դ����
				_RSUAdapterDRA.DRAPushToEmergencyWaitEventIdList(eventId);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				continue;
			}

			//����Դ���Ϊռ��
			_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] = false;

			//��������Ϣѹ��m_DRAEmergencyTransimitEventIdList��
			list<tuple<int, int>> scheduleIntervalList = DRABuildEmergencyScheduleInterval(m_TTI, m_EventVec[eventId]);
			_RSUAdapterDRA.DRAPushToEmergencyTransmitScheduleInfoList(new sDRAScheduleInfo(eventId, VeUEId, _RSUAdapterDRA.m_HoldObj.m_RSUId, patternIdx, scheduleIntervalList), patternIdx);
			newCount++;
		}
		_RSUAdapterDRA.DRAPullFromEmergencyScheduleInfoTable();
		/*  EMERGENCY  */


		//��ʼ�����Emergency���¼�
		int roundATTI = m_TTI%gc_DRA_NTTI;//��TTIӳ�䵽[0-gc_DRA_NTTI)�ķ�Χ

		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);

		/*
		* ��ǰTTI���õ�Pattern����
		* �±�����Pattern����ı��
		* ÿ���ڲ�vector���������Pattern���õ�Pattern���
		*/
		vector<vector<int>> curAvaliablePatternIdx(gc_DRAPatternTypeNum);

		for (int patternTypeIdx = 0; patternTypeIdx < gc_DRAPatternTypeNum; patternTypeIdx++) {
			for (int patternIdx : gc_DRAPatternIdxTable[patternTypeIdx]) {
				if (_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		for (int eventId : _RSUAdapterDRA.m_DRAAdmitEventIdList) {//�����ô��ڽ��������е��¼�

			int VeUEId = m_EventVec[eventId].VeUEId;

			//Ϊ��ǰ�û��ڿ��õĶ�Ӧ���¼����͵�Pattern�������ѡ��һ����ÿ���û��������ѡ�����Pattern��
			int patternIdx = m_VeUEAdapterVec[VeUEId].RBSelectBasedOnP2(curAvaliablePatternIdx, m_EventVec[eventId].message.messageType);

			if (patternIdx == -1) {//���û��������Ϣ����û��patternʣ����
				_RSUAdapterDRA.DRAPushToWaitEventIdList(eventId);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//��¼TTI��־
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
				continue;
			}

			//����Դ���Ϊռ��
			_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = false;

			//��������Ϣѹ��m_DRATransimitEventIdList��
			list<tuple<int, int>> scheduleIntervalList = DRABuildScheduleIntervalList(m_TTI, m_EventVec[eventId], _RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]);
			_RSUAdapterDRA.DRAPushToTransmitScheduleInfoList(new sDRAScheduleInfo(eventId, VeUEId, _RSUAdapterDRA.m_HoldObj.m_RSUId, patternIdx, scheduleIntervalList), patternIdx);
			newCount++;
		}

		//�����ȱ��е�ǰ���Լ���������û�ѹ�봫��������
		_RSUAdapterDRA.DRAPullFromScheduleInfoTable(m_TTI);

	}
}


void RRM_DRA::DRAWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];
		
		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);
		out << "    RSU[" << _RSUAdapterDRA.m_HoldObj.m_RSUId << "] :" << endl;
		out << "    {" << endl;
		out << "    EMERGENCY:" << endl;
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] ? "Available" : "Unavailable") << endl;
			for (sDRAScheduleInfo* info : _RSUAdapterDRA.m_DRAEmergencyTransimitScheduleInfoList[patternIdx]) {
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    PERIOD:" << endl;
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[PERIOD]; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] ? "Available" : "Unavailable") << endl;
			for (sDRAScheduleInfo* info : _RSUAdapterDRA.m_DRATransimitScheduleInfoList[patternIdx]) {
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    DATA:" << endl;
		for (int patternIdx = gc_DRAPatternNumPerPatternType[PERIOD]; patternIdx < gc_DRAPatternNumPerPatternType[PERIOD] + gc_DRAPatternNumPerPatternType[DATA]; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] ? "Available" : "Unavailable") << endl;
			for (sDRAScheduleInfo* info : _RSUAdapterDRA.m_DRATransimitScheduleInfoList[patternIdx]) {
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    }" << endl;

	}
	out << "}" << endl;
	out << "\n\n" << endl;
}


void RRM_DRA::DRADelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*
		* �ú���Ӧ����DRASelectBasedOnP..֮�����
		* ��ʱ�¼����ڵȴ����������������ߵ��ȱ��У��ǵ�ǰ�أ�
		*/

		//����ȴ�����
		/*  EMERGENCY  */
		for (int eventId : _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;
		/*  EMERGENCY  */
		for (int eventId : _RSUAdapterDRA.m_DRAWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//����˿����ڽ�Ҫ����Ĵ�������
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++)
			for (sDRAScheduleInfo* &p : _RSUAdapterDRA.m_DRAEmergencyTransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			for (sDRAScheduleInfo* &p : _RSUAdapterDRA.m_DRATransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		}

		//����˿�λ�ڵ��ȱ��У��������ڵ�ǰ�ص��¼�
		int curClusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++) {
			if (clusterIdx == curClusterIdx) continue;
			for (sDRAScheduleInfo *p : _RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx]) {
				if (p == nullptr) continue;
				m_EventVec[p->eventId].queuingDelay++;
			}
		}
	}
}

void RRM_DRA::DRAConflictListener() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			list<sDRAScheduleInfo*> &lst = _RSUAdapterDRA.m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
				for (sDRAScheduleInfo* &info : lst) {
					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

					//���Ƚ��¼�ѹ��ȴ��б�
					_RSUAdapterDRA.DRAPushToEmergencyWaitEventIdList(info->eventId);

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
					deleteCount++;
				}

				//�ͷ�Pattern��Դ
				_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] = true;
			}
			else if (lst.size() == 1) {
				sDRAScheduleInfo *info = *lst.begin();

				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

				//ά��ռ������
				tuple<int, int> &scheduleInterval = *(info->occupiedIntervalList.begin());

				++get<0>(scheduleInterval);
				if (get<0>(scheduleInterval) > get<1>(scheduleInterval)) {//�Ѿ�������ϣ�����Դ�ͷ�

					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete *lst.begin();
					deleteCount++;

					//�ͷ�Pattern��Դ
					_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] = true;
				}
				else {//��δ�������
					_RSUAdapterDRA.DRAPushToEmergencyScheduleInfoTable(patternIdx, *lst.begin());
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


		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			list<sDRAScheduleInfo*> &lst = _RSUAdapterDRA.m_DRATransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
				for (sDRAScheduleInfo* &info : lst) {
					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

					//���Ƚ��¼�ѹ��ȴ��б�
					_RSUAdapterDRA.DRAPushToWaitEventIdList(info->eventId);

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
					deleteCount++;
				}
				//�ͷ�Pattern��Դ
				_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

			}
			else if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��

				sDRAScheduleInfo *info = *lst.begin();

				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

				//ά��ռ������
				list<tuple<int, int>> &OIList = info->occupiedIntervalList;

				tuple<int, int> &firstInterval = *OIList.begin();
				get<0>(firstInterval)++;
				if (get<0>(firstInterval) > get<1>(firstInterval))
					OIList.erase(OIList.begin());//ɾ����һ������

				if (OIList.size() == 0) {//˵���������Ѿ��������

					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

					//��¼TTI��־
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete *lst.begin();
					deleteCount++;

					//�ͷ�Pattern��Դ
					_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;
				}
				else {//��������δ���꣬����ѹ��m_DRAScheduleInfoTable
					_RSUAdapterDRA.DRAPushToScheduleInfoTable(clusterIdx, patternIdx, *lst.begin());
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
}


void RRM_DRA::DRAWriteTTILogInfo(std::ofstream& out, int TTI, eEventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx) {
	stringstream ss;
	switch (type) {
	case SUCCEED:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]   ClusterIdx[" << clusterIdx << "]    PatternIdx[" << patternIdx << "] }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[0]Succeed";
		out << "    " << ss.str() << endl;
		break;
	case EVENT_TO_WAIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[2]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[3]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[4]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_SWITCH:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[5]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_ADMIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[6]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SWITCH_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[8]Switch";
		out << "    " << ss.str() << endl;
		break;
	case TRANSIMIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]'s TransimitScheduleInfoList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[9]Conflict";
		out << "    " << ss.str() << endl;
		break;
	case ADMIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]'s AdmitEventIdList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[11]Conflict";
		out << "    " << ss.str() << endl;
		break;
	}
}


void RRM_DRA::writeClusterPerformInfo(std::ofstream &out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;

	//��ӡVeUE��Ϣ
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUEAdapterDRA &_VeUE = m_VeUEAdapterVec[VeUEId];
		out << _VeUE.toString(2) << endl;
	}
	out << "    }\n" << endl;

	////��ӡ��վ��Ϣ
	//out << "    eNB Info: " << endl;
	//out << "    {" << endl;
	//for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
	//	ceNB &_eNB = m_eNBAry[eNBId];
	//	out << _eNB.toString(2) << endl;
	//}
	//out << "    }\n" << endl;

	//��ӡRSU��Ϣ
	out << "    RSU Info: " << endl;
	out << "    {" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSU = m_RSUAdapterVec[RSUId];
		out << _RSU.toString(2) << endl;
	}
	out << "    }" << endl;

	out << "}\n\n";
}


int RRM_DRA::DRAGetMaxIndex(const std::vector<double>&clusterSize) {
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


list<tuple<int, int>> RRM_DRA::DRABuildScheduleIntervalList(int TTI, const sEvent& event, std::tuple<int, int, int>ClasterTTI) {
	list<tuple<int, int>> scheduleIntervalList;
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


std::list<std::tuple<int, int>> RRM_DRA::DRABuildEmergencyScheduleInterval(int TTI, const sEvent& event) {
	std::list<std::tuple<int, int>> scheduleIntervalList;
	std::tuple<int, int> scheduleInterval;
	int eventId = event.eventId;
	int occupiedTTI = event.message.bitNum / gc_BitNumPerRB / gc_DRAEmergencyFBNumPerPattern;
	get<0>(scheduleInterval) = TTI;
	get<1>(scheduleInterval) = TTI + occupiedTTI - 1;
	scheduleIntervalList.push_back(scheduleInterval);
	return scheduleIntervalList;
}



