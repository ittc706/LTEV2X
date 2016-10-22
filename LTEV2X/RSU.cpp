#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Event.h"
#include"Global.h"


using namespace std;


cRSU::cRSU() {
	m_GTAT = new GTAT();
	m_GTATUrban = new GTATUrban();
	m_GTATHigh = new GTATHigh();
}


void cRSU::initializeUrban(sRSUConfigure &t_RSUConfigure){
	m_GTAT->m_RSUId = t_RSUConfigure.wRSUID;
	m_GTATUrban->m_fAbsX = ns_GTAT_Urban::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 0] * ns_GTAT_Urban::c_wide;
	m_GTATUrban->m_fAbsY = ns_GTAT_Urban::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 1] * ns_GTAT_Urban::c_length;
	RandomUniform(&m_GTATUrban->m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU：");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_GTAT->m_RSUId, m_GTATUrban->m_fAbsX, m_GTATUrban->m_fAbsY);

	m_GTAT->m_DRAClusterNum = ns_GTAT_Urban::c_RSUClusterNum[m_GTAT->m_RSUId];
	m_GTAT->m_DRAClusterVeUEIdList = vector<list<int>>(m_GTAT->m_DRAClusterNum);

	initializeElse();
}


void cRSU::initializeHigh(sRSUConfigure &t_RSUConfigure) {
	m_GTAT->m_RSUId = t_RSUConfigure.wRSUID;
	m_GTATHigh->m_fAbsX = ns_GTAT_High::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 0] * 100;
	m_GTATHigh->m_fAbsY = ns_GTAT_High::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 1];
	RandomUniform(&m_GTATHigh->m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU：");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_GTAT->m_RSUId, m_GTATHigh->m_fAbsX, m_GTATHigh->m_fAbsY);

	m_GTAT->m_DRAClusterNum = ns_GTAT_High::c_RSUClusterNum;
	m_GTAT->m_DRAClusterVeUEIdList = vector<list<int>>(m_GTAT->m_DRAClusterNum);

	initializeElse();
}


void cRSU::initializeElse() {
	m_RRMDRA = new RRMDRA(m_GTAT);
	m_RRMRR = new RRMRR();
	m_WT = new WT();
	m_TMAC = new TMAC();
}

cRSU::~cRSU() {
	delete m_GTAT;
	delete m_GTATUrban;
	delete m_GTATHigh;
	delete m_RRM;
	delete m_RRMDRA;
	delete m_RRMRR;
	delete m_WT;
	delete m_TMAC;
}



string cRSU::RRMDRA::sDRAScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ eventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


std::string cRSU::RRMDRA::sDRAScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ " << endl;
	ss << indent << " eventId = " << eventId << endl;
	ss << indent << " VeUEId = " << VeUEId << endl;
	ss << indent << " remainBitNum = " << remainBitNum << endl;
	ss << indent << " transimitBitNum = " << transimitBitNum << endl;
	ss << indent << " occupiedTTINum(exclude current TTI) = " << occupiedTTINum << endl;
	ss << indent << "}";
	return ss.str();
}



cRSU::RRMDRA::RRMDRA(GTAT* t_GTAT) {
	m_GTAT = t_GTAT;

	/*  EMERGENCY  */
	m_DRAEmergencyPatternIsAvailable = vector<bool>(gc_DRAEmergencyTotalPatternNum, true);
	m_DRAEmergencyScheduleInfoTable = vector<sDRAScheduleInfo*>(gc_DRAEmergencyTotalPatternNum);
	m_DRAEmergencyTransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(gc_DRAEmergencyTotalPatternNum);
	/*  EMERGENCY  */

	m_DRAPatternIsAvailable = vector<vector<bool>>(m_GTAT->m_DRAClusterNum, vector<bool>(gc_DRATotalPatternNum, true));
	m_DRAScheduleInfoTable = vector<vector<sDRAScheduleInfo*>>(m_GTAT->m_DRAClusterNum, vector<sDRAScheduleInfo*>(gc_DRATotalPatternNum, nullptr));
	m_DRATransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(gc_DRATotalPatternNum, list<sDRAScheduleInfo*>(0, nullptr));

}


int cRSU::DRAGetClusterIdx(int TTI) {
	int roundATTI = TTI%gc_DRA_NTTI; //将TTI映射到[0-gc_DRA_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < m_GTAT->m_DRAClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_RRMDRA->m_DRAClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


string cRSU::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << m_GTAT->m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < m_GTAT->m_DRAClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : m_GTAT->m_DRAClusterVeUEIdList[clusterIdx]) {
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
	for (const tuple<int, int, int>&t : m_RRMDRA->m_DRAClusterTDRInfo)
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << endl;
	ss << indent << "    " << "}" << endl;


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}