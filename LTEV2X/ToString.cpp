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
	ss << "{ VeUE Id = " << left << setw(5) << m_VEId << " , " << m_Message.toString() << " }";
	return ss.str();
}

string ceNB::toString() {
	ostringstream ss;
	ss << "{ eNB Id = " << left << setw(5) << m_eNBId << " , VEIdList = [ ";
	for (int Id : m_VUESet)
		ss << left << setw(5) << Id << " , ";
	ss << " ] , RSUIdList = [ ";
	for (int Id : m_RSUSet)
		ss << left << setw(5) << Id << " , ";
	ss << " ] }";
	return ss.str();
}

string cRSU::toString() {
	ostringstream ss;
	ss << "{ RSU Id = " << m_RSUId << " , VEIdList = [ ";
	for (int Id : m_VUESet)
		ss << left << setw(5) << Id << " , ";
	ss << " ] }";
	return ss.str();
}