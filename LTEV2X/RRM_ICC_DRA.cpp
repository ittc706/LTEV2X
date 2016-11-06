/*
* =====================================================================================
*
*       Filename:  RRM_ICC_DRA.cpp
*
*    Description:  RRM_ICC_DRAģ��
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


#include"RRM_ICC_DRA.h"

using namespace std;

RRM_ICC_DRA::RRM_ICC_DRA(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {

	m_InterferenceVec = vector<list<int>>(ns_RRM_ICC_DRA::gc_TotalPatternNum);
	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = m_Config.RSUNum - 1;//�������һ���߽�
}


void RRM_ICC_DRA::initialize() {
	//��ʼ��VeUE�ĸ�ģ���������
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeRRM_ICC_DRA();
	}

	//��ʼ��RSU�ĸ�ģ���������
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeRRM_ICC_DRA();
	}
}


void RRM_ICC_DRA::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();

		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(ns_RRM_ICC_DRA::gc_TotalPatternNum, false);
	}
}


void RRM_ICC_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//��Դ������Ϣ���:����ÿ��RSU�ڵĽ��������
	informationClean();

	//������������
	updateWaitEventIdList(clusterFlag);

	//��Դѡ��
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


void RRM_ICC_DRA::informationClean() {

}


void RRM_ICC_DRA::updateWaitEventIdList(bool t_ClusterFlag) {
	//���ȣ�����System������¼���������
	processEventList();
	//��Σ������ǰTTI������λ�ø��£���Ҫ������ȱ�
	if (t_ClusterFlag) {
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
		//����RSU����ĵ�������
		processScheduleInfoTableWhenLocationUpdate();

		//����RSU����ĵȴ�����
		processWaitEventIdListWhenLocationUpdate();

		//����System������л�����
		processSwitchListWhenLocationUpdate();
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
	}
}


void RRM_ICC_DRA::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event &event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		
		//�����¼�ѹ��ȴ�����
		_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx,eventId);

		//���¸��¼�����־
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Trigger");

		//��¼TTI��־
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1);
	}
}


void RRM_ICC_DRA::processScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx] == nullptr) continue;

				int eventId = _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
					//ѹ��Switch����
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//���ͷŸõ�����Ϣ����Դ
					delete _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx];
					_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "LocationUpdate");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx);
				}
				else {
					if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//RSU�ڲ������˴��л�������ӵ��ȱ���ȡ����ѹ��ȴ�����
						//ѹ���RSU�ĵȴ�����
						_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId);

						//���ͷŸõ�����Ϣ����Դ
						delete _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx];
						_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

						//�ͷ�Pattern��Դ
						_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

						//���¸��¼�����־
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "LocationUpdate");

						//��¼TTI��־
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx);
					}
				}
			}
		}
	}
}


void RRM_ICC_DRA::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//��ʼ���� m_WaitEventIdList
			list<int>::iterator it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
					//������ӵ�System�����RSU�л�������
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].erase(it);

					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "LocationUpdate");

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
}


void RRM_ICC_DRA::processSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_SwitchEventIdList.begin();
	while (it != m_SwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		
		//ת��ȴ�����
		_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx,eventId);

		//��Switch���н���ɾ��
		it = m_SwitchEventIdList.erase(it);

		//���¸��¼�����־
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "LocationUpdate");

		//��¼TTI��־
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1);
	}
}



void RRM_ICC_DRA::selectBasedOnP123() {

}


void RRM_ICC_DRA::delaystatistics() {

}


void RRM_ICC_DRA::conflictListener() {

}


void RRM_ICC_DRA::transimitPreparation() {

}


void RRM_ICC_DRA::transimitStart() {

}


void RRM_ICC_DRA::transimitStartThread(int t_FromRSUId, int t_ToRSUId) {

}


void RRM_ICC_DRA::transimitEnd() {

}


void RRM_ICC_DRA::writeScheduleInfo(ofstream& t_File) {

}


void RRM_ICC_DRA::writeTTILogInfo(std::ofstream& t_File, int TTI, EventLogType t_EventLogType, int t_EventId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx) {

}


void RRM_ICC_DRA::writeClusterPerformInfo(ofstream& t_File) {

}


pair<int, int> RRM_ICC_DRA::getOccupiedSubCarrierRange(MessageType t_MessageType, int t_PatternIdx) {
	return pair<int, int>(-1, -1);
}
