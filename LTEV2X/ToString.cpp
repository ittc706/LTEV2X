#include"VUE.h"
#include"eNB.h"
#include"RSU.h"
#include<iomanip>
#include<sstream>
using namespace std;

string sEvent::toString() {
	ostringstream ss;
	ss << "{ EventId = " << left << setw(3) << eventId;
	ss << " , VeUEId = " << left << setw(3) << VeUEId;
	ss << "] ， Message = " << message.toString() << " }";
	return ss.str();
}


string sEvent::toLogString(int n) {
	string indent;
	for (int i = 0;i < n;i++)
		indent.append("    ");

	ostringstream ss;
	for (string log : logTrackList)
		ss << indent << log << endl;
	return ss.str();
}


string sMessage::toString() {
	ostringstream ss;
	ss << "[ byteNum = " << left << setw(3) << byteNum;
	ss << " , DRA_ONTTI = " << left << setw(3) << DRA_ONTTIPerFB;
	ss << " , MessageType = " << (messageType == PERIOD ? "PEROID" : "ELSE") << " ]";
	return ss.str();
}


string cVeUE::toString(int n) {
	string indent;
	for (int i = 0;i < n;i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent<< "{ VeUEId = " << left << setw(3) << m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << m_ClusterIdx << " }";
	return ss.str();
}

string ceNB::toString(int n) {
	string indent;
	for (int i = 0;i < n;i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "eNB[" << m_eNBId<<"] :" << endl;
	ss << indent << "{" << endl;
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	int cnt = 0;
	for (int VeUEId : m_VeUEIdList) {
		if (cnt % 10 == 0)
			ss << indent << "        [ ";
		ss << left << setw(3) << VeUEId << " , ";
		if (cnt % 10 == 9)
			ss << " ]" << endl;
		cnt++;
	}
	if (cnt != 0 && cnt % 10 != 0)
		ss << " ]" << endl;

	ss << indent << "    " << "{" << endl;

	ss << indent << "    " << "RSUIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	cnt = 0;
	for (int RSUId : m_RSUIdList) {
		if (cnt % 10 == 0)
			ss << indent << "        [ ";
		ss << left << setw(3) << RSUId << " , ";
		if (cnt % 10 == 9)
			ss << " ]" << endl;
		cnt++;
	}
	if (cnt != 0 && cnt % 10 != 0)
		ss << " ]" << endl;

	ss << indent << "    " << "{" << endl;
	ss << indent << "}" << endl;
	return ss.str();
}

string cRSU::toString(int n) {
	string indent;
	for (int i = 0;i < n;i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << m_RSUId<<"] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		ss << indent << "        " << "Cluster[" <<clusterIdx<<"] :"<< endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : m_DRAClusterVeUEIdList[clusterIdx]) {
			if (cnt % 10 == 0)
				ss << indent << "            [ ";
			ss << left << setw(3) << RSUId << " , ";
			if (cnt % 10 == 9)
				ss << " ]" << endl;
			cnt++;
		}
		if (cnt != 0 && cnt % 10 != 0)
			ss << " ]" << endl;
		ss << indent << "        " << "}" << endl;
	}
	ss << indent << "    " << "}" << endl;

	//开始打印clusterInfo
	ss << indent << "    " << "clusterInfo :" << endl;
	ss << indent << "    " << "{" << endl;

	ss << indent << "        ";
	for (const tuple<int, int, int>&t : m_DRAClusterTDRInfo)
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << endl;
	ss << indent << "    " << "}" << endl;


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}


string sDRAScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss<<"[ eventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx<< " ] ";
	return ss.str();
}


std::string sDRAScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0;i < n;i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ ";
	ss << "[ eventId = " << left << setw(3) << eventId << " , VeUEID = " << left << setw(3) << VeUEId << " ]";
	ss << " : occupy Interval = { ";
	for (tuple<int, int> t : occupiedIntervalList) {
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] , ";
	}
	ss << "} }";
	return ss.str();
}