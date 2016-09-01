#include<fstream>
#include<iomanip>
#include<sstream>
#include"System.h"
using namespace std;



void cSystem::writeEventListInfo(ofstream &out) {
	for (int i = 0; i < m_NTTI; i++) {
		out << "[ TTI = " << left << setw(3) << i << " ]" << endl;
		out << "{" << endl;
		for (int eventId : m_EventTTIList[i]) {
			sEvent& e = m_EventVec[eventId];
			out << "    " << e.toString() << endl;
		}
		out << "}\n\n" << endl;
	}
}


void cSystem::writeEventLogInfo(std::ofstream &out) {
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++) {
		string s;
		switch (m_EventVec[eventId].message.messageType) {
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
		out << "Event[" << eventId << "]";
		out << "{" << endl;
		out << "    " << "VeUEId = " << m_EventVec[eventId].VeUEId << endl;
		out << "    " << "MessageType = " << s << endl;
		out << "    " << "sendDelay = " << m_EventVec[eventId].sendDelay << "(TTI)" << endl;
		out << "    " << "queuingDelay = " << m_EventVec[eventId].queuingDelay << "(TTI)" << endl;
		out << m_EventVec[eventId].toLogString(1);
		out << "}" << endl;
	}
}


void cSystem::writeVeUELocationUpdateLogInfo(std::ofstream &out) {
	for (int VeUEId = 0;VeUEId < m_Config.VeUENum;VeUEId++) {
		out << "VeUE[ " << left << setw(3) << VeUEId << "]" << endl;
		out << "{" << endl;
		for (const tuple<int, int> &t : m_VeUEAry[VeUEId].m_LocationUpdateLogInfoList)
			out << "    " << "[ RSUId = " << left << setw(2) << get<0>(t) << " , ClusterIdx = " << get<1>(t) << " ]" << endl;
		out << "}" << endl;
	}
}

//<UNDONE>
void cRSU::RRWriteScheduleInfo(std::ofstream& out, int TTI) {
	out << "    RSU[" << m_RSUId << "] :" << endl;
	out << "    {" << endl;
	for (int patternIdx = 0; patternIdx < m_RRPatternNum; patternIdx++) {
		out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
		sRRScheduleInfo* info = m_RRScheduleInfoTable[patternIdx];
		if (info == nullptr) continue;
		out << info->toScheduleString(3) << endl;
	}
	out << "    }" << endl;
}


//<UNDONE>
void cRSU::RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx) {
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
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << patternIdx << "] ; To: RSU[" << m_RSUId << "]'s WaitEventIdList }";
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


void sEvent::addEventLog(int TTI, eEventLogType type,int RSUId,int clusterIdx,int patternIdx) {
	stringstream ss;
	switch (type) {
	case 0:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Transimit Succeed At: RSU[" << RSUId << "] - ClusterIdx[" << clusterIdx << "] - PatternIdx[" << patternIdx << "] }";
		break;
	case 1:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: EventList - To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		break;
	case 2:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: EventList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case 3:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] - To: SwitchList }";
		break;
	case 4:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case 5:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s WaitEventIdList - To: SwitchList }";
		break;
	case 6:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s WaitEventIdList - To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		break;
	case 7:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: SwitchList - To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		break;
	case 8:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: SwitchList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case 9:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s TransmitScheduleInfoList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		conflictNum++;
		break;
	case 10:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Transimit At: RSU[" << RSUId << "] - Cluster[" << clusterIdx << "] - Pattern[" << patternIdx << "] }";
		break;
	case 11:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s AdmitEventIdList - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case 21:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: EventList - To: RSU[" << RSUId << "]'s EmergencyAdmitEventIdList }";
		break;
	case 23:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s EmergencyScheduleTable[" << clusterIdx << "][" << patternIdx << "] - To: SwitchList }";
		break;
	case 25:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s EmergencyWaitEventIdList - To: SwitchList }";
		break;
	case 26:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s EmergencyWaitEventIdList - To: RSU[" << RSUId << "]'s EmergencyAdmitEventIdList }";
		break;
	case 27:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: SwitchList - To: RSU[" << RSUId << "]'s EmergencyAdmitEventIdList }";
		break;
	case 29:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s EmergencyTransimitScheduleInfoList - To: RSU[" << RSUId << "]'s EmergencyWaitEventIdList }";
		conflictNum++;
		break;
	case 30:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Transimit At: RSU[" << RSUId << "] - Pattern[" << patternIdx << "] }";
		break;
	case 31:
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s EmergencyAdmitEventIdList - To: RSU[" << RSUId << "]'s EmergencyWaitEventIdList }";
		break;
	}
	logTrackList.push_back(ss.str());
}
