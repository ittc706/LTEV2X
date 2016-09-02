#include<iomanip>
#include"Traffic.h"
#include"Enumeration.h"
#include"Global.h"
#include"System.h"

using namespace std;

int sEvent::s_EventCount = 0;

sMessage::sMessage(eMessageType messageType) {
	this->messageType = messageType;
	switch (messageType) {
	case PERIOD:
		bitNum = gc_PeriodMessageBitNum;
		break;
	case EMERGENCY:
		bitNum = gc_EmergencyMessageBitNum;
		break;
	case DATA:
		bitNum = gc_DataMessageBitNum;
		break;
	}
}

string sMessage::toString() {
	string s;
	switch (messageType) {
	case PERIOD:
		s = "PERIOD";
		break;
	case EMERGENCY:
		s = "EMERGENCY";
		break;
	case DATA:
		s = "DATA";
		break;
	}
	ostringstream ss;
	ss << "[ byteNum = " << left << setw(3) << bitNum;
	ss << " , MessageType = " << s << " ]";
	return ss.str();
}


sEvent::sEvent(int VeUEId, int TTI, eMessageType messageType) :isSuccessded(false), propagationDelay(0), sendDelay(0), processingDelay(0), queuingDelay(0), conflictNum(0) {
	this->VeUEId = VeUEId;
	this->TTI = TTI;
	message = sMessage(messageType);
}


string sEvent::toString() {
	ostringstream ss;
	ss << "{ EventId = " << left << setw(3) << eventId;
	ss << " , VeUEId = " << left << setw(3) << VeUEId;
	ss << "] �� Message = " << message.toString() << " }";
	return ss.str();
}


string sEvent::toLogString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	for (string log : logTrackList)
		ss << indent << log << endl;
	return ss.str();
}


void sEvent::addEventLog(int TTI, eEventLogType type, int RSUId, int clusterIdx, int patternIdx) {
	stringstream ss;
	switch (type) {
	case SUCCEED:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Transimit Succeed At: RSU[" << RSUId << "] - ClusterIdx[" << clusterIdx << "] - PatternIdx[" << patternIdx << "] }";
		break;
	case EVENT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: EventList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] - To: SwitchList }";
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case WAIT_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s WaitEventIdList - To: SwitchList }";
		break;
	case WAIT_TO_ADMIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s WaitEventIdList - To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		break;
	case SWITCH_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: SwitchList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case TRANSIMIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s TransmitScheduleInfoList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		conflictNum++;
		break;
	case IS_TRANSIMITTING:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Transimit At: RSU[" << RSUId << "] - Cluster[" << clusterIdx << "] - Pattern[" << patternIdx << "] }";
		break;
	case ADMIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s AdmitEventIdList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	}
	logTrackList.push_back(ss.str());
}


void cSystem::buildEventList() {
	/*��ʱ��˳���¼���Id��ʱ����أ�IdԽС���¼�������ʱ��ԽС�����¼�����*/

	srand((unsigned)time(NULL));//iomanip
	//�������ɸ����������������¼�����ʼʱ��
	vector<list<int>> startTTIVec(m_Config.periodicEventNTTI, list<int>());
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		int startTTI = rand() % m_Config.periodicEventNTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	//���ɽ����¼��ķ���ʱ�̣�ÿ��ʱ��۴�Ÿ�ʱ�̷��������¼��ĳ���
	m_VeUEEmergencyNum = vector<int>(m_Config.VeUENum,0);//��ʼ��ͳ����
	int countEmergency = 0;
	vector<list<int>> emergencyEventTriggerTTI(m_NTTI);
	for (int VeUEId = 0;VeUEId < m_Config.VeUENum;VeUEId++) {
		//��������ÿ�������Ľ����¼�����ʱ��
		double T = 0;
		while (T < m_NTTI) {
			double u = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
			if (u == 0) break;
			T = T - (1 / m_Config.emergencyLamda)*log(u);
			int IntegerT = static_cast<int>(T);
			if (IntegerT < m_NTTI) {
				emergencyEventTriggerTTI[IntegerT].push_back(VeUEId);
				++m_VeUEEmergencyNum[VeUEId];
				++countEmergency;
			}
		}
	}

	cout << "countEmergency: " << countEmergency << endl;


	//����startTTIVec�������PERIOD�¼��������в�����Ӳ��ɷֲ��Ľ����¼�

	int CTTI = 0;
	while (CTTI < m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			//ѹ������¼�
			if (CTTI + TTIOffset < m_NTTI) {
				list<int> &emergencyList = emergencyEventTriggerTTI[CTTI + TTIOffset];
				for (int VeUEId : emergencyList) {
					/*-----------------------ATTENTION-----------------------
					* ����������sEvent�Ķ���Ȼ����ѹ��m_EventVec
					* ����Vector<T>.push_back��ѹ�봫�����ĸ���Ʒ����˻����sEvent�Ŀ������캯��
					* sEventĬ�ϵĿ������캯���ḳֵid��Ա������ǰ�ȫ�ģ�
					*sEvent����Զ��忽�����캯���������ڹ��캯���ĳ�ʼ�����ֿ���id��Ա
					*-----------------------ATTENTION-----------------------*/
					sEvent evt= sEvent(VeUEId, CTTI + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
					--countEmergency;
				}
			}

			//�����������¼�
			if (CTTI + TTIOffset < m_NTTI) {
				list<int> &periodList = startTTIVec[TTIOffset];
				for (int VeUEId : periodList) {
					sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, PERIOD);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
				}
			}
		}
		CTTI += m_Config.periodicEventNTTI;
	}
}


void cSystem::processStatistics() {
	//ͳ�Ƶȴ�ʱ��
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++)
		if (m_EventVec[eventId].isSuccessded)
			g_OutDelayStatistics << m_EventVec[eventId].queuingDelay << " ";
	g_OutDelayStatistics << endl;//����ܹؼ�����������������ˢ�µ�����

	//ͳ�ƴ���ʱ��
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++)
		if (m_EventVec[eventId].isSuccessded)
			g_OutDelayStatistics << m_EventVec[eventId].sendDelay << " ";
	g_OutDelayStatistics << endl;//����ܹؼ�����������������ˢ�µ�����

	//ͳ�ƽ����¼��ֲ����
	for (int num : m_VeUEEmergencyNum)
		g_OutEmergencyPossion << num << " ";
	g_OutEmergencyPossion << endl;//����ܹؼ�����������������ˢ�µ�����

	//ͳ�Ƴ�ͻ���
	for (sEvent &event : m_EventVec)
		g_OutConflictNum << event.conflictNum << " ";
	g_OutConflictNum << endl;
}







