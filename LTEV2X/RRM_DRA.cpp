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
#include<set>
#include"RRM_DRA.h"
#include"Exception.h"

using namespace std;

RRM_DRA::RRM_DRA(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput, DRAMode systemDRAMode, WT_Basic* systemWTPoint, GTAT_Basic* systemGTATPoint) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList, systemTTIRSUThroughput), m_DRAMode(systemDRAMode), m_WTPoint(systemWTPoint), m_GTATPoint(systemGTATPoint) {

	m_DRAInterferenceVec = vector<std::pair<MessageType, std::list<int>>>(gc_DRAEmergencyTotalPatternNum + gc_DRATotalPatternNum);
}


void RRM_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//��Դ������Ϣ���:����ÿ��RSU�ڵĽ��������
	DRAInformationClean();

	//���ݴش�С����ʱ����Դ�Ļ���
	DRAGroupSizeBasedTDM(clusterFlag);

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
	//ͳ��ʱ����Ϣ
	DRADelaystatistics();

	//֡����ͻ
	DRAConflictListener();

	//����������˵�Ԫ���������Ӧ����
	DRATransimitPreparation();

	//ģ�⴫�俪ʼ�����µ�����Ϣ
	DRATransimitStart();

	//д�������Ϣ
	DRAWriteScheduleInfo(g_FileDRAScheduleInfo);

	//ģ�⴫�������ͳ��������
	DRATransimitEnd();
}


void RRM_DRA::DRAInformationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_RRM_DRA->m_DRAAdmitEventIdList.clear();
		_RSU.m_RRM_DRA->m_DRAEmergencyAdmitEventIdList.clear();
	}
}


void RRM_DRA::DRAGroupSizeBasedTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		
		//�������������RSU����һ����ʱ
		if (_RSU.m_GTAT->m_VeUEIdList.size() == 0) {
			/*-----------------------ATTENTION-----------------------
			* ����ֵΪ(0,-1,0)�ᵼ�»�ȡ��ǰ�ر��ʧ�ܣ����������ط���Ҫ����
			* ���ֱ�Ӹ�ÿ���ض���ֵΪ�������䣬����Ҳû���κ����ã����������������
			*------------------------ATTENTION-----------------------*/
			_RSU.m_RRM_DRA->m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTAT->m_DRAClusterNum, tuple<int, int, int>(0, gc_DRA_NTTI - 1, gc_DRA_NTTI));
			continue;
		}

		//��ʼ��
		_RSU.m_RRM_DRA->m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTAT->m_DRAClusterNum, tuple<int, int, int>(0, 0, 0));

		//����ÿ��TTIʱ϶��Ӧ��VeUE��Ŀ(double)��!!!������!!��
		double VeUESizePerTTI = static_cast<double>(_RSU.m_GTAT->m_VeUEIdList.size()) / static_cast<double>(gc_DRA_NTTI);

		//clusterSize�洢ÿ���ص�VeUE��Ŀ(double)��!!!������!!��
		std::vector<double> clusterSize(_RSU.m_GTAT->m_DRAClusterNum, 0);

		//��ʼ��clusterSize
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_DRAClusterNum; clusterIdx++)
			clusterSize[clusterIdx] = static_cast<double>(_RSU.m_GTAT->m_DRAClusterVeUEIdList[clusterIdx].size());

		//���ȸ�������һ�����Ĵط���һ��TTI
		int basicNTTI = 0;
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_DRAClusterNum; clusterIdx++) {
			//����ô���������һ��VeUE��ֱ�ӷ����һ����λ��ʱ����Դ
			if (_RSU.m_GTAT->m_DRAClusterVeUEIdList[clusterIdx].size() != 0) {
				get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) = 1;
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
			get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[dex])++;
			remainNTTI--;
			clusterSize[dex] -= VeUESizePerTTI;
		}

		//��ʼ�������䷶Χ��������
		get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[0]) = 0;
		get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[0]) = get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[0]) + get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[0]) - 1;
		for (int clusterIdx = 1; clusterIdx < _RSU.m_GTAT->m_DRAClusterNum; clusterIdx++) {
			get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) = get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx - 1]) + 1;
			get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) = get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) + get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) - 1;
		}


		//����������д���RSU�ڵ�ÿһ������
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_DRAClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU.m_GTAT->m_DRAClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId].m_RRM_DRA->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]), get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]));
		}
	}
	DRAWriteClusterPerformInfo(g_FileClasterPerformInfo);
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
		RSU &_RSU = m_RSUAry[RSUId];

		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);//��ǰ�ɴ������ݵĴر��
		for (int eventId : m_EventTTIList[m_TTI]) {
			Event event = m_EventVec[eventId];
			
			int VeUEId = event.VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//��ǰ�¼���Ӧ��VeUE���ڵ�ǰRSU�У���������
				continue;
			}
			else {//��ǰ�¼���Ӧ��VeUE�ڵ�ǰRSU��
				if (m_EventVec[eventId].message.messageType == EMERGENCY) {//���ǽ������¼�
					/*  EMERGENCY  */
					//�����¼�ѹ������¼��ȴ�����
					_RSU.m_RRM_DRA->DRAPushToEmergencyWaitEventIdList(eventId);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Trigger");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
					/*  EMERGENCY  */
				}
				else {//�ǽ������¼�
				    //�����¼�ѹ��ȴ�����
					_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(eventId);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Trigger");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
				}
			}
		}
	}
}


void RRM_DRA::DRAProcessScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			if (_RSU.m_RRM_DRA->m_DRAEmergencyScheduleInfoTable[patternIdx] == nullptr) {//��ǰEmergencyPattern���¼�����
				continue;
			}
			else {
				int eventId = _RSU.m_RRM_DRA->m_DRAEmergencyScheduleInfoTable[patternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
					//ѹ��Switch����
					_RSU.m_RRM_DRA->DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);

					//��ʣ�����bit����
					m_EventVec[eventId].message.resetRemainBitNum();

					//���ͷŸõ�����Ϣ����Դ
					delete _RSU.m_RRM_DRA->m_DRAEmergencyScheduleInfoTable[patternIdx];
					m_DeleteCount++;
					_RSU.m_RRM_DRA->m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx] = true;

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "LocationUpdate");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1, patternIdx);
				}
				else {//�ó������ڵ�ǰRSU��
					  //doing nothing 
					continue;
				}
			}
		}
		/*  EMERGENCY  */

		//��ʼ���� m_DRAScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_DRAClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx] == nullptr) {//��ǰPattern�����¼��ڴ���
					continue;
				}
				else {
					int eventId = _RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx]->eventId;
					int VeUEId = m_EventVec[eventId].VeUEId;
					if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
						//ѹ��Switch����
						_RSU.m_RRM_DRA->DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);

						//��ʣ�����bit����
						m_EventVec[eventId].message.resetRemainBitNum();

						//���ͷŸõ�����Ϣ����Դ
						delete _RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx];
						m_DeleteCount++;
						_RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

						//�ͷ�Pattern��Դ
						_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

						//���¸��¼�����־
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx, "LocationUpdate");

						//��¼TTI��־
						DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx);
					}
					else {
						if (_RSU.m_RRM_DRA->DRAGetClusterIdxOfVeUE(VeUEId) != clusterIdx) {//RSU�ڲ������˴��л�������ӵ��ȱ���ȡ����ѹ��ȴ�����
							//ѹ���RSU�ĵȴ�����
							_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(eventId);

							//��ʣ�����bit����
							m_EventVec[eventId].message.resetRemainBitNum();

							//���ͷŸõ�����Ϣ����Դ
							delete _RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx];
							m_DeleteCount++;
							_RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

							//�ͷ�Pattern��Դ
							_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

							//���¸��¼�����־
							m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx, "LocationUpdate");

							//��¼TTI��־
							DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx);
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
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		//��ʼ����m_DRAEmergencyWaitEventIdList
		list<int>::iterator it = _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList.begin();
		while (it != _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
                //������ӵ�System�����RSU�л�������
				_RSU.m_RRM_DRA->DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);
				
                //�ӵȴ�������ɾ��
				it = _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList.erase(it);

				//��ʣ�����bit����
				m_EventVec[eventId].message.resetRemainBitNum();

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

				//��¼TTI��־
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
			}
			else {//��Ȼ���ڵ�ǰRSU��Χ��
				it++;
				continue; //�������ڵ�ǰRSU��Emergency�ȴ�����
			}
		}
		/*  EMERGENCY  */


		//��ʼ���� m_DRAWaitEventIdList
		it = _RSU.m_RRM_DRA->m_DRAWaitEventIdList.begin();
		while (it != _RSU.m_RRM_DRA->m_DRAWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
				//������ӵ�System�����RSU�л�������
				_RSU.m_RRM_DRA->DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);
				
			    //����ӵȴ�������ɾ��
				it = _RSU.m_RRM_DRA->m_DRAWaitEventIdList.erase(it);

				//��ʣ�����bit����
				m_EventVec[eventId].message.resetRemainBitNum();

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

				//��¼TTI��־
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
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
		RSU &_RSU = m_RSUAry[RSUId];

		list<int>::iterator it = m_DRASwitchEventIdList.begin();
		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);//��ǰ�ɴ������ݵĴر��
		while (it !=m_DRASwitchEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//���л������е��¼���Ӧ��VeUE�������ڵ�ǰ�أ���������
				it++;
				continue;
			}
			else {//���л������е�VeUE�����ڵ�RSU
				  /*  EMERGENCY  */
				if (m_EventVec[eventId].message.messageType == EMERGENCY) {//���ڽ����¼�
					//ת��ȴ�����
					_RSU.m_RRM_DRA->DRAPushToEmergencyWaitEventIdList(eventId);

					//��Switch���н���ɾ��
					it = m_DRASwitchEventIdList.erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
				}
				/*  EMERGENCY  */

				else {//�ǽ����¼�
					//ת��ȴ�����
					_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(eventId);

					//��Switch���н���ɾ��
					it = m_DRASwitchEventIdList.erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
				}
			}
		}
	}
}


void RRM_DRA::DRAProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList) {
			_RSU.m_RRM_DRA->DRAPushToEmergencyAdmitEventIdList(eventId);//���¼�ѹ���������

		    //���¸��¼�����־
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTAT->m_RSUId, -1, -1, "TryAccept");

			//��¼TTI��־
			DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
		}

		//���ڻ�ȫ��ѹ��Emergency�����������ֱ����վ����ˣ�����Ҫһ����ɾ��
		_RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList.clear();
		/*  EMERGENCY  */


		//��ʼ���� m_DRAWaitEventIdList
		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);//���ڵ����еĴر��
		list<int>::iterator it = _RSU.m_RRM_DRA->m_DRAWaitEventIdList.begin();
		while (it != _RSU.m_RRM_DRA->m_DRAWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx == clusterIdx) {//���¼���ǰ���Խ��е���
				_RSU.m_RRM_DRA->DRAPushToAdmitEventIdList(eventId);//��ӵ�RSU����Ľ���������
				it = _RSU.m_RRM_DRA->m_DRAWaitEventIdList.erase(it);//����ӵȴ�������ɾ��

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTAT->m_RSUId, -1, -1, "TryAccept");

				//��¼TTI��־
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
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
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		vector<int> curAvaliableEmergencyPatternIdx;//��ǰ���õ�EmergencyPattern���

		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			if (_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx]) {
				curAvaliableEmergencyPatternIdx.push_back(patternIdx);
			}
		}

		for (int eventId : _RSU.m_RRM_DRA->m_DRAEmergencyAdmitEventIdList) {
			int VeUEId = m_EventVec[eventId].VeUEId;

			//Ϊ��ǰ�û��ڿ��õ�EmergencyPattern�������ѡ��һ����ÿ���û��������ѡ�����EmergencyPattern��
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_DRA->DRARBEmergencySelectBasedOnP2(curAvaliableEmergencyPatternIdx);

			if (patternIdx == -1) {//�޶�ӦPattern���͵�pattern��Դ����
				_RSU.m_RRM_DRA->DRAPushToEmergencyWaitEventIdList(eventId);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "AllBusy");

				//��¼TTI��־
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);

				continue;
			}

			//����Դ���Ϊռ��
			_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx] = false;

			//��������Ϣѹ��m_DRAEmergencyTransimitEventIdList��
			_RSU.m_RRM_DRA->DRAPushToEmergencyTransmitScheduleInfoList(new RSU::RRM_DRA::DRAScheduleInfo(eventId, VeUEId, _RSU.m_GTAT->m_RSUId, patternIdx), patternIdx);
			m_NewCount++;
		}
		_RSU.m_RRM_DRA->DRAPullFromEmergencyScheduleInfoTable();
		/*  EMERGENCY  */


		//��ʼ�����Emergency���¼�
		int roundATTI = m_TTI%gc_DRA_NTTI;//��TTIӳ�䵽[0-gc_DRA_NTTI)�ķ�Χ

		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);

		/*
		* ��ǰTTI���õ�Pattern����
		* �±�����Pattern����ı��
		* ÿ���ڲ�vector���������Pattern���õ�Pattern���
		*/
		vector<vector<int>> curAvaliablePatternIdx(gc_DRAPatternTypeNum);

		for (int patternTypeIdx = 0; patternTypeIdx < gc_DRAPatternTypeNum; patternTypeIdx++) {
			for (int patternIdx = gc_DRAPatternTypePatternIdxInterval[patternTypeIdx][0]; patternIdx <= gc_DRAPatternTypePatternIdxInterval[patternTypeIdx][1]; patternIdx++) {
				if (_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		for (int eventId : _RSU.m_RRM_DRA->m_DRAAdmitEventIdList) {//�����ô��ڽ��������е��¼�

			int VeUEId = m_EventVec[eventId].VeUEId;

			//Ϊ��ǰ�û��ڿ��õĶ�Ӧ���¼����͵�Pattern�������ѡ��һ����ÿ���û��������ѡ�����Pattern��
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_DRA->DRARBSelectBasedOnP2(curAvaliablePatternIdx, m_EventVec[eventId].message.messageType);

			if (patternIdx == -1) {//���û��������Ϣ����û��patternʣ����
				_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(eventId);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "AllBusy");

				//��¼TTI��־
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
				continue;
			}

			//����Դ���Ϊռ��
			_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][patternIdx] = false;

			//��������Ϣѹ��m_DRATransimitEventIdList��
			_RSU.m_RRM_DRA->DRAPushToTransmitScheduleInfoList(new RSU::RRM_DRA::DRAScheduleInfo(eventId, VeUEId, _RSU.m_GTAT->m_RSUId, patternIdx), patternIdx);
			m_NewCount++;
		}

		//�����ȱ��е�ǰ���Լ���������û�ѹ�봫��������
		_RSU.m_RRM_DRA->DRAPullFromScheduleInfoTable(m_TTI);
	}
}

void RRM_DRA::DRADelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*
		* �ú���Ӧ����DRASelectBasedOnP..֮�����
		* ��ʱ�¼����ڵȴ����������������ߵ��ȱ��У��ǵ�ǰ�أ�
		*/

		//����ȴ�����
		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;
		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_DRA->m_DRAWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//����˿����ڽ�Ҫ����Ĵ�������
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++)
			for (RSU::RRM_DRA::DRAScheduleInfo* &p : _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			for (RSU::RRM_DRA::DRAScheduleInfo* &p : _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		}

		//����˿�λ�ڵ��ȱ��У��������ڵ�ǰ�ص��¼�
		int curClusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_DRAClusterNum; clusterIdx++) {
			if (clusterIdx == curClusterIdx) continue;
			for (RSU::RRM_DRA::DRAScheduleInfo *p : _RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx]) {
				if (p == nullptr) continue;
				m_EventVec[p->eventId].queuingDelay++;
			}
		}
	}
}

void RRM_DRA::DRAConflictListener() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
				for (RSU::RRM_DRA::DRAScheduleInfo* &info : lst) {
					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Transimit");

					//���Ƚ��¼�ѹ��ȴ��б�
					_RSU.m_RRM_DRA->DRAPushToEmergencyWaitEventIdList(info->eventId);

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Conflict");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTAT->m_RSUId, -1, -1);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
					m_DeleteCount++;
				}

				//�ͷ�Pattern��Դ
				_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx] = true;

				lst.clear();
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
				for (RSU::RRM_DRA::DRAScheduleInfo* &info : lst) {
					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx, "Transimit");

					//���Ƚ��¼�ѹ��ȴ��б�
					_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(info->eventId);

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Conflict");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTAT->m_RSUId, -1, -1);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete info;
					info = nullptr;
					m_DeleteCount++;
				}
				//�ͷ�Pattern��Դ
				_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

				lst.clear();
			}
		}
	}
}


void RRM_DRA::DRATransimitPreparation() {
	//���������һ�θ�����Ϣ
	for (std::pair<MessageType, std::list<int>>&lst : m_DRAInterferenceVec) {
		lst.second.clear();
	}

	//ͳ�Ʊ��εĸ�����Ϣ
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();
				MessageType messageType = m_EventVec[info->eventId].message.messageType;
				m_DRAInterferenceVec[patternIdx].first = messageType;
				m_DRAInterferenceVec[patternIdx].second.push_back(info->VeUEId);
			}
		}
		/*  EMERGENCY  */

		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int relativePatternIdx = 0; relativePatternIdx < gc_DRATotalPatternNum; relativePatternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();
				MessageType messageType = m_EventVec[info->eventId].message.messageType;
				m_DRAInterferenceVec[relativePatternIdx + gc_DRAEmergencyTotalPatternNum].first = messageType;
				m_DRAInterferenceVec[relativePatternIdx + gc_DRAEmergencyTotalPatternNum].second.push_back(info->VeUEId);
			}
		}
	}
    
	/*
	* ��ÿ�����ĸ��ų����б�
	* ΪʲôҪ��set������vector����list
	* ��Ϊ��ͬһ��������ͬʱ�����˲�ͬ���¼�������ͬʱ��PERIOD��DATA��������ô�ڲ�ͬ��patternIdx��������ͬ�ĳ���ID
	* ��������ͬ������IDֻ����һ��
	*/
	
	for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum + gc_DRATotalPatternNum; patternIdx++) {
		pair<MessageType, list<int>> &p = m_DRAInterferenceVec[patternIdx];
		MessageType messageType = p.first;
		list<int>& lst = p.second;
		for (int VeUEId : lst) {

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUENum[messageType] = (int)lst.size() - 1;//д�������Ŀ
			
			set<int> s(lst.begin(), lst.end());//���ڸ��¸����б��
			s.erase(VeUEId);

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEVec[messageType].assign(s.begin(), s.end());//д����ų���ID

			g_FileTemp << "VeUEId: " << VeUEId << " [";
			for (auto c : m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEVec[messageType])
				g_FileTemp << c << ", ";
			g_FileTemp << " ]" << endl;
		}
	}

	//����������˵�Ԫ���������Ӧ����
	long double start = clock();
	m_GTATPoint->calculateInterference(m_DRAInterferenceVec);
	long double end = clock();
	m_GTATTimeConsume += end - start;
}


void RRM_DRA::DRATransimitStart() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();

				//����SINR����ȡ���Ʊ��뷽ʽ
				long double start = clock();
				pair<int, int> &subCarrierIdxRange = DRAGetOccupiedSubCarrierRange(m_EventVec[info->eventId].message.messageType, patternIdx);
				MessageType messageType = m_EventVec[info->eventId].message.messageType;
				g_FileTemp << "Emergency PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  "  <<((messageType==-1)?"Emergency":(messageType==0?"Period":"Data"))<< endl;
				tuple<ModulationType, int, double>curModulationAndRate = m_WTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, messageType);
				double factor = get<1>(curModulationAndRate) * get<2>(curModulationAndRate);
				long double end = clock();
				m_WTTimeConsume += end - start;

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(gc_DRAEmergencyRBNumPerPattern*gc_BitNumPerRB)* factor);

				//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit����(ȡ����ʣ�����bit�����Ƿ����maxEquivalentBitNum)
				int realEquivalentBitNum = m_EventVec[info->eventId].message.remainBitNum>maxEquivalentBitNum ? maxEquivalentBitNum : m_EventVec[info->eventId].message.remainBitNum;
				
				//�ۼ�������
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTAT->m_RSUId] += realEquivalentBitNum;

				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Transimit");

				//����ʣ�������bit����
				m_EventVec[info->eventId].message.remainBitNum -= realEquivalentBitNum;

				//���µ�����Ϣ
				info->transimitBitNum = realEquivalentBitNum;
				info->remainBitNum = m_EventVec[info->eventId].message.remainBitNum;
				info->occupiedTTINum = (int)ceil((double)info->remainBitNum / (double)info->transimitBitNum);
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();

				int patternType = DRAGetPatternType(patternIdx);

				//����SINR����ȡ���Ʊ��뷽ʽ
				long double start = clock();
				pair<int, int> &subCarrierIdxRange = DRAGetOccupiedSubCarrierRange(m_EventVec[info->eventId].message.messageType, patternIdx);
				MessageType messageType = m_EventVec[info->eventId].message.messageType;
				g_FileTemp << "NoEmergency PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << ((messageType == -1) ? "Emergency" : (messageType == 0 ? "Period" : "Data")) << endl;
				tuple<ModulationType, int, double>curModulationAndRate = m_WTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, messageType);
				double factor = get<1>(curModulationAndRate) * get<2>(curModulationAndRate);
				long double end = clock();
				m_WTTimeConsume += end - start;

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(gc_DRA_RBNumPerPatternType[patternType] * gc_BitNumPerRB)* factor);

				//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit����(ȡ����ʣ�����bit�����Ƿ����maxEquivalentBitNum)
				int realEquivalentBitNum = m_EventVec[info->eventId].message.remainBitNum>maxEquivalentBitNum ? maxEquivalentBitNum : m_EventVec[info->eventId].message.remainBitNum;

				//�ۼ�������
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTAT->m_RSUId] += realEquivalentBitNum;

				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx, "Transimit");

				//����ʣ�������bit����
				m_EventVec[info->eventId].message.remainBitNum -= realEquivalentBitNum;
	
				//���µ�����Ϣ
				info->transimitBitNum = realEquivalentBitNum;
				info->remainBitNum = m_EventVec[info->eventId].message.remainBitNum;
				info->occupiedTTINum = (int)ceil((double)info->remainBitNum / (double)info->transimitBitNum);
			}
		}
	}
}

void RRM_DRA::DRAWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		out << "    RSU[" << _RSU.m_GTAT->m_RSUId << "][TTI = " << m_TTI << "]" << endl;
		out << "    {" << endl;
		out << "    EMERGENCY:" << endl;
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			bool isAvaliable = _RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *(_RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    PERIOD:" << endl;
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[PERIOD]; patternIdx++) {
			bool isAvaliable = _RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][patternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM_DRA::DRAScheduleInfo *info=*(_RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[patternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    DATA:" << endl;
		for (int patternIdx = gc_DRAPatternNumPerPatternType[PERIOD]; patternIdx < gc_DRAPatternNumPerPatternType[PERIOD] + gc_DRAPatternNumPerPatternType[DATA]; patternIdx++) {
			bool isAvaliable = _RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][patternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *(_RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[patternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    }" << endl;

	}
	out << "}" << endl;
	out << "\n\n" << endl;
}

void RRM_DRA::DRATransimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();
				if (m_EventVec[info->eventId].message.remainBitNum == 0) {//�Ѿ�������ϣ�����Դ�ͷ�

					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Succeed");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTAT->m_RSUId, -1, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete *lst.begin();
					m_DeleteCount++;

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx] = true;
				}
				else {//��δ�������
					_RSU.m_RRM_DRA->DRAPushToEmergencyScheduleInfoTable(patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			lst.clear();
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();
				if (m_EventVec[info->eventId].message.remainBitNum == 0) {//˵���������Ѿ��������

					//���ô���ɹ����
					m_EventVec[info->eventId].isSuccessded = true;

					//���¸��¼�����־
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx, "Succeed");

					//��¼TTI��־
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx);

					//�ͷŵ�����Ϣ������ڴ���Դ
					delete *lst.begin();
					m_DeleteCount++;

					//�ͷ�Pattern��Դ
					_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;
				}
				else {//��������δ���꣬����ѹ��m_DRAScheduleInfoTable
					_RSU.m_RRM_DRA->DRAPushToScheduleInfoTable(clusterIdx, patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			//������󣬽���pattern�ϵ�������գ���ʱҪ��������ǿգ�Ҫ������nullptrָ�룩
			lst.clear();
		}
	}
}


void RRM_DRA::DRAWriteTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx) {
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


void RRM_DRA::DRAWriteClusterPerformInfo(std::ofstream &out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;

	//��ӡVeUE��Ϣ
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUE &_VeUE = m_VeUEAry[VeUEId];
		out << _VeUE.m_RRM_DRA->toString(2) << endl;
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
		out << _RSU.m_RRM_DRA->toString(2) << endl;
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


int RRM_DRA::DRAGetPatternType(int patternIdx) {
	//patternIdx����ָ�ǽ����¼�����ţ���0��ʼ���
	for (int patternType = 0; patternType < gc_DRAPatternTypeNum; patternType++) {
		if (patternIdx >= gc_DRAPatternTypePatternIdxInterval[patternType][0] && patternIdx <= gc_DRAPatternTypePatternIdxInterval[patternType][1])
			return patternType;
	}
	throw Exp("getPatternType");
}


pair<int, int> RRM_DRA::DRAGetOccupiedSubCarrierRange(MessageType messageType, int patternIdx) {
	/*
	* ���ڽ����¼����ԣ�patternIdx��0��ʼ
	* ���ڷǽ����¼���patternIdxҲ�Ǵ�0��ʼ���������ж����¼����ڣ����Ҫ����ƫ�Ƶ�����
	*/
	pair<int, int> res;
	if (messageType == EMERGENCY) {
		res.first = gc_DRAEmergencyRBNumPerPattern*patternIdx;
		res.second = gc_DRAEmergencyRBNumPerPattern*(patternIdx + 1) - 1;
	}
	else{
		int offset = gc_DRAEmergencyRBNumPerPattern * gc_DRAEmergencyTotalPatternNum;
		for (int i = 0; i < messageType; i++) {
			offset += gc_DRA_RBNumPerPatternType[i] * gc_DRAPatternNumPerPatternType[i];
			patternIdx -= gc_DRAPatternNumPerPatternType[i];
		}
		res.first = offset + gc_DRA_RBNumPerPatternType[messageType] * patternIdx;
		res.second = offset + gc_DRA_RBNumPerPatternType[messageType] * (patternIdx + 1) - 1;
	}
	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
