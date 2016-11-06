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
		packageNum = gc_PeriodMessagePackageNum;
		bitNumPerPackage = vector<int>(packageNum);
		for (int i = 0; i < packageNum; i++)
			bitNumPerPackage[i] = gc_PeriodMessageBitNumPerPackage[i];
		currentPackageIdx = 0;
		remainBitNum = bitNumPerPackage[0];
		break;
	case EMERGENCY:
		packageNum = gc_EmergencyMessagePackageNum;
		bitNumPerPackage = vector<int>(packageNum);
		for (int i = 0; i < packageNum; i++)
			bitNumPerPackage[i] = gc_EmergencyMessageBitNumPerPackage[i];
		currentPackageIdx = 0;
		remainBitNum = bitNumPerPackage[0];
		break;
	case DATA:
		packageNum = gc_DataMessagePackageNum;
		bitNumPerPackage = vector<int>(packageNum);
		for (int i = 0; i < packageNum; i++)
			bitNumPerPackage[i] = gc_DataMessageBitNumPerPackage[i];
		currentPackageIdx = 0;
		remainBitNum = bitNumPerPackage[0];
		break;
	}
	isDone = false;
}


void Message::reset() { 
	currentPackageIdx = 0;
	remainBitNum = bitNumPerPackage[0];
	isDone = false;
}


int Message::transimit(int transimitMaxBitNum) {
	if (transimitMaxBitNum >= remainBitNum) {//当前package传输完毕
		int temp = remainBitNum;
		if (++currentPackageIdx == packageNum) {//若当前package是最后一个package，那么说明传输成功
			remainBitNum = 0;
			isDone = true;
		}
		else
			remainBitNum = bitNumPerPackage[currentPackageIdx];
		return temp;
	}
	else {//当前package尚未传输完毕，只需更新remainBitNum
		remainBitNum -= transimitMaxBitNum;
		return transimitMaxBitNum;
	}
}


bool Message::isFinished() {
	return isDone;
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
	ss << "[ byteNum = { ";
	for(int bitNum: bitNumPerPackage)
		ss << left << setw(3) << bitNum << ", ";
	ss << "} , MessageType = " << s << " ]";
	return ss.str();
}


Event::Event(int t_VeUEId, int t_TTI, MessageType t_MessageType) :
	isSuccessded(false), 
	propagationDelay(0), 
	sendDelay(0), 
	processingDelay(0), 
	queuingDelay(0), 
	conflictNum(0),
	message(Message(t_MessageType)) {
	this->VeUEId = t_VeUEId;
	this->TTI = t_TTI;
}


string Event::toString() {
	ostringstream ss;
	ss << "{ EventId = " << left << setw(3) << eventId;
	ss << " , VeUEId = " << left << setw(3) << VeUEId;
	ss << "] ， Message = " << message.toString() << " }";
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


void Event::addEventLog(int t_TTI, EventLogType t_EventLogType, int t_RSUId, int t_ClusterIdx, int t_PatternIdx,string t_Description) {
	stringstream ss;
	switch (t_EventLogType) {
	case SUCCEED:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - Transimit Succeed At: RSU[" << t_RSUId << "] - ClusterIdx[" << t_ClusterIdx << "] - PatternIdx[" << t_PatternIdx << "] }";
		break;
	case EVENT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: EventList - To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_RSUId << "]'s ScheduleTable[" << t_ClusterIdx << "][" << t_PatternIdx << "] - To: SwitchList }";
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_RSUId << "]'s ScheduleTable[" << t_ClusterIdx << "][" << t_PatternIdx << "] - To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		break;
	case WAIT_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_RSUId << "]'s WaitEventIdList - To: SwitchList }";
		break;
	case WAIT_TO_ADMIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_RSUId << "]'s WaitEventIdList - To: RSU[" << t_RSUId << "]'s AdmitEventIdList }";
		break;
	case WAIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_RSUId << "]'s WaitEventIdList - To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		break;
	case SWITCH_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: SwitchList - To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		break;
	case TRANSIMIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_RSUId << "]'s TransmitScheduleInfoList - To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		conflictNum++;
		break;
	case IS_TRANSIMITTING:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - Transimit At: RSU[" << t_RSUId << "] - Cluster[" << t_ClusterIdx << "] - Pattern[" << t_PatternIdx << "] }";
		break;
	case ADMIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_RSUId << "]'s AdmitEventIdList - To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		break;
	}
	logTrackList.push_back(ss.str());
}









