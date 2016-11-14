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
#include"Function.h"

using namespace std;

RRM_TDM_DRA::RRM_TDM_DRA(int &t_TTI, SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {

	m_InterferenceVec = vector<vector<list<int>>>(m_Config.VeUENum, vector<list<int>>(ns_RRM_TDM_DRA::gc_TotalPatternNum));
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

		m_VeUEAry[VeUEId].m_RRM->m_PreSINR.assign(ns_RRM_TDM_DRA::gc_TotalPatternNum, (numeric_limits<double>::min)());
	}
}


void RRM_TDM_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//���ݴش�С����ʱ����Դ�Ļ���
	//groupSizeBasedTDM(clusterFlag);
	uniformTDM(clusterFlag);

	//���µȴ�����
	updateWaitEventIdList(clusterFlag);

	//��Դѡ��
	selectRBBasedOnP123();

	//ͳ��ʱ����Ϣ
	delaystatistics();

	//֡����ͻ
	conflictListener();

	//����������˵�Ԫ���������Ӧ����
	transimitPreparation();

	//ģ�⴫�俪ʼ�����µ�����Ϣ
	transimitStart();

	//д�������Ϣ
	writeScheduleInfo(g_FileScheduleInfo);

	//ģ�⴫�������ͳ��������
	transimitEnd();
}


void RRM_TDM_DRA::groupSizeBasedTDM(bool t_ClusterFlag) {
	if (!t_ClusterFlag)return;
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


void RRM_TDM_DRA::uniformTDM(bool t_ClusterFlag) {
	if (!t_ClusterFlag)return;
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


void RRM_TDM_DRA::updateWaitEventIdList(bool t_ClusterFlag) {
	//���ȣ�����System������¼���������
	processEventList();
	//��Σ������ǰTTI������λ�ø��£���Ҫ������ȱ�
	if (t_ClusterFlag) {
		//����RSU����ĵ�������
		processScheduleInfoTableWhenLocationUpdate();

		//����RSU����ĵȴ�����
		processWaitEventIdListWhenLocationUpdate();

		//����System������л�����
		processSwitchListWhenLocationUpdate();
	}
}


void RRM_TDM_DRA::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event &event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;

		//�����¼�ѹ������¼��ȴ�����
		bool isEmergency = event.message.getMessageType() == EMERGENCY;
		_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//������־
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, -1, -1, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Trigger");
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, -1, -1, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Trigger");
	}
}


void RRM_TDM_DRA::processScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		//��ʼ���� m_ScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx] == nullptr) continue;

				int eventId = _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
					//ѹ��Switch����
					_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//���ͷŸõ�����Ϣ����Դ
					Delete::safeDelete(_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx]);

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					//������־
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "LocationUpdate");
				}
				else {
					if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//RSU�ڲ������˴��л�������ӵ��ȱ���ȡ����ѹ��ȴ�����
						//ѹ���RSU�ĵȴ�����
						bool isEmergency = m_EventVec[eventId].message.getMessageType() == EMERGENCY;
						_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(isEmergency, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId);

						//���ͷŸõ�����Ϣ����Դ
						Delete::safeDelete(_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx]);

						//�ͷ�Pattern��Դ
						_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

						//������־
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
					}
				}
			}
		}
	}
}


void RRM_TDM_DRA::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		//��ʼ���� m_WaitEventIdList

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			/*  EMERGENCY  */
			list<int>::iterator it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].first.begin();
			while (it != _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].first.end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
																			 
					//������ӵ�System�����RSU�л�������
					_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].first.erase(it);

					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//������־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				else if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//��Ȼ���ڵ�ǰRSU��Χ�ڣ���λ�ڲ�ͬ�Ĵ�

				   //������ӵ����ڴصĵȴ�����
					_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(true, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId);

					//����ӵȴ������еĵ�ǰ��ɾ��
					it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].first.erase(it);

					//������־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
				}
				else {
					it++;
					continue; //�������ڵ�ǰRSU�ĵȴ�����
				}
			}
			/*  EMERGENCY  */

			it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].second.begin();
			while (it != _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].second.end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��

					//������ӵ�System�����RSU�л�������
					_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].second.erase(it);

					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//������־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				else if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//��Ȼ���ڵ�ǰRSU��Χ�ڣ���λ�ڲ�ͬ�Ĵ�

					//������ӵ����ڴصĵȴ�����
					_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(false, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId);

					//����ӵȴ������еĵ�ǰ��ɾ��
					it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].second.erase(it);

					//������־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
				}
				else {
					it++;
					continue; //�������ڵ�ǰRSU�ĵȴ�����
				}
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
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;

		//ת��ȴ�����
		bool isEmergency = m_EventVec[eventId].message.getMessageType() == EMERGENCY;
		_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//��Switch���н���ɾ��
		it = m_SwitchEventIdList.erase(it);

		//������־
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, -1, -1, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "LocationUpdate");
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, -1, -1, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "LocationUpdate");
	}
}



void RRM_TDM_DRA::selectRBBasedOnP123() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			vector<int> curAvaliableEmergencyPatternIdx;//��ǰ�ؿ��õ�EmergencyPattern���

			for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
				if (_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliableEmergencyPatternIdx.push_back(patternIdx);
				}
			}

            list<int>::iterator it= _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].first.begin();
			while (it != _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].first.end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;

				//Ϊ��ǰ�û��ڿ��õ�EmergencyPattern�������ѡ��һ����ÿ���û��������ѡ�����EmergencyPattern��
				int patternIdx = m_VeUEAry[VeUEId].m_RRM_TDM_DRA->selectRBBasedOnP2(curAvaliableEmergencyPatternIdx);

				if (patternIdx == -1) {//�޶�ӦPattern���͵�pattern��Դ����

					//������־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "AllBusy");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "AllBusy");

					it++;
					continue;
				}
				//����Դ���Ϊռ��
				_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

				//��������Ϣѹ��m_EmergencyTransimitEventIdList��
				_RSU.m_RRM_TDM_DRA->pushToTransimitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, -1, patternIdx));

				//�����¼�Id��Wait������ɾ��
				it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].first.erase(it);
			}
		}
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
				if (_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		list<int>::iterator it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].second.begin();
		while (it != _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].second.end()) {//�����ô��ڽ��������е��¼�
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;

			//Ϊ��ǰ�û��ڿ��õĶ�Ӧ���¼����͵�Pattern�������ѡ��һ����ÿ���û��������ѡ�����Pattern��
			MessageType messageType = m_EventVec[eventId].message.getMessageType();
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_TDM_DRA->selectRBBasedOnP2(curAvaliablePatternIdx[messageType]);

			if (patternIdx == -1) {//���û��������Ϣ����û��patternʣ����

				//������־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "AllBusy");
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "AllBusy");
				
				it++;
				continue;
			}

			//����Դ���Ϊռ��
			_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

			//��������Ϣѹ��m_TransimitEventIdList��
			_RSU.m_RRM_TDM_DRA->pushToTransimitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx));

			//�����¼�Id��Wait������ɾ��
			it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].second.erase(it);
		}

		//�����ȱ��е�ǰ���Լ���������û�ѹ�봫��������(���������ͷǽ���)
		_RSU.m_RRM_TDM_DRA->pullFromScheduleInfoTable(m_TTI);
	}
}

void RRM_TDM_DRA::delaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		
		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//����ȴ�����
			for (int eventId : _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].first)
				m_EventVec[eventId].queuingDelay++;
			//����˿����ڽ�Ҫ����Ĵ�������
			for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++)
				for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
					m_EventVec[p->eventId].sendDelay++;
		}
		/*  EMERGENCY  */


		//����ȴ�����
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int eventId : _RSU.m_RRM_TDM_DRA->m_WaitEventIdList[clusterIdx].second)
				m_EventVec[eventId].queuingDelay++;
		}

		//����˿����ڽ�Ҫ����Ĵ�������
		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		}

		//����˿�λ�ڵ��ȱ��У��������ڵ�ǰ�ص��¼�
		for (int otherClusterIdx = 0; otherClusterIdx < _RSU.m_GTT->m_ClusterNum; otherClusterIdx++) {
			if (otherClusterIdx == clusterIdx) continue;
			for (RSU::RRM::ScheduleInfo *p : _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[otherClusterIdx]) {
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
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
					for (RSU::RRM::ScheduleInfo* &info : lst) {
						//���Ƚ��¼�ѹ��ȴ��б�
						_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(true, clusterIdx, info->eventId);

						//������־
						m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Conflict");
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Conflict");

						//�ͷŵ�����Ϣ������ڴ���Դ
						Delete::safeDelete(info);
					}

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					lst.clear();
				}
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
			if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
				for (RSU::RRM::ScheduleInfo* &info : lst) {
					//���Ƚ��¼�ѹ��ȴ��б�
					_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(false, clusterIdx, info->eventId);

					//������־
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Conflict");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Conflict");

					//�ͷŵ�����Ϣ������ڴ���Դ
					Delete::safeDelete(info);
				}
				//�ͷ�Pattern��Դ
				_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

				lst.clear();
			}
		}
	}
}


void RRM_TDM_DRA::transimitPreparation() {
	//���������һ�θ�����Ϣ
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++)
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++)
			m_InterferenceVec[VeUEId][patternIdx].clear();

	//ͳ�Ʊ��εĸ�����Ϣ
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
				list<RSU::RRM::ScheduleInfo*> &curList = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (curList.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
					RSU::RRM::ScheduleInfo *curInfo = *curList.begin();
					int curVeUEId = curInfo->VeUEId;
					for (int otherClusterIdx = 0; otherClusterIdx < _RSU.m_GTT->m_ClusterNum; otherClusterIdx++) {
						if (otherClusterIdx == clusterIdx)continue;
						list<RSU::RRM::ScheduleInfo*> &otherList = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[otherClusterIdx][patternIdx];
						if (otherList.size() == 1) {//�������и�pattern���г����ڴ��䣬��ô���ó�����Ϊ���ų���
							RSU::RRM::ScheduleInfo *otherInfo = *otherList.begin();
							int otherVeUEId = otherInfo->VeUEId;
							m_InterferenceVec[curVeUEId][patternIdx].push_back(otherVeUEId);
						}
					}
				}
			}
		}
	}

	//����ÿ�����ĸ��ų����б�	
	for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
		for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
			list<int>& interList = m_InterferenceVec[VeUEId][patternIdx];

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUENum[patternIdx] = (int)interList.size();//д�������Ŀ

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx].assign(interList.begin(), interList.end());//д����ų���ID

			if (m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUENum[patternIdx]>0) {
				g_FileTemp << "VeUEId: " << VeUEId << " [";
				for (auto c : m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx])
					g_FileTemp << c << ", ";
				g_FileTemp << " ]" << endl;
			}
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

void RRM_TDM_DRA::transimitStartThread(int t_FromRSUId, int t_ToRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//����ÿ���̵߳ĸ�ģ����в�ͬ��״̬���޷����������������ģ�����ڱ��μ���
	for (int RSUId = t_FromRSUId; RSUId <= t_ToRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {
					RSU::RRM::ScheduleInfo *info = *lst.begin();
					int VeUEId = info->VeUEId;

					//����SINR����ȡ���Ʊ��뷽ʽ
					pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(m_EventVec[info->eventId].message.getMessageType(), patternIdx);
					g_FileTemp << "Emergency PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

					double factor = m_VeUEAry[VeUEId].m_RRM->m_ModulationType * m_VeUEAry[VeUEId].m_RRM->m_CodeRate;

					//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
					int maxEquivalentBitNum = (int)((double)(ns_RRM_TDM_DRA::gc_RBNumPerPatternType[EMERGENCY] * gc_BitNumPerRB)* factor);

					//����SINR
					double curSINR = 0;
					if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->isAlreadyCalculateSINR(patternIdx)) {//���Ʊ��뷽ʽ��Ҫ����ʱ
						curSINR = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
						m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
						m_VeUEAry[VeUEId].m_RRM->m_PreSINR[patternIdx] = curSINR;
					}
					else
						curSINR = m_VeUEAry[VeUEId].m_RRM->m_PreSINR[patternIdx];

					//��¼������Ϣ
					if (curSINR < gc_CriticalPoint) m_EventVec[info->eventId].message.packetLoss();//��¼����
					info->transimitBitNum = maxEquivalentBitNum;
					info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
					info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

					//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit��������������Ϣ״̬
					int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);

					//�ۼ�������
					m_TTIRSUThroughput[m_TTI][_RSU.m_GTT->m_RSUId] += realEquivalentBitNum;

					//������־
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMITTING, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMITTING, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
				}
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {

			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				int patternType = getPatternType(patternIdx);

				//����SINR����ȡ���Ʊ��뷽ʽ
				pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(m_EventVec[info->eventId].message.getMessageType(), patternIdx);
				g_FileTemp << "NonEmergencyPatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << ((patternType == 0) ? "Emergency" : (patternType == 1 ? "Period" : "Data")) << endl;
	
				double factor = m_VeUEAry[VeUEId].m_RRM->m_ModulationType * m_VeUEAry[VeUEId].m_RRM->m_CodeRate;

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(ns_RRM_TDM_DRA::gc_RBNumPerPatternType[patternType] * gc_BitNumPerRB)* factor);
				
				//����SINR
				double curSINR = 0;
				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->isAlreadyCalculateSINR(patternIdx)) {//���Ʊ��뷽ʽ��Ҫ����ʱ
					curSINR = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_PreSINR[patternIdx] = curSINR;
				}
				else
					curSINR = m_VeUEAry[VeUEId].m_RRM->m_PreSINR[patternIdx];

				//��¼������Ϣ
				if (curSINR < gc_CriticalPoint) m_EventVec[info->eventId].message.packetLoss();//��¼����
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
				info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

				//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit��������������Ϣ״̬
				int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);

				//�ۼ�������
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTT->m_RSUId] += realEquivalentBitNum;

				//������־
				m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMITTING, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMITTING, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
			}
		}
	}
	Delete::safeDelete(copyWTPoint);//getCopy��ͨ��new�����ģ���������ͷ���Դ
}

void RRM_TDM_DRA::writeScheduleInfo(ofstream& t_File) {
	t_File << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	t_File << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		int NonEmergencyClusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		
		t_File << "    RSU[" << _RSU.m_GTT->m_RSUId << "][TTI = " << m_TTI << "]" << endl;
		t_File << "    {" << endl;
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			t_File << "        Cluster[" << clusterIdx << "]" << endl;
			t_File << "        {" << endl;

			t_File << "            EMERGENCY:" << endl;
			for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[EMERGENCY][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[EMERGENCY][1]; patternIdx++) {
				bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx];
				t_File << "                Pattern[" << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
				if (!isAvaliable) {
					RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					t_File << info->toScheduleString(5) << endl;
				}
			}
			if (clusterIdx != NonEmergencyClusterIdx)continue;
			t_File << "            PERIOD:" << endl;
			for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[PERIOD][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[PERIOD][1]; patternIdx++) {
				bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx];
				t_File << "                Pattern[" << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
				if (!isAvaliable) {
					RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					t_File << info->toScheduleString(5) << endl;
				}
			}
			t_File << "            DATA:" << endl;
			for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[DATA][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[DATA][1]; patternIdx++) {
				bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx];
				t_File << "                Pattern[" << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
				if (!isAvaliable) {
					RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					t_File << info->toScheduleString(5) << endl;
				}
			}
			t_File << "        }" << endl;
		}
		t_File << "{" << endl;
	}
	t_File << "}" << endl;
	t_File << "\n\n" << endl;
}

void RRM_TDM_DRA::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {
					RSU::RRM::ScheduleInfo* &info = *lst.begin();
					if (m_EventVec[info->eventId].message.isFinished()) {//�Ѿ�������ϣ�����Դ�ͷ�

						//���ô���ɹ����
						m_EventVec[info->eventId].isSuccessded = true;

						//������־
						m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

						//�ͷŵ�����Ϣ������ڴ���Դ
						Delete::safeDelete(info);

						//�ͷ�Pattern��Դ
						_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;
					}
					else {//��δ�������
						_RSU.m_RRM_TDM_DRA->pushToScheduleInfoTable(info);
						info = nullptr;
					}
				}
				lst.clear();
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {

			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RSU::RRM::ScheduleInfo* &info = *lst.begin();
				if (m_EventVec[info->eventId].message.isFinished()) {//˵���������Ѿ��������

					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//������־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

					//�ͷŵ�����Ϣ������ڴ���Դ
					Delete::safeDelete(info);

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;
				}
				else {//��������δ���꣬����ѹ��m_ScheduleInfoTable
					_RSU.m_RRM_TDM_DRA->pushToScheduleInfoTable(info);
					info = nullptr;
				}
			}
			//������󣬽���pattern�ϵ�������գ���ʱҪ��������ǿգ�Ҫ������nullptrָ�룩
			lst.clear();
		}
	}
}


void RRM_TDM_DRA::writeTTILogInfo(ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description) {
	stringstream ss;
	switch (t_EventLogType) {
	case TRANSIMITTING:
		ss << " - Transimiting  At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SUCCEED:
		ss << " - Transimit Succeed At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case EVENT_TO_WAIT:
		ss << " - From: EventList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: SwitchList";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WAIT_TO_SWITCH:
		ss << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: SwitchList";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WAIT_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SWITCH_TO_WAIT:
		ss << " - From: SwitchList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case TRANSIMIT_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s TransimitScheduleInfoList[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	}
}


void RRM_TDM_DRA::writeClusterPerformInfo(ofstream &t_File) {
	t_File << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	t_File << "{" << endl;

	//��ӡVeUE��Ϣ
	t_File << "    VUE Info: " << endl;
	t_File << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUE &_VeUE = m_VeUEAry[VeUEId];
		t_File << _VeUE.m_RRM_TDM_DRA->toString(2) << endl;
	}
	t_File << "    }\n" << endl;

	////��ӡ��վ��Ϣ
	//out << "    eNB Info: " << endl;
	//out << "    {" << endl;
	//for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
	//	ceNB &_eNB = m_eNBAry[eNBId];
	//	out << _eNB.toString(2) << endl;
	//}
	//out << "    }\n" << endl;

	//��ӡRSU��Ϣ
	t_File << "    RSU Info: " << endl;
	t_File << "    {" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		t_File << _RSU.m_RRM_TDM_DRA->toString(2) << endl;
	}
	t_File << "    }" << endl;

	t_File << "}\n\n";
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
