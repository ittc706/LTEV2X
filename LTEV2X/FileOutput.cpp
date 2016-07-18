#include<fstream>
#include<iomanip>
#include<sstream>
#include"System.h"
using namespace std;

void cSystem::writeClusterPerformInfo(ofstream &out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;

	//打印VeUE信息
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (cVeUE &_VeUE : m_VeUEVec)
		out << _VeUE.toString(2) << endl;
	out << "    }\n" << endl;

	//打印基站信息
	out << "    eNB Info: " << endl;
	out << "    {" << endl;
	for (ceNB &_eNB : m_eNBVec)
		out << _eNB.toString(2) << endl;
	out << "    }\n" << endl;

	//打印RSU信息
	out << "    RSU Info: " << endl;
	out << "    {" << endl;
	for (cRSU &_RSU : m_RSUVec)
		out << _RSU.toString(2) << endl;
	out << "    }" << endl;

	out << "}\n\n";
}




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
		out << "Event[" << eventId << "]";
		out << "{" << endl;
		out << "    " << "VeUEId = " << m_EventVec[eventId].VeUEId << endl;
		out << "    " << "sendDelay = " << m_EventVec[eventId].sendDelay << "(TTI)" << endl;
		out << "    " << "queuingDelay = " << m_EventVec[eventId].queuingDelay << "(TTI)" << endl;
		out << m_EventVec[eventId].toLogString(1);
		out << "}" << endl;
	}
}


void cSystem::writeVeUELocationUpdateLogInfo(std::ofstream &out) {
	for (int VeUEId = 0;VeUEId < static_cast<int>(m_VeUEVec.size());VeUEId++) {
		out << "VeUE[ " << left << setw(3) << VeUEId << "]" << endl;
		out << "{" << endl;
		for (const tuple<int, int> &t : m_VeUEVec[VeUEId].m_LocationUpdateLogInfoList)
			out << "    " << "[ RSUId = " << left << setw(2) << get<0>(t) << " , ClusterIdx = " << get<1>(t) << " ]" << endl;
		out << "}" << endl;
	}
}



void cRSU::DRAWriteScheduleInfo(std::ofstream& out, int TTI) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	out << "    RSU[" << m_RSUId << "] :" << endl;
	out << "    {" << endl;
	for (int patternIdx = 0;patternIdx < s_DRATotalPatternNum;patternIdx++) {
		out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (m_DRAPatternIsAvailable[clusterIdx][patternIdx] ? "Available" : "Unavailable") << endl;
		for (sDRAScheduleInfo* info : m_DRATransimitScheduleInfoList[patternIdx]) {
			out << info->toScheduleString(3) << endl;
		}
	}
	out << "    }" << endl;
}


void cRSU::DRAWriteTTILogInfo(std::ofstream& out, int TTI, int type,int eventId,int RSUId,int clusterIdx,int patternIdx) {
	stringstream ss;
	switch (type){
	case 0:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]   ClusterIdx[" << clusterIdx << "]    PatternIdx[" << patternIdx << "] }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[0]Succeed";
		out << "    " << ss.str() << endl;
		break;
	case 1:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[1]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 2:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[2]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 3:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[3]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 4:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] ; To: RSU[" << m_RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[4]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 5:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[5]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 6:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[6]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 7:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[7]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 8:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[8]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 9:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]'s AdmitEventIdLIst ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(11) << "[9]Conflict";
		out << "    " << ss.str() << endl;
		break;
	}
}


void sEvent::addEventLog(int TTI,int type,int RSUId,int clusterIdx,int patternIdx) {
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
		ss << "{ TTI: " << left << setw(3) << TTI << " - From: RSU[" << RSUId << "]'s AdmitEventIdLIst - To: RSU[" << RSUId << "]'s WaitEventIdList }";
		break;
	case 10:
		ss << "{ TTI: " << left << setw(3) << TTI << " - Transimit At: RSU[" << RSUId << "] - Cluster[" << clusterIdx << "] - Pattern[" << patternIdx << "] }";
		break;
	}
	logTrackList.push_back(ss.str());
}
