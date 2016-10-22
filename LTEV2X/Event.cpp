#include<iomanip>
#include"Event.h"
#include"Enumeration.h"
#include"Global.h"
#include"System.h"

using namespace std;

int Event::s_EventCount = 0;

Message::Message(MessageType messageType) {
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
	remainBitNum = bitNum;
}

string Message::toString() {
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


Event::Event(int VeUEId, int TTI, MessageType messageType) :
	isSuccessded(false), 
	propagationDelay(0), 
	sendDelay(0), 
	processingDelay(0), 
	queuingDelay(0), 
	conflictNum(0),
	message(Message(messageType)) {
	this->VeUEId = VeUEId;
	this->TTI = TTI;
}


string Event::toString() {
	ostringstream ss;
	ss << "{ EventId = " << left << setw(3) << eventId;
	ss << " , VeUEId = " << left << setw(3) << VeUEId;
	ss << "] £¬ Message = " << message.toString() << " }";
	return ss.str();
}


string Event::toLogString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	for (string log : logTrackList)
		ss << indent << log << endl;
	return ss.str();
}


void Event::addEventLog(int TTI, EventLogType type, int RSUId, int clusterIdx, int patternIdx,std::string description) {
	stringstream ss;
	switch (type) {
	case SUCCEED:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - Transimit Succeed At: RSU[" << RSUId << "] - ClusterIdx[" << clusterIdx << "] - PatternIdx[" << patternIdx << "] }";
		break;
	case EVENT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - From: EventList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] - To: SwitchList }";
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case WAIT_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - From: RSU[" << RSUId << "]'s WaitEventIdList - To: SwitchList }";
		break;
	case WAIT_TO_ADMIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - From: RSU[" << RSUId << "]'s WaitEventIdList - To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		break;
	case SWITCH_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - From: SwitchList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case TRANSIMIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - From: RSU[" << RSUId << "]'s TransmitScheduleInfoList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		conflictNum++;
		break;
	case IS_TRANSIMITTING:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - Transimit At: RSU[" << RSUId << "] - Cluster[" << clusterIdx << "] - Pattern[" << patternIdx << "] }";
		break;
	case ADMIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Description : <" << left << setw(10) << description + ">" << " - From: RSU[" << RSUId << "]'s AdmitEventIdList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	}
	logTrackList.push_back(ss.str());
}









