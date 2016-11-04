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
	
	m_InterferenceVec = vector<list<int>>(ns_RRM_RR::gc_RRTotalPatternNum);

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
		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(ns_RRM_TDM_DRA::gc_TotalPatternNum, false);
	}
}


void RRM_RR::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//����ǰ������
	informationClean();

	//������������
	updateAdmitEventIdList(clusterFlag);

	//��ʼ���ε���
	roundRobin();

    //ͳ��ʱ����Ϣ
	delaystatistics();

	//ͳ�Ƹ�����Ϣ
	transimitPreparation();

	//ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	transimitStart();

	//д������־
	writeScheduleInfo(g_FileRRScheduleInfo);

	//�������
	transimitEnd();
}



void RRM_RR::informationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++)
			_RSU.m_RRM_RR->m_AdmitEventIdList[clusterIdx].clear();
	}
}


void RRM_RR::updateAdmitEventIdList(bool clusterFlag) {
	//���ȣ�����System������¼���������
	processEventList();

	//��Σ������ǰTTI�����˷ִأ���Ҫ������ȱ�
	if (clusterFlag) {
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");

		//����RSU����ĵȴ�����
		processWaitEventIdListWhenLocationUpdate();

		//����System������л�����
		processSwitchListWhenLocationUpdate();

		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");
	}

	//Ȼ�󣬴���RSU����ĵȴ�����
	processWaitEventIdList();
}

void RRM_RR::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		RSU &_RSU = m_RSUAry[RSUId];
		//���¼�ѹ��ȴ�����
		_RSU.m_RRM_RR->pushToWaitEventIdList(clusterIdx,eventId, m_EventVec[eventId].message.messageType);

		//���¸��¼�����־
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, RSUId, -1, -1, "Trigger");

		//��¼TTI��־
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, RSUId, -1);
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
				MessageType messageType = m_EventVec[eventId].message.messageType;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
					//��ʣ�����bit����
					m_EventVec[eventId].message.reset();

					//������ӵ�System�����RSU�л�������
					_RSU.m_RRM_RR->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, -1);
				}else if(m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx!= clusterIdx){//��Ȼ���ڵ�ǰRSU��Χ�ڣ�����λ�ڲ�ͬ�Ĵ�
					//����ת�Ƶ���ǰRSU����������
					_RSU.m_RRM_RR->pushToWaitEventIdList(m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId, messageType);

					//����ӵȴ�������ɾ��
					it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1);
				}else {
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

		_RSU.m_RRM_RR->pushToWaitEventIdList(clusterIdx, eventId, m_EventVec[eventId].message.messageType);

		//��Switch������ɾ��
		it = m_SwitchEventIdList.erase(it);

		//���¸��¼�����־
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, RSUId, -1, -1, "LocationUpdate");

		//��¼TTI��־
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, RSUId, -1);

	}
}


void RRM_RR::processWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			int count = 0;
			list<int>::iterator it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].end() && count++ < ns_RRM_RR::gc_RRTotalPatternNum) {
				int eventId = *it;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				//ѹ���������
				_RSU.m_RRM_RR->pushToAdmitEventIdList(clusterIdx, eventId);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, RSUId, -1, -1, "Accept");

				//��¼TTI��־
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, RSUId, -1);

				it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);
			}
			if (_RSU.m_RRM_RR->m_AdmitEventIdList[clusterIdx].size() > ns_RRM_RR::gc_RRTotalPatternNum)
				throw Exp("RRProcessWaitEventIdList()");
		}
	}
}


void RRM_RR::roundRobin() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < _RSU.m_RRM_RR->m_AdmitEventIdList[clusterIdx].size(); patternIdx++) {
				int eventId = _RSU.m_RRM_RR->m_AdmitEventIdList[clusterIdx][patternIdx];
				int VeUEId = m_EventVec[eventId].VeUEId;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				_RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx] = new RSU::RRM_RR::ScheduleInfo(eventId, messageType, VeUEId, _RSU.m_GTT->m_RSUId, patternIdx);
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
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx] == nullptr)continue;
				m_EventVec[_RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx]->eventId].sendDelay++;
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
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
				RSU::RRM_RR::ScheduleInfo *&info = _RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				m_InterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
	}


	//����ÿ�����ĸ��ų����б�	
	for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
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


void RRM_RR::transimitStartThread(int fromRSUId, int toRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//����ÿ���̵߳ĸ�ģ����в�ͬ��״̬���޷����������������ģ�����ڱ��μ���
	for (int RSUId = fromRSUId; RSUId <= toRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
				RSU::RRM_RR::ScheduleInfo *&info = _RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx];

				if (info == nullptr) continue;
				int VeUEId = info->VeUEId;

				//����SINR����ȡ���Ʊ��뷽ʽ
				pair<int, int> &subCarrierIdxRange = getOccupiedSubCarrierRange(patternIdx);
				g_FileTemp << "PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//���Ʊ��뷽ʽ��Ҫ����ʱ
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(ns_RRM_RR::gc_RRNumRBPerPattern * gc_BitNumPerRB)* factor);

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
	}
}


void RRM_RR::writeScheduleInfo(ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {

		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			out << "    RSU[" << _RSU.m_GTT->m_RSUId << "] :" << endl;
			out << "    {" << endl;
			out << "        Cluster[" << clusterIdx << "] :" << endl;
			out << "        {" << endl;
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
				out << "            Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
				RSU::RRM_RR::ScheduleInfo* &info = _RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx];
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


void RRM_RR::writeTTILogInfo(ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int patternIdx) {
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


void RRM_RR::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {

			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {

				RSU::RRM_RR::ScheduleInfo* &info = _RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;

				if (m_EventVec[info->eventId].message.isFinished()) {//˵���Ѿ��������
					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, -1, patternIdx, "Succeed");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
				}
				else {//û�д�����ϣ�ת��Wait�����ȴ���һ�ε���
					_RSU.m_RRM_RR->pushToWaitEventIdList(clusterIdx, info->eventId, m_EventVec[info->eventId].message.messageType);

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, patternIdx, "WaitNextTurn");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
				}
			}
		}
	}
}


pair<int, int> RRM_RR::getOccupiedSubCarrierRange(int patternIdx) {
	pair<int, int> res;

	res.first = ns_RRM_RR::gc_RRNumRBPerPattern * patternIdx;
	res.second = ns_RRM_RR::gc_RRNumRBPerPattern * (patternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
