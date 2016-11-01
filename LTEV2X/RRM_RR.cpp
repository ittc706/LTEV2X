/*
* =====================================================================================
*
*       Filename:  RRM_RR.cpp
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


#include<iomanip>
#include<sstream>
#include<iostream>
#include"RRM_RR.h"

using namespace std;


RRM_RR::RRM_RR(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList, systemTTIRSUThroughput) {
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
		_RSU.m_RRM_RR->m_RRAdmitEventIdList.clear();
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
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int eventId : m_EventTTIList[m_TTI]) {
			Event event = m_EventVec[eventId];
			int VeUEId = event.VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//��ǰ�¼���Ӧ��VeUE���ڵ�ǰRSU�У���������
				continue;
			}
			else {//��ǰ�¼���Ӧ��VeUE�ڵ�ǰRSU��
				//���¼�ѹ��ȴ�����
				_RSU.m_RRM_RR->RRPushToWaitEventIdList(eventId, m_EventVec[eventId].message.messageType);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Trigger");

				//��¼TTI��־
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1);
			}
		}
	}
}


void RRM_RR::RRProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		//��ʼ���� m_RRWaitEventIdList
		list<int>::iterator it = _RSU.m_RRM_RR->m_RRWaitEventIdList.begin();
		while (it != _RSU.m_RRM_RR->m_RRWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			MessageType messageType = m_EventVec[eventId].message.messageType;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
				//��ʣ�����bit����
				m_EventVec[eventId].message.reset();

				//������ӵ�System�����RSU�л�������
				_RSU.m_RRM_RR->RRPushToSwitchEventIdList(eventId, m_RRSwitchEventIdList);
				
				//����ӵȴ�������ɾ��
				it = _RSU.m_RRM_RR->m_RRWaitEventIdList.erase(it);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

				//��¼TTI��־
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1);
			}
			else {//��Ȼ���ڵ�ǰRSU��Χ��
				it++;
				continue; //�������ڵ�ǰRSU�ĵȴ�����
			}
		}
	}
}


void RRM_RR::RRProcessSwitchListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		list<int>::iterator it = m_RRSwitchEventIdList.begin();
		while (it != m_RRSwitchEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//���л������е��¼���Ӧ��VeUE�������ڵ�ǰ�أ���������
				it++;
				continue;
			}
			else {//���л������е�VeUE�����ڵ�RSU
				//ѹ��ȴ�����
				_RSU.m_RRM_RR->RRPushToWaitEventIdList(eventId, m_EventVec[eventId].message.messageType);

				//��Switch������ɾ��
				it = m_RRSwitchEventIdList.erase(it);

				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

				//��¼TTI��־
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1);
			}
		}
	}
}


void RRM_RR::RRProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		int count = 0;
		list<int>::iterator it = _RSU.m_RRM_RR->m_RRWaitEventIdList.begin();
		while (it != _RSU.m_RRM_RR->m_RRWaitEventIdList.end() && count++ < gc_RRPatternNum) {
			int eventId = *it;
			MessageType messageType = m_EventVec[eventId].message.messageType;
			//ѹ���������
			_RSU.m_RRM_RR->RRPushToAdmitEventIdList(eventId);
			
			//���¸��¼�����־
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Accept");

			//��¼TTI��־
			RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTAT->m_RSUId, -1);

			it = _RSU.m_RRM_RR->m_RRWaitEventIdList.erase(it);
		}
		if (_RSU.m_RRM_RR->m_RRAdmitEventIdList.size() > gc_RRPatternNum)
			throw Exp("RRProcessWaitEventIdList()");
	}
}


void RRM_RR::RRRoundRobin() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int patternIdx = 0; patternIdx < static_cast<int>(_RSU.m_RRM_RR->m_RRAdmitEventIdList.size()); patternIdx++) {
			int eventId = _RSU.m_RRM_RR->m_RRAdmitEventIdList[patternIdx];
			int VeUEId = m_EventVec[eventId].VeUEId;
			MessageType messageType = m_EventVec[eventId].message.messageType;
			_RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx] = new RSU::RRM_RR::RRScheduleInfo(eventId, messageType, VeUEId, _RSU.m_GTAT->m_RSUId, patternIdx);
			m_NewCount++;
		}
	}
}


void RRM_RR::RRDelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		//����ȴ�����
		for (int eventId : _RSU.m_RRM_RR->m_RRWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//����˿����ڽ�Ҫ����ĵ��ȱ�
		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			if (_RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx] == nullptr)continue;
			m_EventVec[_RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx]->eventId].sendDelay++;
		}
	}
}


void RRM_RR::RRTransimitPreparation() {

}


void RRM_RR::RRTransimitStart() {

}



void RRM_RR::RRWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		
		out << "    RSU[" << _RSU.m_GTAT->m_RSUId << "] :" << endl;
		out << "    {" << endl;
		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
			RSU::RRM_RR::RRScheduleInfo* info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx];
			if (info == nullptr) continue;
			out << info->toScheduleString(3) << endl;
		}
		out << "    }" << endl;
	}
	out << "}" << endl;
	out << "\n\n" << endl;
}


void RRM_RR::RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx) {
	stringstream ss;
	switch (type) {
	case 0:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]    PatternIdx[" << patternIdx << "] }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[0]Succeed";
		out << "    " << ss.str() << endl;
		break;
	case 1:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[1]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 2:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[2]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 3:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << patternIdx << "] ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[3]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 4:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[4]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 5:
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

		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			RSU::RRM_RR::RRScheduleInfo* &info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx];
			if (info == nullptr) continue;

			//UNDONE
			//������Ҫ����SINR����
			//������Ҫ����message.transimit
			//UNDONE
			////�ۼ�������
			//m_TTIRSUThroughput[m_TTI][_RSU.m_GTAT->m_RSUId] += m_EventVec[info->eventId].message.remainBitNum>gc_RRNumRBPerPattern*gc_BitNumPerRB? gc_RRNumRBPerPattern*gc_BitNumPerRB: m_EventVec[info->eventId].message.remainBitNum;

			//���¸��¼�����־
			m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Transimit");

			

			if (m_EventVec[info->eventId].message.isFinished()) {//˵���Ѿ��������
				//���ô���ɹ����
				m_EventVec[info->eventId].isSuccessded = true;

				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Succeed");

				//��¼TTI��־
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, 0, info->eventId, _RSU.m_GTAT->m_RSUId, patternIdx);

				//�ͷŵ�����Ϣ������ڴ���Դ
				delete info;
				info = nullptr;
				m_DeleteCount++;
			}
			else {//û�д�����ϣ�ת��Wait�����ȴ���һ�ε���
				_RSU.m_RRM_RR->RRPushToWaitEventIdList(info->eventId, m_EventVec[info->eventId].message.messageType);

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