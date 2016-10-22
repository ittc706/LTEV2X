#include<sstream>
#include<iomanip>
#include"eNB.h"
#include"Config.h"
using namespace std;


void eNB::initializeUrban(seNBConfigure &t_eNBConfigure){
	m_RoadId = t_eNBConfigure.wRoadID;
	m_eNBId = t_eNBConfigure.weNBID;
	m_X = t_eNBConfigure.fX;
	m_Y = t_eNBConfigure.fY;
	m_AbsX = t_eNBConfigure.fAbsX;
	m_AbsY = t_eNBConfigure.fAbsY;
	printf("»ùÕ¾£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n", m_AbsX, m_AbsY);
}


void eNB::initializeHighSpeed(seNBConfigure &t_eNBConfigure)
{
	//m_wLaneID = t_eNBConfigure.wLaneID;
	m_eNBId = t_eNBConfigure.weNBID;
	//m_fX = t_eNBConfigure.fX;
	//m_fY = t_eNBConfigure.fY;
	m_AbsX = ns_GTAT_HighSpeed::c_eNBTopo[m_eNBId * 2 + 0];
	m_AbsY = ns_GTAT_HighSpeed::c_eNBTopo[m_eNBId * 2 + 1];
	printf("»ùÕ¾£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n", m_AbsX, m_AbsY);
}


string eNB::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "eNB[" << m_eNBId << "] :" << endl;
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