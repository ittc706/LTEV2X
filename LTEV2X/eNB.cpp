#include<sstream>
#include<iomanip>
#include"eNB.h"
#include"Config.h"
using namespace std;


void eNB::initializeUrban(eNBConfig &t_eNBConfig){
	m_RoadId = t_eNBConfig.roadId;
	m_eNBId = t_eNBConfig.eNBId;
	m_X = t_eNBConfig.X;
	m_Y = t_eNBConfig.Y;
	m_AbsX = t_eNBConfig.AbsX;
	m_AbsY = t_eNBConfig.AbsY;
	g_FileLocationInfo << toString(0);
}


void eNB::initializeHighSpeed(eNBConfig &t_eNBConfig) {
	m_eNBId = t_eNBConfig.eNBId;
	m_AbsX = ns_GTT_HighSpeed::gc_eNBTopo[m_eNBId * 2 + 0];
	m_AbsY = ns_GTT_HighSpeed::gc_eNBTopo[m_eNBId * 2 + 1];
	g_FileLocationInfo << toString(0);
}


string eNB::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "eNB[" << m_eNBId << "] : (" <<m_AbsX<<","<<m_AbsY<<")"<< endl;
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