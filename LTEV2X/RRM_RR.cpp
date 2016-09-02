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


#include <iomanip>
#include <sstream>
#include "RRM_RR.h"

using namespace std;

std::string sRRScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ eventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


std::string sRRScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ ";
	ss << "[ eventId = " << left << setw(3) << eventId << " , VeUEId = " << left << setw(3) << VeUEId << " ]";
	ss << " : occupy Interval = { ";
	ss << "[ " << get<0>(occupiedInterval) << " , " << get<1>(occupiedInterval) << " ] , ";
	ss << "} }";
	return ss.str();
}

RSUAdapterRR::RSUAdapterRR(cRSU& _RSU):m_HoldObj(_RSU){
	m_RRScheduleInfoTable = vector<sRRScheduleInfo*>(m_RRPatternNum);
}


RRM_RR::RRM_RR(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList) {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAdapterVec.push_back(VeUEAdapterRR(m_VeUEAry[VeUEId]));
	}
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAdapterVec.push_back(RSUAdapterRR(m_RSUAry[RSUId]));
	}
}



void RRM_RR::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//����ǰ������
	RRInformationClean();

	//������������
	RRUpdateAdmitEventIdList(clusterFlag);

	//��ʼ���ε���
	RRProcessTransimit1();
	RRWriteScheduleInfo(g_OutRRScheduleInfo);

	RRDelaystatistics();
	RRProcessTransimit2();
}



void RRM_RR::RRInformationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		_RSUAdapterRR.m_RRAdmitEventIdList.clear();
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
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		for (int eventId : m_EventTTIList[m_TTI]) {
			sEvent event = m_EventVec[eventId];
			int VeUEId = event.VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterRR.m_HoldObj.m_RSUId) {//��ǰ�¼���Ӧ��VeUE���ڵ�ǰRSU�У���������
				continue;
			}
			else {//��ǰ�¼���Ӧ��VeUE�ڵ�ǰRSU��
				if (m_EventVec[eventId].message.messageType == EMERGENCY) {//���ǽ����¼�
					/*  EMERGENCY  */
					_RSUAdapterRR.RRPushToWaitEventIdList(eventId, EMERGENCY);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 1, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
					/*  EMERGENCY  */
				}
				else {//һ�����¼������������¼�����������ҵ���¼�
					  //�����¼�ѹ��ȴ�����
					_RSUAdapterRR.RRPushToWaitEventIdList(eventId, PERIOD);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 1, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
				}
			}
		}
	}
}


void RRM_RR::RRProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		//��ʼ���� m_RRWaitEventIdList
		list<int>::iterator it = _RSUAdapterRR.m_RRWaitEventIdList.begin();
		while (it != _RSUAdapterRR.m_RRWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			eMessageType messageType = m_EventVec[eventId].message.messageType;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterRR.m_HoldObj.m_RSUId) {//��VeUE�Ѿ����ڸ�RSU��Χ��
				_RSUAdapterRR.RRPushToSwitchEventIdList(eventId, m_RRSwitchEventIdList);//������ӵ�System�����RSU�л�������
				it = _RSUAdapterRR.m_RRWaitEventIdList.erase(it);//����ӵȴ�������ɾ��

				if (messageType == EMERGENCY) {
					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 4, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
				}
				else if (messageType == PERIOD) {
					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 4, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
				}
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
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		list<int>::iterator it = m_RRSwitchEventIdList.begin();
		while (it != m_RRSwitchEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterRR.m_HoldObj.m_RSUId) {//���л������е��¼���Ӧ��VeUE�������ڵ�ǰ�أ���������
				it++;
				continue;
			}
			else {//���л������е�VeUE�����ڵ�RSU
				  /*  EMERGENCY  */
				if (m_EventVec[eventId].message.messageType == EMERGENCY) {//���ڽ����¼�
					_RSUAdapterRR.RRPushToWaitEventIdList(eventId, EMERGENCY);
					it = m_RRSwitchEventIdList.erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 5, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
				}
				/*  EMERGENCY  */

				else {//�ǽ����¼�
					_RSUAdapterRR.RRPushToWaitEventIdList(eventId, PERIOD);
					it = m_RRSwitchEventIdList.erase(it);

					//���¸��¼�����־
					m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

					//��¼TTI��־
					RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 5, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
				}
			}
		}
	}
}


void RRM_RR::RRProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		int count = 0;
		list<int>::iterator it = _RSUAdapterRR.m_RRWaitEventIdList.begin();
		while (it != _RSUAdapterRR.m_RRWaitEventIdList.end() && count < _RSUAdapterRR.m_RRPatternNum) {
			int eventId = *it;
			eMessageType messageType = m_EventVec[eventId].message.messageType;
			_RSUAdapterRR.RRPushToAdmitEventIdList(eventId);
			if (messageType == EMERGENCY) {
				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 2, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
			}
			else if (messageType == PERIOD) {
				//���¸��¼�����־
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 2, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
			}
			it = _RSUAdapterRR.m_RRWaitEventIdList.erase(it);
		}
	}
}


void RRM_RR::RRProcessTransimit1() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		for (int patternIdx = 0; patternIdx < static_cast<int>(_RSUAdapterRR.m_RRAdmitEventIdList.size()); patternIdx++) {
			int eventId = _RSUAdapterRR.m_RRAdmitEventIdList[patternIdx];
			int VeUEId = m_EventVec[eventId].VeUEId;
			int occupiedTTI = (int)ceil((double)m_EventVec[eventId].message.bitNum / (double)gc_BitNumPerRB / (double)_RSUAdapterRR.m_RRNumRBPerPattern);
			eMessageType messageType = m_EventVec[eventId].message.messageType;
			tuple<int, int> scheduleInterval(m_TTI, m_TTI + occupiedTTI - 1);
			_RSUAdapterRR.m_RRScheduleInfoTable[patternIdx] = new sRRScheduleInfo(eventId, messageType, VeUEId, _RSUAdapterRR.m_HoldObj.m_RSUId, patternIdx, scheduleInterval);
			newCount++;
		}
	}
}


void RRM_RR::RRWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];
		
		out << "    RSU[" << _RSUAdapterRR.m_HoldObj.m_RSUId << "] :" << endl;
		out << "    {" << endl;
		for (int patternIdx = 0; patternIdx < _RSUAdapterRR.m_RRPatternNum; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
			sRRScheduleInfo* info = _RSUAdapterRR.m_RRScheduleInfoTable[patternIdx];
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



void RRM_RR::RRDelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		//����ȴ�����
		for (int eventId : _RSUAdapterRR.m_RRWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//����˿����ڽ�Ҫ����ĵ��ȱ�
		for (int patternIdx = 0; patternIdx < _RSUAdapterRR.m_RRPatternNum; patternIdx++) {
			if (_RSUAdapterRR.m_RRScheduleInfoTable[patternIdx] == nullptr)continue;
			m_EventVec[_RSUAdapterRR.m_RRScheduleInfoTable[patternIdx]->eventId].sendDelay++;
		}
	}
}


void RRM_RR::RRProcessTransimit2() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		for (int patternIdx = 0; patternIdx < _RSUAdapterRR.m_RRPatternNum; patternIdx++) {
			sRRScheduleInfo* &info = _RSUAdapterRR.m_RRScheduleInfoTable[patternIdx];
			if (info == nullptr) continue;
			//���¸��¼�����־
			m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, patternIdx);

			//WRONG
			if (m_TTI == get<1>(info->occupiedInterval)) {
				//���¸��¼�����־
				m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, patternIdx);

				//��¼TTI��־
				RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 0, info->eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, patternIdx);

				//�ͷŵ�����Ϣ������ڴ���Դ
				delete info;
				info = nullptr;
				deleteCount++;
			}
		}
	}
}