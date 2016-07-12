#include"Message.h"
#include"VUE.h"
#include"eNB.h"
#include"RSU.h"
#include<iomanip>
#include<sstream>
using namespace std;

string sMessage::toString() {
	ostringstream ss;
	ss << "[ byteNum = " << left<<setw(5)<<byteNum << " ,  DRA_ONTTI = " << left << setw(5) << DRA_ONTTI << " ]";
	return ss.str();
}


string cVeUE::toString() {
	ostringstream ss;
	ss << "{ VeUE Id = " << left << setw(5) << m_VEId << " , RSU Id = " << left << setw(5) << m_RSUId << " , Cluster Idx = " << left << setw(5) << m_ClusterIdx << m_Message.toString()  << " }";
	return ss.str();
}

string ceNB::toString() {
	ostringstream ss;
	ss << "{ eNB Id = " << left << setw(5) << m_eNBId << " , VEIdList = [ ";
	for (int Id : m_VeUEList)
		ss << left << setw(5) << Id << " , ";
	ss << " ] , RSUIdList = [ ";
	for (int Id : m_RSUIdList)
		ss << left << setw(5) << Id << " , ";
	ss << " ] }";
	return ss.str();
}

string cRSU::toString() {
	ostringstream ss;
	ss << "        {" << endl;
	ss << left << setw(15) << "            RSU Id = " << m_RSUId << endl;
	ss << left << setw(15) << "            VEIdList = " << endl;
	ss << "            { " << endl;
	for (list<int> list : m_DRAClusterVeUEIdList) {
		ss << "                [ ";
		for (int VeUEId : list)
			ss << left << setw(4) << VeUEId << " , ";
		ss << "                ]" << endl;
	}
	ss << "            }" << endl;
	ss << left << setw(15) << "            clusterInfo = {";
	for (const tuple<int, int, int>&t : m_DRAClusterTDRInfo)
		ss << " [ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << " }" << endl;
	ss << "        }" << endl;
	return ss.str();
}


string sDRAScheduleInfo::toLogString() {
	ostringstream ss;
	ss<<"[ VeUEID = ";
	ss << left << setw(5) << VeUEId;
	ss << " , FBIdx = " << left << setw(5) << FBIdx<< " ] ";
	return ss.str();
}