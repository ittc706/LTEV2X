#include <sstream>
#include <iomanip>
#include "eNB.h"
#include "Config.h"
using namespace std;


void ceNB::initialize(seNBConfigure &t_eNBConfigure){
	m_wRoadID = t_eNBConfigure.wRoadID;
	m_eNBId = t_eNBConfigure.weNBID;
	m_fX = t_eNBConfigure.fX;
	m_fY = t_eNBConfigure.fY;
	m_fAbsX = t_eNBConfigure.fAbsX;
	m_fAbsY = t_eNBConfigure.fAbsY;
	printf("»ùÕ¾£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n", m_fAbsX, m_fAbsY);
}


string ceNB::toString(int n) {
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