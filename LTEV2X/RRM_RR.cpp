/*
* =====================================================================================
*
*       Filename:  RRM_RR.cpp
*
*    Description:  RRģ��
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


#include<iomanip>
#include<sstream>
#include<iostream>
#include<set>
#include"RRM_RR.h"

using namespace std;


RRM_RR::RRM_RR(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {
	
	m_InterferenceVec = vector<list<int>>(ns_RRM_RR::gc_TotalPatternNum);

	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = m_Config.RSUNum - 1;//�������һ���߽�
}


void RRM_RR::initialize() {
	//��ʼ��VeUE�ĸ�ģ���������
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeRRM_RR();
	}

	//��ʼ��RSU�ĸ�ģ���������
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeRRM_RR();
	}
}


void RRM_RR::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();
		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(ns_RRM_RR::gc_TotalPatternNum, false);
	}
}


void RRM_RR::schedule() {
	bool isLocationUpdate = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//д�����λ����Ϣ
	writeClusterPerformInfo(isLocationUpdate, g_FileClasterPerformInfo);

	//����ǰ������
	informationClean();

	//������������
	updateAdmitEventIdList(isLocationUpdate);

	//��ʼ���ε���
	roundRobin();

    //ͳ��ʱ����Ϣ
	delaystatistics();

	//ͳ�Ƹ�����Ϣ
	transimitPreparation();

	//ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	transimitStart();

	//д������־
	writeScheduleInfo(g_FileScheduleInfo);

	//�������
	transimitEnd();
}



void RRM_RR::informationClean() {
	
}


void RRM_RR::updateAdmitEventIdList(bool t_ClusterFlag) {
	//���ȣ�����System������¼���������
	processEventList();

	//��Σ������ǰTTI�����˷ִأ���Ҫ������ȱ�
	if (t_ClusterFlag) {
		//����RSU����ĵȴ�����
		processWaitEventIdListWhenLocationUpdate();

		//����System������л�����
		processSwitchListWhenLocationUpdate();
	}
}

void RRM_RR::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		RSU &_RSU = m_RSUAry[RSUId];
		//���¼�ѹ��ȴ�����
		bool isEmergency = event.message.messageType == EMERGENCY;
		_RSU.m_RRM_RR->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//������־
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, -1, -1, -1, RSUId, clusterIdx, -1, "Trigger");
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, -1, -1, -1, RSUId, clusterIdx, -1, "Trigger");
	}
}


void RRM_RR::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {

			list<int>::iterator it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				bool isEmergency = m_EventVec[eventId].message.messageType == EMERGENCY;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��

					//������ӵ�System�����RSU�л�������
					_RSU.m_RRM_RR->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);

					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//������־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				else if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//��Ȼ���ڵ�ǰRSU��Χ�ڣ�����λ�ڲ�ͬ�Ĵ�
				   //����ת�Ƶ���ǰRSU����������
					_RSU.m_RRM_RR->pushToWaitEventIdList(isEmergency, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);

					//������־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
				}
				else {
					it++;
					continue; //�������ڵ�ǰRSU��ǰ�صĵȴ�����
				}
			}
		}
	}
}


void RRM_RR::processSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_SwitchEventIdList.begin();
	while (it != m_SwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];

		bool isEmergency = m_EventVec[eventId].message.messageType == EMERGENCY;
		_RSU.m_RRM_RR->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//��Switch������ɾ��
		it = m_SwitchEventIdList.erase(it);

		//������־
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, -1, -1, -1, RSUId, clusterIdx, -1, "LocationUpdate");
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, -1, -1, -1, RSUId, clusterIdx, -1, "LocationUpdate");

	}
}


void RRM_RR::roundRobin() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			int patternIdx = 0;
			list<int>::iterator it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].begin();
			while (it!= _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].end() && patternIdx < ns_RRM_RR::gc_TotalPatternNum) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				_RSU.m_RRM_RR->pushToTransimitScheduleInfoTable(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx));
				it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);
				patternIdx++;
			}
		}
	}
}


void RRM_RR::delaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//����ȴ�����
			for (int eventId : _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx])
				m_EventVec[eventId].queuingDelay++;

			//����˿����ڽ�Ҫ����ĵ��ȱ�
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_RR->m_TransimitScheduleInfoTable[clusterIdx][patternIdx] == nullptr)continue;
				m_EventVec[_RSU.m_RRM_RR->m_TransimitScheduleInfoTable[clusterIdx][patternIdx]->eventId].sendDelay++;
			}
		}
	}
}


void RRM_RR::transimitPreparation() {
	for (list<int>&lst : m_InterferenceVec) {
		lst.clear();
	}

	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
				RSU::RRM::ScheduleInfo *&info = _RSU.m_RRM_RR->m_TransimitScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				m_InterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
	}


	//����ÿ�����ĸ��ų����б�	
	for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
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
	//m_GTTPoint->calculateInterference(m_InterferenceVec);
	long double end = clock();
	m_GTTTimeConsume += end - start;
}


void RRM_RR::transimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_RR::transimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}


void RRM_RR::transimitStartThread(int t_FromRSUId, int t_ToRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//����ÿ���̵߳ĸ�ģ����в�ͬ��״̬���޷����������������ģ�����ڱ��μ���
	for (int RSUId = t_FromRSUId; RSUId <= t_ToRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
				RSU::RRM::ScheduleInfo *&info = _RSU.m_RRM_RR->m_TransimitScheduleInfoTable[clusterIdx][patternIdx];

				if (info == nullptr) continue;
				int VeUEId = info->VeUEId;

				//����SINR����ȡ���Ʊ��뷽ʽ
				pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(patternIdx);
				g_FileTemp << "PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//���Ʊ��뷽ʽ��Ҫ����ʱ
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(ns_RRM_RR::gc_RBNumPerPattern * gc_BitNumPerRB)* factor);

				//��¼������Ϣ
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
}


void RRM_RR::writeScheduleInfo(ofstream& t_File) {
	t_File << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	t_File << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {

		RSU &_RSU = m_RSUAry[RSUId];
		t_File << "    RSU[" << _RSU.m_GTT->m_RSUId << "] :" << endl;
		t_File << "    {" << endl;
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			t_File << "        Cluster[" << clusterIdx << "] :" << endl;
			t_File << "        {" << endl;
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
				t_File << "            Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
				RSU::RRM::ScheduleInfo* &info = _RSU.m_RRM_RR->m_TransimitScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				t_File << info->toScheduleString(3) << endl;
			}
			t_File << "        }" << endl;
		}
		t_File << "    }" << endl;
	}
	t_File << "}" << endl;
	t_File << "\n\n" << endl;
}


void RRM_RR::writeTTILogInfo(ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description) {
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


void RRM_RR::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {

			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {

				RSU::RRM::ScheduleInfo* &info = _RSU.m_RRM_RR->m_TransimitScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;

				if (m_EventVec[info->eventId].message.isFinished()) {//˵���Ѿ��������

					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//������־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
				}
				else {//û�д�����ϣ�ת��Wait�����ȴ���һ�ε���
					bool isEmergency = m_EventVec[info->eventId].message.messageType == EMERGENCY;
					_RSU.m_RRM_RR->pushToWaitEventIdList(isEmergency, clusterIdx, info->eventId);

					//������־
					m_EventVec[info->eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "NextTurn");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "NextTurn");

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
				}
			}
		}
	}
}



void RRM_RR::writeClusterPerformInfo(bool isLocationUpdate, ofstream& t_File) {
	if (!isLocationUpdate) return;
	t_File << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	t_File << "{" << endl;

	//��ӡVeUE��Ϣ
	t_File << "    VUE Info: " << endl;
	t_File << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUE &_VeUE = m_VeUEAry[VeUEId];
		t_File << _VeUE.m_RRM_RR->toString(2) << endl;
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
		t_File << _RSU.m_RRM_RR->toString(2) << endl;
	}
	t_File << "    }" << endl;

	t_File << "}\n\n";
}


pair<int, int> RRM_RR::getOccupiedSubCarrierRange(int t_PatternIdx) {
	pair<int, int> res;

	res.first = ns_RRM_RR::gc_RBNumPerPattern * t_PatternIdx;
	res.second = ns_RRM_RR::gc_RBNumPerPattern * (t_PatternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
