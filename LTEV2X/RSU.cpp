#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Traffic.h"
#include"Global.h"


using namespace std;

int newCount = 0;//��¼��̬�����Ķ���Ĵ���

int deleteCount = 0;//��¼ɾ����̬��������Ĵ���

void cRSU::initialize(sRSUConfigure &t_RSUConfigure){
	m_RSUId = t_RSUConfigure.wRSUID;
	m_fAbsX = c_RSUTopoRatio[m_RSUId * 2 + 0] * c_wide;
	m_fAbsY = c_RSUTopoRatio[m_RSUId * 2 + 1] * c_length;
	RandomUniform(&m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU��");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_RSUId, m_fAbsX, m_fAbsY);

	m_DRAClusterNum = c_RSUClusterNum[m_RSUId];
	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);


	//RR������ʼ��
	m_RRScheduleInfoTable = vector<sRRScheduleInfo*>(m_RRPatternNum);
}


//<DONE>
void cRSU::RRInformationClean() {
	m_RRAdmitEventIdList.clear();
}

//<UNDONE>
void cRSU::RRProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList) {
	for (int eventId : systemEventTTIList[TTI]) {
		sEvent event = systemEventVec[eventId];
		int VeUEId = event.VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//��ǰ�¼���Ӧ��VeUE���ڵ�ǰRSU�У���������
			continue;
		}
		else {//��ǰ�¼���Ӧ��VeUE�ڵ�ǰRSU��
			if (systemEventVec[eventId].message.messageType == EMERGENCY) {//���ǽ����¼�
				/*  EMERGENCY  */
				RRPushToWaitEventIdList(eventId, EMERGENCY);

				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 2, m_RSUId, -1, -1);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 1, eventId, m_RSUId , -1);
				/*  EMERGENCY  */
			}
			else {//һ�����¼������������¼�����������ҵ���¼�
				//�����¼�ѹ��ȴ�����
				RRPushToWaitEventIdList(eventId,PERIOD);

				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 2, m_RSUId, -1, -1);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 1, eventId, m_RSUId, -1);
			}
		}
	}
}



//<UNDONE>
void cRSU::RRProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList) {
	//��ʼ���� m_RRWaitEventIdList
	list<int>::iterator it = m_RRWaitEventIdList.begin();
	while (it != m_RRWaitEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		eMessageType messageType = systemEventVec[eventId].message.messageType;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
			RRPushToSwitchEventIdList(eventId, systemRRSwitchEventIdList);//������ӵ�System�����RSU�л�������
			it = m_RRWaitEventIdList.erase(it);//����ӵȴ�������ɾ��

			if (messageType == EMERGENCY) {
				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 5, m_RSUId, -1, -1);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 4, eventId, m_RSUId, -1);
			}
			else if (messageType == PERIOD) {
				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 5, m_RSUId, -1, -1);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 4, eventId, m_RSUId, -1);
			}
		}
		else {//��Ȼ���ڵ�ǰRSU��Χ��
			it++;
			continue; //�������ڵ�ǰRSU�ĵȴ�����
		}
	}
}

//<UNDONE>
void cRSU::RRProcessSwitchListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList) {
	list<int>::iterator it = systemRRSwitchEventIdList.begin();
	while (it != systemRRSwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//���л������е��¼���Ӧ��VeUE�������ڵ�ǰ�أ���������
			it++;
			continue;
		}
		else {//���л������е�VeUE�����ڵ�RSU
			  /*  EMERGENCY  */
			if (systemEventVec[eventId].message.messageType == EMERGENCY) {//���ڽ����¼�
				RRPushToWaitEventIdList(eventId, EMERGENCY);
				it = systemRRSwitchEventIdList.erase(it);

				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 8, m_RSUId, -1, -1);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 5, eventId, m_RSUId, -1);
			}
			/*  EMERGENCY  */

			else {//�ǽ����¼�
				RRPushToWaitEventIdList(eventId,PERIOD);
				it = systemRRSwitchEventIdList.erase(it);

				//���¸��¼�����־
				systemEventVec[eventId].addEventLog(TTI, 8, m_RSUId, -1, -1);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 5, eventId, m_RSUId, -1);		
			}
		}
	}
}

//<UNDONE>
void cRSU::RRProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList) {
	int count = 0;
	list<int>::iterator it = m_RRWaitEventIdList.begin();
	while (it != m_RRWaitEventIdList.end() && count < m_RRPatternNum) {
		int eventId = *it;
		eMessageType messageType = systemEventVec[eventId].message.messageType;
		RRPushToAdmitEventIdList(eventId);
		if (messageType == EMERGENCY) {
			//���¸��¼�����־
			systemEventVec[eventId].addEventLog(TTI, 6, m_RSUId, -1, -1);

			//��¼TTI��־
			RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 2, eventId, m_RSUId, -1);
		}
		else if (messageType == PERIOD) {
			//���¸��¼�����־
			systemEventVec[eventId].addEventLog(TTI, 6, m_RSUId, -1, -1);

			//��¼TTI��־
			RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 2, eventId, m_RSUId, -1);
		}
		it = m_RRWaitEventIdList.erase(it);
	}
}

//<UNDONE>
void cRSU::RRProcessTransimit1(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec){
	for (int patternIdx = 0; patternIdx < static_cast<int>(m_RRAdmitEventIdList.size()); patternIdx++) {
		int eventId = m_RRAdmitEventIdList[patternIdx];
		int VeUEId = systemEventVec[eventId].VeUEId;
		int occupiedTTI = ceil((double)systemEventVec[eventId].message.bitNum / (double)gc_BitNumPerRB / (double)m_RRNumRBPerPattern);
		eMessageType messageType = systemEventVec[eventId].message.messageType;
		tuple<int, int> scheduleInterval(TTI, TTI + occupiedTTI - 1);
		m_RRScheduleInfoTable[patternIdx] = new sRRScheduleInfo(eventId, messageType, VeUEId, m_RSUId, patternIdx,scheduleInterval);
	}
}

//<UNDONE>
void cRSU::RRDelaystatistics(int TTI, std::vector<sEvent>& systemEventVec) {
	//����ȴ�����
	for (int eventId : m_RRWaitEventIdList)
		systemEventVec[eventId].queuingDelay++;

	//����˿����ڽ�Ҫ����ĵ��ȱ�
	for (int patternIdx = 0; patternIdx < m_RRPatternNum; patternIdx++) {
		if (m_RRScheduleInfoTable[patternIdx] == nullptr)continue;
		systemEventVec[m_RRScheduleInfoTable[patternIdx]->eventId].sendDelay++;
	}
}

//<UNDONE>
void cRSU::RRProcessTransimit2(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec) {
	for (int patternIdx = 0; patternIdx < m_RRPatternNum; patternIdx++) {
		sRRScheduleInfo* &info = m_RRScheduleInfoTable[patternIdx];
		if (info == nullptr) continue;
		//���¸��¼�����־
		systemEventVec[info->eventId].addEventLog(TTI, 10, m_RSUId, -1, patternIdx);

		//WRONG
		if (TTI == get<1>(info->occupiedInterval)) {
			//���¸��¼�����־
			systemEventVec[info->eventId].addEventLog(TTI, 0, m_RSUId, -1, patternIdx);

			//��¼TTI��־
			RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 0, info->eventId, m_RSUId, patternIdx);

			//�ͷŵ�����Ϣ������ڴ���Դ
			delete info;
			info = nullptr;
			deleteCount++;
		}
	}
}










