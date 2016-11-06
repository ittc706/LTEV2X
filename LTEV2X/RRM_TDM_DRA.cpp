/*
* =====================================================================================
*
*       Filename:  RRM_TDM_DRA.cpp
*
*    Description:  RRM_TDM_DRAģ��
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
#include<set>
#include"RRM_TDM_DRA.h"
#include"Exception.h"

using namespace std;

RRM_TDM_DRA::RRM_TDM_DRA(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {

	m_InterferenceVec = vector<list<int>>(ns_RRM_TDM_DRA::gc_TotalPatternNum);
	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num,(threadIdx+1)*num-1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = m_Config.RSUNum - 1;//�������һ���߽�
}


void RRM_TDM_DRA::initialize() {
	//��ʼ��VeUE�ĸ�ģ���������
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeRRM_TDM_DRA();
	}

	//��ʼ��RSU�ĸ�ģ���������
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeRRM_TDM_DRA();
	}
}


void RRM_TDM_DRA::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();

		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(ns_RRM_TDM_DRA::gc_TotalPatternNum, false);
	}
}


void RRM_TDM_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//��Դ������Ϣ���:����ÿ��RSU�ڵĽ��������
	informationClean();

	//���ݴش�С����ʱ����Դ�Ļ���
	//groupSizeBasedTDM(clusterFlag);
	uniformTDM(clusterFlag);

	//������������
	updateAdmitEventIdList(clusterFlag);

	
	selectBasedOnP123();

	//ͳ��ʱ����Ϣ
	delaystatistics();

	//֡����ͻ
	conflictListener();

	//����������˵�Ԫ���������Ӧ����
	transimitPreparation();

	//ģ�⴫�俪ʼ�����µ�����Ϣ
	transimitStart();

	//д�������Ϣ
	writeScheduleInfo(g_FileDRAScheduleInfo);

	//ģ�⴫�������ͳ��������
	transimitEnd();
}


void RRM_TDM_DRA::informationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_RRM_TDM_DRA->m_AdmitEventIdList.clear();
		_RSU.m_RRM_TDM_DRA->m_EmergencyAdmitEventIdList.clear();
	}
}


void RRM_TDM_DRA::groupSizeBasedTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		
		//�������������RSU����һ����ʱ
		if (_RSU.m_GTT->m_VeUEIdList.size() == 0) {
			/*-----------------------ATTENTION-----------------------
			* ����ֵΪ(0,-1,0)�ᵼ�»�ȡ��ǰ�ر��ʧ�ܣ����������ط���Ҫ����
			* ���ֱ�Ӹ�ÿ���ض���ֵΪ�������䣬����Ҳû���κ����ã����������������
			*------------------------ATTENTION-----------------------*/
			_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTT->m_ClusterNum, tuple<int, int, int>(0, ns_RRM_TDM_DRA::gc_NTTI - 1, ns_RRM_TDM_DRA::gc_NTTI));
			continue;
		}

		//��ʼ��
		_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTT->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		//����ÿ��TTIʱ϶��Ӧ��VeUE��Ŀ(double)��!!!������!!��
		double VeUESizePerTTI = static_cast<double>(_RSU.m_GTT->m_VeUEIdList.size()) / static_cast<double>(ns_RRM_TDM_DRA::gc_NTTI);

		//clusterSize�洢ÿ���ص�VeUE��Ŀ(double)��!!!������!!��
		vector<double> clusterSize(_RSU.m_GTT->m_ClusterNum, 0);

		//��ʼ��clusterSize
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++)
			clusterSize[clusterIdx] = static_cast<double>(_RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx].size());

		//���ȸ�������һ�����Ĵط���һ��TTI
		int basicNTTI = 0;
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//����ô���������һ��VeUE��ֱ�ӷ����һ����λ��ʱ����Դ
			if (_RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx].size() != 0) {
				get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) = 1;
				clusterSize[clusterIdx] -= VeUESizePerTTI;
				basicNTTI++;
			}
		}

		//���˸���Ϊ�յĴط����һ��TTI�⣬ʣ��ɷ����TTI����
		int remainNTTI = ns_RRM_TDM_DRA::gc_NTTI - basicNTTI;

		//����ʣ�µ���Դ�飬ѭ������һʱ϶�������ǰ������ߵĴأ�����֮�󣬸��Ķ�Ӧ�ı�������ȥ��ʱ϶��Ӧ��VeUE����
		while (remainNTTI > 0) {
			int dex = getMaxIndex(clusterSize);
			if (dex == -1) throw Exp("������û�з����ʱ����Դ������ÿ�����ڵ�VeUE�Ѿ�Ϊ����");
			get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[dex])++;
			remainNTTI--;
			clusterSize[dex] -= VeUESizePerTTI;
		}

		//��ʼ�������䷶Χ��������
		get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[0]) = 0;
		get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[0]) = get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[0]) + get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[0]) - 1;
		for (int clusterIdx = 1; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) = get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx - 1]) + 1;
			get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) = get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) + get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) - 1;
		}


		//����������д���RSU�ڵ�ÿһ������
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId].m_RRM_TDM_DRA->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]), get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]));
		}
	}
	writeClusterPerformInfo(g_FileClasterPerformInfo);
}


void RRM_TDM_DRA::uniformTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		//��ʼ��
		_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTT->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		int equalTimeLength = ns_RRM_TDM_DRA::gc_NTTI / _RSU.m_GTT->m_ClusterNum;

		int lastClusterLength = ns_RRM_TDM_DRA::gc_NTTI - equalTimeLength*_RSU.m_GTT->m_ClusterNum + equalTimeLength;

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx] = tuple<int, int, int>(equalTimeLength*clusterIdx, equalTimeLength*(clusterIdx + 1) - 1, equalTimeLength);
		}

		//�޸����һ���ص�ʱ�򳤶ȣ���Ϊƽ���س����ܱ����������ˣ���ˣ�ƽ���س���*�������������ܵ���ʱ�䣬��˽������������һ����
		get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[_RSU.m_GTT->m_ClusterNum - 1]) = ns_RRM_TDM_DRA::gc_NTTI - 1;
		get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[_RSU.m_GTT->m_ClusterNum - 1]) = lastClusterLength;

		

		//����������д���RSU�ڵ�ÿһ������
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId].m_RRM_TDM_DRA->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]), get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]));
		}
	}
	writeClusterPerformInfo(g_FileClasterPerformInfo);
}


void RRM_TDM_DRA::updateAdmitEventIdList(bool clusterFlag) {
	//���ȣ�����System������¼���������
	processEventList();
	//��Σ������ǰTTI������λ�ø��£���Ҫ������ȱ�
	if (clusterFlag) {
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
		//����RSU����ĵ�������
		processScheduleInfoTableWhenLocationUpdate();

		//����RSU����ĵȴ�����
		processWaitEventIdListWhenLocationUpdate();

		//����System������л�����
		processSwitchListWhenLocationUpdate();
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
	}
	//Ȼ�󣬴���RSU����ĵȴ�����
	processWaitEventIdList();
}


void RRM_TDM_DRA::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event &event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];

		if (m_EventVec[eventId].message.messageType == EMERGENCY) {//���ǽ������¼�
			/*  EMERGENCY  */
			//�����¼�ѹ������¼��ȴ�����
			_RSU.m_RRM_TDM_DRA->pushToEmergencyWaitEventIdList(eventId);

			//���¸��¼�����־
			m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "Trigger");

			//��¼TTI��־
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			/*  EMERGENCY  */
		}
		else {//�ǽ������¼�
			//�����¼�ѹ��ȴ�����
			_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(eventId);

			//���¸��¼�����־
			m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "Trigger");

			//��¼TTI��־
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
		}
	}
}


void RRM_TDM_DRA::processScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			if (_RSU.m_RRM_TDM_DRA->m_EmergencyScheduleInfoTable[patternIdx] == nullptr) continue;

			int eventId = _RSU.m_RRM_TDM_DRA->m_EmergencyScheduleInfoTable[patternIdx]->eventId;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
				//ѹ��Switch����
				_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

				//��ʣ�����bit����
				m_EventVec[eventId].message.reset();

				//���ͷŸõ�����Ϣ����Դ
				delete _RSU.m_RRM_TDM_DRA->m_EmergencyScheduleInfoTable[patternIdx];
				_RSU.m_RRM_TDM_DRA->m_EmergencyScheduleInfoTable[patternIdx] = nullptr;

				//�ͷ�Pattern��Դ
				_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx] = true;

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTT->m_RSUId, -1, patternIdx, "LocationUpdate");

				//��¼TTI��־
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, -1, patternIdx);
			}
		}
		/*  EMERGENCY  */

		//��ʼ���� m_ScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
				int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
				if (_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx] == nullptr) continue;


				int eventId = _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
					//ѹ��Switch����
					_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//���ͷŸõ�����Ϣ����Դ
					delete _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx];
					_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx] = nullptr;

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = true;

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, relativePatternIdx, "LocationUpdate");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, relativePatternIdx);
				}
				else {
					if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//RSU�ڲ������˴��л�������ӵ��ȱ���ȡ����ѹ��ȴ�����
						//ѹ���RSU�ĵȴ�����
						_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(eventId);

						//��ʣ�����bit����
						m_EventVec[eventId].message.reset();

						//���ͷŸõ�����Ϣ����Դ
						delete _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx];
						_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx] = nullptr;

						//�ͷ�Pattern��Դ
						_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = true;

						//���¸��¼�����־
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, relativePatternIdx, "LocationUpdate");

						//��¼TTI��־
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, relativePatternIdx);
					}
				}
			}
		}
	}
}


void RRM_TDM_DRA::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		//��ʼ����m_EmergencyWaitEventIdList
		list<int>::iterator it = _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList.begin();
		while (it != _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
                //������ӵ�System�����RSU�л�������
				_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);
				
                //�ӵȴ�������ɾ��
				it = _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList.erase(it);

				//��ʣ�����bit����
				m_EventVec[eventId].message.reset();

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

				//��¼TTI��־
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			}
			else {//��Ȼ���ڵ�ǰRSU��Χ��
				it++;
				continue; //�������ڵ�ǰRSU��Emergency�ȴ�����
			}
		}
		/*  EMERGENCY  */


		//��ʼ���� m_WaitEventIdList
		it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.begin();
		while (it != _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
				//������ӵ�System�����RSU�л�������
				_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);
				
			    //����ӵȴ�������ɾ��
				it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.erase(it);

				//��ʣ�����bit����
				m_EventVec[eventId].message.reset();

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

				//��¼TTI��־
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			}
			else {//��Ȼ���ڵ�ǰRSU��Χ��
				it++;
				continue; //�������ڵ�ǰRSU�ĵȴ�����
			}
		}
	}
}


void RRM_TDM_DRA::processSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_SwitchEventIdList.begin();
	while (it != m_SwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
																
		if (m_EventVec[eventId].message.messageType == EMERGENCY) {//���ڽ����¼�
			/*  EMERGENCY  */
			//ת��ȴ�����
			_RSU.m_RRM_TDM_DRA->pushToEmergencyWaitEventIdList(eventId);

			//��Switch���н���ɾ��
			it = m_SwitchEventIdList.erase(it);

			//���¸��¼�����־
			m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

			//��¼TTI��־
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			/*  EMERGENCY  */
		}else {//�ǽ����¼�
			 //ת��ȴ�����
			_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(eventId);

			//��Switch���н���ɾ��
			it = m_SwitchEventIdList.erase(it);

			//���¸��¼�����־
			m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

			//��¼TTI��־
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
		}
	}
}


void RRM_TDM_DRA::processWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList) {
			_RSU.m_RRM_TDM_DRA->pushToEmergencyAdmitEventIdList(eventId);//���¼�ѹ���������

		    //���¸��¼�����־
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTT->m_RSUId, -1, -1, "TryAccept");

			//��¼TTI��־
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
		}

		//���ڻ�ȫ��ѹ��Emergency�����������ֱ����վ����ˣ�����Ҫһ����ɾ��
		_RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList.clear();
		/*  EMERGENCY  */


		//��ʼ���� m_WaitEventIdList
		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);//���ڵ����еĴر��
		list<int>::iterator it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.begin();
		while (it != _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx == clusterIdx) {//���¼���ǰ���Խ��е���
				_RSU.m_RRM_TDM_DRA->pushToAdmitEventIdList(eventId);//��ӵ�RSU����Ľ���������
				it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.erase(it);//����ӵȴ�������ɾ��

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTT->m_RSUId, -1, -1, "TryAccept");

				//��¼TTI��־
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			}
			else {//���¼���ǰ�����Խ��е���
				it++;
				continue; //�����ȴ�����ǰTTI��������
			}
		}
	}
}


void RRM_TDM_DRA::selectBasedOnP123() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		vector<int> curAvaliableEmergencyPatternIdx;//��ǰ���õ�EmergencyPattern���

		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			if (_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx]) {
				curAvaliableEmergencyPatternIdx.push_back(patternIdx);
			}
		}

		for (int eventId : _RSU.m_RRM_TDM_DRA->m_EmergencyAdmitEventIdList) {
			int VeUEId = m_EventVec[eventId].VeUEId;

			//Ϊ��ǰ�û��ڿ��õ�EmergencyPattern�������ѡ��һ����ÿ���û��������ѡ�����EmergencyPattern��
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_TDM_DRA->selectEmergencyRBBasedOnP2(curAvaliableEmergencyPatternIdx);

			if (patternIdx == -1) {//�޶�ӦPattern���͵�pattern��Դ����
				_RSU.m_RRM_TDM_DRA->pushToEmergencyWaitEventIdList(eventId);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "AllBusy");

				//��¼TTI��־
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);

				continue;
			}
			//����Դ���Ϊռ��
			_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx] = false;

			//��������Ϣѹ��m_EmergencyTransimitEventIdList��
			_RSU.m_RRM_TDM_DRA->pushToEmergencyTransmitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, patternIdx), patternIdx);
		}
		_RSU.m_RRM_TDM_DRA->pullFromEmergencyScheduleInfoTable();
		/*  EMERGENCY  */


		//��ʼ�����Emergency���¼�
		int roundATTI = m_TTI%ns_RRM_TDM_DRA::gc_NTTI;//��TTIӳ�䵽[0-gc_DRA_NTTI)�ķ�Χ

		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);

		/*
		* ��ǰTTI���õ�Pattern����
		* �±�����Pattern����ı�ţ�����Emergency��һ���ǿյģ����Ǳ�����������Ų���ƫ��
		* ÿ���ڲ�vector���������Pattern���õ�Pattern���(����Pattern���)
		*/
		vector<vector<int>> curAvaliablePatternIdx(ns_RRM_TDM_DRA::gc_PatternTypeNum);

		for (int patternTypeIdx = 1; patternTypeIdx < ns_RRM_TDM_DRA::gc_PatternTypeNum; patternTypeIdx++) {
			for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[patternTypeIdx][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[patternTypeIdx][1]; patternIdx++) {
				int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
				if (_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		for (int eventId : _RSU.m_RRM_TDM_DRA->m_AdmitEventIdList) {//�����ô��ڽ��������е��¼�

			int VeUEId = m_EventVec[eventId].VeUEId;

			//Ϊ��ǰ�û��ڿ��õĶ�Ӧ���¼����͵�Pattern�������ѡ��һ����ÿ���û��������ѡ�����Pattern��
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_TDM_DRA->selectRBBasedOnP2(curAvaliablePatternIdx, m_EventVec[eventId].message.messageType);
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];

			if (patternIdx == -1) {//���û��������Ϣ����û��patternʣ����
				_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(eventId);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "AllBusy");

				//��¼TTI��־
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
				continue;
			}

			//����Դ���Ϊռ��
			_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = false;

			//��������Ϣѹ��m_TransimitEventIdList��
			_RSU.m_RRM_TDM_DRA->pushToTransmitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, patternIdx), patternIdx);
		}

		//�����ȱ��е�ǰ���Լ���������û�ѹ�봫��������
		_RSU.m_RRM_TDM_DRA->pullFromScheduleInfoTable(m_TTI);
	}
}

void RRM_TDM_DRA::delaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*
		* �ú���Ӧ����selectBasedOnP..֮�����
		* ��ʱ�¼����ڵȴ����������������ߵ��ȱ��У��ǵ�ǰ�أ�
		*/

		//����ȴ�����
		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;
		/*  EMERGENCY  */

		for (int eventId : _RSU.m_RRM_TDM_DRA->m_WaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//����˿����ڽ�Ҫ����Ĵ�������
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++)
			for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		/*  EMERGENCY  */

		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx])
				m_EventVec[p->eventId].sendDelay++;
		}

		//����˿�λ�ڵ��ȱ��У��������ڵ�ǰ�ص��¼�
		int curClusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			if (clusterIdx == curClusterIdx) continue;
			for (RSU::RRM::ScheduleInfo *p : _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx]) {
				if (p == nullptr) continue;
				m_EventVec[p->eventId].queuingDelay++;
			}
		}
	}
}

void RRM_TDM_DRA::conflictListener() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
				for (RSU::RRM::ScheduleInfo* &info : lst) {
					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTT->m_RSUId, -1, patternIdx, "Transimit");

					//���Ƚ��¼�ѹ��ȴ��б�
					_RSU.m_RRM_TDM_DRA->pushToEmergencyWaitEventIdList(info->eventId);

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "Conflict");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, -1, -1);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
				}

				//�ͷ�Pattern��Դ
				_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx] = true;

				lst.clear();
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
				for (RSU::RRM::ScheduleInfo* &info : lst) {
					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Transimit");

					//���Ƚ��¼�ѹ��ȴ��б�
					_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(info->eventId);

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "Conflict");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, -1, -1);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
				}
				//�ͷ�Pattern��Դ
				_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = true;

				lst.clear();
			}
		}
	}
}


void RRM_TDM_DRA::transimitPreparation() {
	//���������һ�θ�����Ϣ
	for (list<int>&lst : m_InterferenceVec) {
		lst.clear();
	}

	//ͳ�Ʊ��εĸ�����Ϣ
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM::ScheduleInfo *info = *lst.begin();

				m_InterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
		/*  EMERGENCY  */

		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RSU::RRM::ScheduleInfo *info = *lst.begin();

				m_InterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
	}
    
	//����ÿ�����ĸ��ų����б�	
	for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
		list<int>& lst = m_InterferenceVec[patternIdx];
		for (int VeUEId : lst) {

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUENum[patternIdx] = (int)lst.size() - 1;//д�������Ŀ
			
			set<int> s(lst.begin(), lst.end());//���ڸ��¸����б��
			s.erase(VeUEId);

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx].assign(s.begin(), s.end());//д����ų���ID
		
			g_FileTemp << "VeUEId: " << VeUEId << " [";
			for (auto c : m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx])
				g_FileTemp << c << ", ";
			g_FileTemp << " ]" << endl;
		}
	}

	//����������˵�Ԫ���������Ӧ����
	long double start = clock();
	m_GTTPoint->calculateInterference(m_InterferenceVec);
	long double end = clock();
	m_GTTTimeConsume += end - start;
}


void RRM_TDM_DRA::transimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_TDM_DRA::transimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}

void RRM_TDM_DRA::transimitStartThread(int fromRSUId, int toRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//����ÿ���̵߳ĸ�ģ����в�ͬ��״̬���޷����������������ģ�����ڱ��μ���
	for (int RSUId = fromRSUId; RSUId <= toRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				//����SINR����ȡ���Ʊ��뷽ʽ
				pair<int, int> &subCarrierIdxRange = getOccupiedSubCarrierRange(m_EventVec[info->eventId].message.messageType, patternIdx);
				g_FileTemp << "Emergency PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				//if (!m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//���Ʊ��뷽ʽ��Ҫ����ʱ
				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//���Ʊ��뷽ʽ��Ҫ����ʱ
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(ns_RRM_TDM_DRA::gc_RBNumPerPatternType[EMERGENCY] * gc_BitNumPerRB)* factor);

				//��¼������Ϣ
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
				info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

				//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit��������������Ϣ״̬
				int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);

				//�ۼ�������
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTT->m_RSUId] += realEquivalentBitNum;

				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTT->m_RSUId, -1, patternIdx, "Transimit");
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];

			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				int patternType = getPatternType(patternIdx);

				//����SINR����ȡ���Ʊ��뷽ʽ
				pair<int, int> &subCarrierIdxRange = getOccupiedSubCarrierRange(m_EventVec[info->eventId].message.messageType, patternIdx);
				g_FileTemp << "NonEmergencyPatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << ((patternType == 0) ? "Emergency" : (patternType == 1 ? "Period" : "Data")) << endl;

				//if ( !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//���Ʊ��뷽ʽ��Ҫ����ʱ
				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//���Ʊ��뷽ʽ��Ҫ����ʱ
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(ns_RRM_TDM_DRA::gc_RBNumPerPatternType[patternType] * gc_BitNumPerRB)* factor);
				
				//��¼������Ϣ
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
				info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

				//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit��������������Ϣ״̬
				int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);

				//�ۼ�������
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTT->m_RSUId] += realEquivalentBitNum;

				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Transimit");
			}
		}
	}
	delete copyWTPoint;//getCopy��ͨ��new�����ģ���������ͷ���Դ
	copyWTPoint = nullptr;
}

void RRM_TDM_DRA::writeScheduleInfo(ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		out << "    RSU[" << _RSU.m_GTT->m_RSUId << "][TTI = " << m_TTI << "]" << endl;
		out << "    {" << endl;
		out << "    EMERGENCY:" << endl;
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    PERIOD:" << endl;
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[PERIOD][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[PERIOD][1]; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    DATA:" << endl;
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[DATA][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[DATA][1]; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    }" << endl;

	}
	out << "}" << endl;
	out << "\n\n" << endl;
}

void RRM_TDM_DRA::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				if (m_EventVec[info->eventId].message.isFinished()) {//�Ѿ�������ϣ�����Դ�ͷ�

					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, -1, patternIdx, "Succeed");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, -1, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete *lst.begin();
					*lst.begin() = nullptr;

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx] = true;
				}
				else {//��δ�������
					_RSU.m_RRM_TDM_DRA->pushToEmergencyScheduleInfoTable(patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			lst.clear();
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx= patternIdx- ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];

			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				if (m_EventVec[info->eventId].message.isFinished()) {//˵���������Ѿ��������

					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Succeed");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete *lst.begin();
					*lst.begin() = nullptr;

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = true;
				}
				else {//��������δ���꣬����ѹ��m_ScheduleInfoTable
					_RSU.m_RRM_TDM_DRA->pushToScheduleInfoTable(clusterIdx, patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			//������󣬽���pattern�ϵ�������գ���ʱҪ��������ǿգ�Ҫ������nullptrָ�룩
			lst.clear();
		}
	}
}


void RRM_TDM_DRA::writeTTILogInfo(ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx) {
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


void RRM_TDM_DRA::writeClusterPerformInfo(ofstream &out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;

	//��ӡVeUE��Ϣ
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUE &_VeUE = m_VeUEAry[VeUEId];
		out << _VeUE.m_RRM_TDM_DRA->toString(2) << endl;
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
		RSU &_RSU = m_RSUAry[RSUId];
		out << _RSU.m_RRM_TDM_DRA->toString(2) << endl;
	}
	out << "    }" << endl;

	out << "}\n\n";
}


int RRM_TDM_DRA::getMaxIndex(const vector<double>&clusterSize) {
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


int RRM_TDM_DRA::getPatternType(int patternIdx) {
	//patternIdxָ�����¼����͵�Pattern�ľ�����ţ���0��ʼ��ţ�����Emergency
	for (int patternType = 0; patternType < ns_RRM_TDM_DRA::gc_PatternTypeNum; patternType++) {
		if (patternIdx >= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[patternType][0] && patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[patternType][1])
			return patternType;
	}
	throw Exp("getPatternType");
}


pair<int, int> RRM_TDM_DRA::getOccupiedSubCarrierRange(MessageType messageType, int patternIdx) {

	pair<int, int> res;

	int offset = 0;
	for (int i = 0; i < messageType; i++) {
		offset += ns_RRM_TDM_DRA::gc_RBNumPerPatternType[i] * ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[i];
		patternIdx -= ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[i];
	}
	res.first = offset + ns_RRM_TDM_DRA::gc_RBNumPerPatternType[messageType] * patternIdx;
	res.second = offset + ns_RRM_TDM_DRA::gc_RBNumPerPatternType[messageType] * (patternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
