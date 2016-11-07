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

#include<iomanip>
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
	writeScheduleInfo(g_FileScheduleInfo);

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
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(m_SwitchEventIdList,eventId);

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
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

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
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {

			/*
			* ��ǰTTI��ǰ�ؿ��õ�Pattern����
			* ÿ���ڲ�vector���������Pattern���õ�Pattern���(����Pattern���)
			*/
			vector<int> curAvaliablePatternIdx;

			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliablePatternIdx.push_back(patternIdx);
				}
			}

			list<int>::iterator it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].begin();
			while (it!= _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].end()) {//�����ô��ڽ��������е��¼�
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;

				//Ϊ��ǰ�û��ڿ��õĶ�Ӧ���¼����͵�Pattern�������ѡ��һ����ÿ���û��������ѡ�����Pattern��
				int patternIdx = m_VeUEAry[VeUEId].m_RRM_ICC_DRA->selectRBBasedOnP2(curAvaliablePatternIdx);

				if (patternIdx == -1) {//���û��������Ϣ����û��patternʣ����
					_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx, eventId);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "AllBusy");

					//��¼TTI��־
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1);
					
					it++;
					continue;
				}

				//����Դ���Ϊռ��
				_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

				//��������Ϣѹ��m_TransimitEventIdList��
				_RSU.m_RRM_ICC_DRA->pushToTransmitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx));

				it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].erase(it);
			}

			//<UNDOWN>Ŀǰ��ʱ����Ҫ���ȱ���ΪĬ�ϲ������ռ����Դ
			////�����ȱ��е�ǰ���Լ���������û�ѹ�봫��������
			//_RSU.m_RRM_ICC_DRA->pullFromScheduleInfoTable(m_TTI);
		}
	}
}


void RRM_ICC_DRA::delaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];


		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//����ȴ�����
			for (int eventId : _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx])
				m_EventVec[eventId].queuingDelay++;

			//����˿����ڽ�Ҫ����Ĵ�������
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
					m_EventVec[p->eventId].sendDelay++;
			}

			//<UNDOWN>Ŀǰ��ʱ����Ҫ���ȱ���ΪĬ�ϲ������ռ����Դ
			////����˿�λ�ڵ��ȱ��У��������ڵ�ǰ�ص��¼�
			//int curClusterIdx = _RSU.m_RRM_ICC_DRA->getClusterIdx(m_TTI);
			//for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//	if (clusterIdx == curClusterIdx) continue;
			//	for (RSU::RRM::ScheduleInfo *p : _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx]) {
			//		if (p == nullptr) continue;
			//		m_EventVec[p->eventId].queuingDelay++;
			//	}
			//}
		}
		
	}
}


void RRM_ICC_DRA::conflictListener() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx =0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {

				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
					for (RSU::RRM::ScheduleInfo* &info : lst) {
						//���¸��¼�����־
						m_EventVec[info->eventId].addEventLog(m_TTI, CONFLICT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Conflict");

						//���Ƚ��¼�ѹ��ȴ��б�
						_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx, info->eventId);

						//���¸��¼�����־
						m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Conflict");

						//��¼TTI��־
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1);

						//�ͷŵ�����Ϣ������ڴ���Դ
						delete info;
						info = nullptr;
					}
					//�ͷ�Pattern��Դ
					_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					lst.clear();
				}
			}
		}
	}
}


void RRM_ICC_DRA::transimitPreparation() {
	//���������һ�θ�����Ϣ
	for (list<int>&lst : m_InterferenceVec) {
		lst.clear();
	}

	//ͳ�Ʊ��εĸ�����Ϣ
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
					RSU::RRM::ScheduleInfo *info = *lst.begin();
					m_InterferenceVec[patternIdx].push_back(info->VeUEId);
				}
			}
		}
	}

	//����ÿ�����ĸ��ų����б�	
	for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
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


void RRM_ICC_DRA::transimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_ICC_DRA::transimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}


void RRM_ICC_DRA::transimitStartThread(int t_FromRSUId, int t_ToRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//����ÿ���̵߳ĸ�ģ����в�ͬ��״̬���޷����������������ģ�����ڱ��μ���
	for (int RSUId = t_FromRSUId; RSUId <= t_ToRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {

				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
					RSU::RRM::ScheduleInfo *info = *lst.begin();
					int VeUEId = info->VeUEId;

					//����SINR����ȡ���Ʊ��뷽ʽ
					pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(patternIdx);
					g_FileTemp << "NonEmergencyPatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

					if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//���Ʊ��뷽ʽ��Ҫ����ʱ
						m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
						m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
						m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
					}
					double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

					//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
					int maxEquivalentBitNum = (int)((double)(ns_RRM_ICC_DRA::gc_RBNumPerPattern * gc_BitNumPerRB)* factor);

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
		
	}
	delete copyWTPoint;//getCopy��ͨ��new�����ģ���������ͷ���Դ
	copyWTPoint = nullptr;
}


void RRM_ICC_DRA::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {

				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
					RSU::RRM::ScheduleInfo *info = *lst.begin();
					if (m_EventVec[info->eventId].message.isFinished()) {//˵���������Ѿ��������

						//���ô���ɹ����
						m_EventVec[info->eventId].isSuccessded = true;

						//���¸��¼�����־
						m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Succeed");

						//��¼TTI��־
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx);

					}
					else {//��������δ���꣬����ѹ��m_WaitEventIdList��Ŀǰ��ʱ�ǲ�֧�ֳ���ռ�õ�<UNDOWN>
						_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx, info->eventId);
					}
					//�ͷŵ�����Ϣ������ڴ���Դ
					delete *lst.begin();
					*lst.begin() = nullptr;

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;
				}
				//������󣬽���pattern�ϵ�������գ���ʱҪ��������ǿգ�Ҫ������nullptrָ�룩
				lst.clear();
			}
		}
	}
}


void RRM_ICC_DRA::writeScheduleInfo(ofstream& t_File) {
	t_File << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	t_File << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {

		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			t_File << "    RSU[" << _RSU.m_GTT->m_RSUId << "] :" << endl;
			t_File << "    {" << endl;
			t_File << "        Cluster[" << clusterIdx << "] :" << endl;
			t_File << "        {" << endl;
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				t_File << "            Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
				RSU::RRM::ScheduleInfo* &info = _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				t_File << info->toScheduleString(3) << endl;
			}
			t_File << "        }" << endl;
			t_File << "    }" << endl;
		}

	}
	t_File << "}" << endl;
	t_File << "\n\n" << endl;
}


void RRM_ICC_DRA::writeTTILogInfo(std::ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx) {
	stringstream ss;
	switch (t_EventLogType) {
	case SUCCEED:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ RSU[" << t_RSUId << "]   ClusterIdx[" << t_ClusterIdx << "]    PatternIdx[" << t_PatternIdx << "] }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[0]Succeed";
		t_File << "    " << ss.str() << endl;
		break;
	case EVENT_TO_WAIT:
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[2]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: RSU[" << t_RSUId << "]'s ScheduleTable[" << t_ClusterIdx << "][" << t_PatternIdx << "] ; To: SwitchList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[3]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: RSU[" << t_RSUId << "]'s ScheduleTable[" << t_ClusterIdx << "][" << t_PatternIdx << "] ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[4]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case WAIT_TO_SWITCH:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: RSU[" << t_RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[5]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case WAIT_TO_ADMIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: RSU[" << t_RSUId << "]'s WaitEventIdList ; To: RSU[" << t_RSUId << "]'s AdmitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[6]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case SWITCH_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[8]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case TRANSIMIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ RSU[" << t_RSUId << "]'s TransimitScheduleInfoList ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[9]Conflict";
		t_File << "    " << ss.str() << endl;
		break;
	case ADMIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ RSU[" << t_RSUId << "]'s AdmitEventIdList ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[11]Conflict";
		t_File << "    " << ss.str() << endl;
		break;
	}
}


void RRM_ICC_DRA::writeClusterPerformInfo(ofstream& t_File) {
	t_File << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	t_File << "{" << endl;

	//��ӡVeUE��Ϣ
	t_File << "    VUE Info: " << endl;
	t_File << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUE &_VeUE = m_VeUEAry[VeUEId];
		t_File << _VeUE.m_RRM_ICC_DRA->toString(2) << endl;
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
		t_File << _RSU.m_RRM_ICC_DRA->toString(2) << endl;
	}
	t_File << "    }" << endl;

	t_File << "}\n\n";
}


pair<int, int> RRM_ICC_DRA::getOccupiedSubCarrierRange(int t_PatternIdx) {
	pair<int, int> res;

	res.first = ns_RRM_ICC_DRA::gc_RBNumPerPattern * t_PatternIdx;
	res.second = ns_RRM_ICC_DRA::gc_RBNumPerPattern * (t_PatternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
