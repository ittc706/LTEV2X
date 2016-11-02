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


RRM_RR::RRM_RR(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput, GTAT_Basic* systemGTATPoint, WT_Basic* systemWTPoint, int threadNum) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList, systemTTIRSUThroughput), m_GTATPoint(systemGTATPoint), m_WTPoint(systemWTPoint), m_ThreadNum(threadNum) {
	
	m_RRInterferenceVec = std::vector<std::list<int>>(gc_RRTotalPatternNum);

	m_ThreadsRSUIdRange = vector<pair<int, int>>(threadNum);

	int num = m_Config.RSUNum / threadNum;
	for (int threadIdx = 0; threadIdx < threadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[threadNum - 1].second = m_Config.RSUNum - 1;//�������һ���߽�
}


void RRM_RR::initialize() {
	//��ʼ��VeUE�ĸ�ģ���������
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeRR();
	}

	//��ʼ��RSU�ĸ�ģ���������
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeRR();
	}
}


void RRM_RR::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();
		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(gc_DRATotalPatternNum, false);
	}
}


void RRM_RR::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//����ǰ������
	RRInformationClean();

	//������������
	RRUpdateAdmitEventIdList(clusterFlag);

	//��ʼ���ε���
	RRRoundRobin();

    //ͳ��ʱ����Ϣ
	RRDelaystatistics();

	//ͳ�Ƹ�����Ϣ
	RRTransimitPreparation();

	//ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	RRTransimitStart();

	//д������־
	RRWriteScheduleInfo(g_FileRRScheduleInfo);

	//�������
	RRTransimitEnd();
}



void RRM_RR::RRInformationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++)
			_RSU.m_RRM_RR->m_RRAdmitEventIdList[clusterIdx].clear();
	}
}


void RRM_RR::RRUpdateAdmitEventIdList(bool clusterFlag) {
	//���ȣ�����System������¼���������
	RRProcessEventList();

	//��Σ������ǰTTI�����˷ִأ���Ҫ������ȱ�
	if (clusterFlag) {
		if (m_RRSwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");

		//����RSU����ĵȴ�����
		RRProcessWaitEventIdListWhenLocationUpdate();

		//����System������л�����
		RRProcessSwitchListWhenLocationUpdate();

		if (m_RRSwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");
	}

	//Ȼ�󣬴���RSU����ĵȴ�����
	RRProcessWaitEventIdList();
}

void RRM_RR::RRProcessEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTAT->m_RSUId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx;
		RSU &_RSU = m_RSUAry[RSUId];
		//���¼�ѹ��ȴ�����
		_RSU.m_RRM_RR->RRPushToWaitEventIdList(clusterIdx,eventId, m_EventVec[eventId].message.messageType);

		//���¸��¼�����־
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, RSUId, -1, -1, "Trigger");

		//��¼TTI��־
		RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, RSUId, -1);
	}
}


void RRM_RR::RRProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {

			list<int>::iterator it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//������ӵ�System�����RSU�л�������
					_RSU.m_RRM_RR->RRPushToSwitchEventIdList(eventId, m_RRSwitchEventIdList);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

					//��¼TTI��־
					RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1);
				}else if(m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx!= clusterIdx){//��Ȼ���ڵ�ǰRSU��Χ�ڣ�����λ�ڲ�ͬ�Ĵ�
					//����ת�Ƶ���ǰRSU����������
					_RSU.m_RRM_RR->RRPushToWaitEventIdList(m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx, eventId, messageType);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

					//��¼TTI��־
					RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1);
				}else {
					it++;
					continue; //�������ڵ�ǰRSU��ǰ�صĵȴ�����
				}
			}
		}
	}
}


void RRM_RR::RRProcessSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_RRSwitchEventIdList.begin();
	while (it != m_RRSwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx;
		int RSUId = m_VeUEAry[VeUEId].m_GTAT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];

		_RSU.m_RRM_RR->RRPushToWaitEventIdList(clusterIdx, eventId, m_EventVec[eventId].message.messageType);

		//��Switch������ɾ��
		it = m_RRSwitchEventIdList.erase(it);

		//���¸��¼�����־
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, RSUId, -1, -1, "LocationUpdate");

		//��¼TTI��־
		RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, RSUId, -1);

	}
}


void RRM_RR::RRProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			int count = 0;
			list<int>::iterator it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].end() && count++ < gc_RRTotalPatternNum) {
				int eventId = *it;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				//ѹ���������
				_RSU.m_RRM_RR->RRPushToAdmitEventIdList(clusterIdx, eventId);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, RSUId, -1, -1, "Accept");

				//��¼TTI��־
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, RSUId, -1);

				it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].erase(it);
			}
			if (_RSU.m_RRM_RR->m_RRAdmitEventIdList[clusterIdx].size() > gc_RRTotalPatternNum)
				throw Exp("RRProcessWaitEventIdList()");
		}
	}
}


void RRM_RR::RRRoundRobin() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < _RSU.m_RRM_RR->m_RRAdmitEventIdList[clusterIdx].size(); patternIdx++) {
				int eventId = _RSU.m_RRM_RR->m_RRAdmitEventIdList[clusterIdx][patternIdx];
				int VeUEId = m_EventVec[eventId].VeUEId;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				_RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx] = new RSU::RRM_RR::RRScheduleInfo(eventId, messageType, VeUEId, _RSU.m_GTAT->m_RSUId, patternIdx);
				m_NewCount++;
			}
		}
	}
}


void RRM_RR::RRDelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			//����ȴ�����
			for (int eventId : _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx])
				m_EventVec[eventId].queuingDelay++;

			//����˿����ڽ�Ҫ����ĵ��ȱ�
			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx] == nullptr)continue;
				m_EventVec[_RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx]->eventId].sendDelay++;
			}
		}
	}
}


void RRM_RR::RRTransimitPreparation() {
	for (list<int>&lst : m_RRInterferenceVec) {
		lst.clear();
	}

	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
				RSU::RRM_RR::RRScheduleInfo *&info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				m_RRInterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
	}


	//����ÿ�����ĸ��ų����б�	
	for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
		list<int>& lst = m_RRInterferenceVec[patternIdx];

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
	m_GTATPoint->calculateInterference(m_RRInterferenceVec);
	long double end = clock();
	m_GTATTimeConsume += end - start;
}


void RRM_RR::RRTransimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_RR::RRTransimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}


void RRM_RR::RRTransimitStartThread(int fromRSUId, int toRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//����ÿ���̵߳ĸ�ģ����в�ͬ��״̬���޷����������������ģ�����ڱ��μ���
	for (int RSUId = fromRSUId; RSUId <= toRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
				RSU::RRM_RR::RRScheduleInfo *&info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx];

				if (info == nullptr) continue;
				int VeUEId = info->VeUEId;

				//����SINR����ȡ���Ʊ��뷽ʽ
				pair<int, int> &subCarrierIdxRange = RRGetOccupiedSubCarrierRange(patternIdx);
				g_FileTemp << "PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//���Ʊ��뷽ʽ��Ҫ����ʱ
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(gc_RRNumRBPerPattern * gc_BitNumPerRB)* factor);

				//��¼������Ϣ
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
				info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

				//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit��������������Ϣ״̬
				int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);

				//�ۼ�������
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTAT->m_RSUId] += realEquivalentBitNum;

				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Transimit");
			}
		}
	}
}


void RRM_RR::RRWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {

		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			out << "    RSU[" << _RSU.m_GTAT->m_RSUId << "] :" << endl;
			out << "    {" << endl;
			out << "        Cluster[" << clusterIdx << "] :" << endl;
			out << "        {" << endl;
			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
				out << "            Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
				RSU::RRM_RR::RRScheduleInfo* &info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				out << info->toScheduleString(3) << endl;
			}
			out << "        }" << endl;
			out << "    }" << endl;
		}
		
	}
	out << "}" << endl;
	out << "\n\n" << endl;
}


void RRM_RR::RRWriteTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int patternIdx) {
	stringstream ss;
	switch (type) {
	case SUCCEED:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]    PatternIdx[" << patternIdx << "] }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[0]Succeed";
		out << "    " << ss.str() << endl;
		break;
	case EVENT_TO_WAIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[1]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_ADMIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[2]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << patternIdx << "] ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[3]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_SWITCH:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[4]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[5]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SWITCH_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[5]Switch";
		out << "    " << ss.str() << endl;
		break;
	}
}


void RRM_RR::RRTransimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {

			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {

				RSU::RRM_RR::RRScheduleInfo* &info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;

				if (m_EventVec[info->eventId].message.isFinished()) {//˵���Ѿ��������
					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Succeed");

					//��¼TTI��־
					RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTAT->m_RSUId, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
					m_DeleteCount++;
				}
				else {//û�д�����ϣ�ת��Wait�����ȴ���һ�ε���
					_RSU.m_RRM_RR->RRPushToWaitEventIdList(clusterIdx, info->eventId, m_EventVec[info->eventId].message.messageType);

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "WaitNextTurn");

					//��¼TTI��־
					RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, info->eventId, _RSU.m_GTAT->m_RSUId, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
					m_DeleteCount++;
				}
			}
		}
	}
}


std::pair<int, int> RRM_RR::RRGetOccupiedSubCarrierRange(int patternIdx) {
	pair<int, int> res;

	res.first = gc_RRNumRBPerPattern * patternIdx;
	res.second = gc_RRNumRBPerPattern * (patternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
