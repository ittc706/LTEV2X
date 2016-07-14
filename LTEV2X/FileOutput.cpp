#include<fstream>
#include<iomanip>
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
		out << "Event[" << left << setw(3) << eventId << "]  ";
		out << "VeUE[" << m_EventVec[eventId].VeUEId << "]" << endl;
		out << "{" << endl;
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
		for (sDRAScheduleInfo* info : m_DRATransimitEventIdList[patternIdx]) {
			out << info->toScheduleString(3) << endl;
		}
	}
	out << "    }" << endl;

}

