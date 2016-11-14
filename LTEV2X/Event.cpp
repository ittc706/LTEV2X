#include<iomanip>
#include"Event.h"
#include"Enumeration.h"
#include"Global.h"
#include"System.h"

using namespace std;

int Event::s_EventCount = 0;

std::pair<int, std::vector<int>> Message::constMemberInitialize(MessageType t_MessageType) {
	pair<int, std::vector<int>> res;
	switch (t_MessageType) {
	case PERIOD:
		res.first = gc_PeriodMessagePackageNum;
		res.second = vector<int>(gc_PeriodMessagePackageNum);
		for (int i = 0; i < gc_PeriodMessagePackageNum; i++)
			res.second[i] = gc_PeriodMessageBitNumPerPackage[i];
		return res;
	case EMERGENCY:
		res.first = gc_EmergencyMessagePackageNum;
		res.second = vector<int>(gc_PeriodMessagePackageNum);
		for (int i = 0; i < gc_PeriodMessagePackageNum; i++)
			res.second[i] = gc_EmergencyMessageBitNumPerPackage[i];
		return res;
	case DATA:
		res.first = gc_DataMessagePackageNum;
		res.second = vector<int>(gc_PeriodMessagePackageNum);
		for (int i = 0; i < gc_PeriodMessagePackageNum; i++)
			res.second[i] = gc_DataMessageBitNumPerPackage[i];
		return res;
	default:
		throw Exp("Wrong MessageType");
	}
}

Message::Message(MessageType t_MessageType) :
	messageType(t_MessageType), 
	packageNum(constMemberInitialize(t_MessageType).first), 
	bitNumPerPackage(constMemberInitialize(t_MessageType).second){

	currentPackageIdx = 0;
	remainBitNum = bitNumPerPackage[0];
	isDone = false;
	packageIsLoss.assign(packageNum, false);
}


void Message::reset() { 
	currentPackageIdx = 0;
	remainBitNum = bitNumPerPackage[0];
	isDone = false;
	packageIsLoss.assign(packageNum, false);
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


void Event::addEventLog(int t_TTI, EventLogType t_EventLogType, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx,string t_Description) {
	stringstream ss;
	switch (t_EventLogType) {
	case TRANSIMITTING:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - Transimit At: RSU[" << t_FromRSUId << "] - Cluster[" << t_FromClusterIdx << "] - Pattern[" << t_FromPatternIdx << "] }";
		break;
	case SUCCEED:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - Transimit Succeed At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "] }";
		break;
	case EVENT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: EventList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: SwitchList }";
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case WAIT_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: SwitchList }";
		break;
	case WAIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case SWITCH_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: SwitchList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case TRANSIMIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s TransmitScheduleInfoList[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		conflictNum++;
		break;
	}
	logTrackList.push_back(ss.str());
}









