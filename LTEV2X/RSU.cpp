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


RSU::RSU() {
	m_GTAT = new GTAT();
	m_GTAT_Urban = new GTAT_Urban();
	m_GTAT_HighSpeed = new GTAT_HighSpeed();
}


void RSU::initializeUrban(RSUConfigure &t_RSUConfigure){
	m_GTAT->m_RSUId = t_RSUConfigure.RSUId;
	m_GTAT_Urban->m_AbsX = ns_GTAT_Urban::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 0] * ns_GTAT_Urban::c_wide;
	m_GTAT_Urban->m_AbsY = ns_GTAT_Urban::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 1] * ns_GTAT_Urban::c_length;
	RandomUniform(&m_GTAT_Urban->m_FantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU：");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_GTAT->m_RSUId, m_GTAT_Urban->m_AbsX, m_GTAT_Urban->m_AbsY);

	m_GTAT->m_ClusterNum = ns_GTAT_Urban::c_RSUClusterNum[m_GTAT->m_RSUId];
	m_GTAT->m_DRAClusterVeUEIdList = vector<list<int>>(m_GTAT->m_ClusterNum);

}


void RSU::initializeHighSpeed(RSUConfigure &t_RSUConfigure) {
	m_GTAT->m_RSUId = t_RSUConfigure.RSUId;
	m_GTAT_HighSpeed->m_AbsX = ns_GTAT_HighSpeed::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 0] * 100;
	m_GTAT_HighSpeed->m_AbsY = ns_GTAT_HighSpeed::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 1];
	RandomUniform(&m_GTAT_HighSpeed->m_FantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU：");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_GTAT->m_RSUId, m_GTAT_HighSpeed->m_AbsX, m_GTAT_HighSpeed->m_AbsY);

	m_GTAT->m_ClusterNum = ns_GTAT_HighSpeed::c_RSUClusterNum;
	m_GTAT->m_DRAClusterVeUEIdList = vector<list<int>>(m_GTAT->m_ClusterNum);

}


void RSU::initializeDRA() {
	m_RRM = new RRM();
	m_RRM_DRA = new RRM_DRA(this);
}


void RSU::initializeRR() {
	m_RRM = new RRM();
	m_RRM_RR = new RRM_RR(this);
}


void RSU::initializeWT() {
	m_WT = new WT();
}


void RSU::initializeTMAC() {
	m_TMAC = new TMAC();
}



RSU::~RSU() {
	if (m_GTAT != nullptr) {
		delete m_GTAT;
		m_GTAT = nullptr;
	}
	if (m_GTAT_Urban != nullptr) {
		delete m_GTAT_Urban;
		m_GTAT_Urban = nullptr;
	}
	if (m_GTAT_HighSpeed != nullptr) {
		delete m_GTAT_HighSpeed;
		m_GTAT_HighSpeed = nullptr;
	}
	if (m_RRM != nullptr) {
		delete m_RRM;
		m_RRM = nullptr;
	}
	if (m_RRM_DRA != nullptr) {
		delete m_RRM_DRA;
		m_RRM_DRA = nullptr;
	}
	if (m_RRM_RR != nullptr) {
		delete m_RRM_RR;
		m_RRM_RR = nullptr;
	}
	if (m_WT != nullptr) {
		delete m_WT;
		m_WT = nullptr;
	}
	if (m_TMAC != nullptr) {
		delete m_TMAC;
		m_TMAC = nullptr;
	}
}



string RSU::RRM_DRA::DRAScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ eventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


std::string RSU::RRM_DRA::DRAScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ " << endl;
	ss << indent << " eventId = " << eventId << endl;
	ss << indent << " VeUEId = " << VeUEId << endl;
	ss << indent << " currentPackageIdx = " << currentPackageIdx << endl;
	ss << indent << " remainBitNum = " << remainBitNum << endl;
	ss << indent << " transimitBitNum = " << transimitBitNum << endl;
	ss << indent << "}";
	return ss.str();
}



RSU::RRM_DRA::RRM_DRA(RSU* t_this) {
	m_This = t_this;

	/*  EMERGENCY  */
	m_DRAEmergencyPatternIsAvailable = vector<bool>(gc_DRAPatternNumPerPatternType[EMERGENCY], true);
	m_DRAEmergencyScheduleInfoTable = vector<DRAScheduleInfo*>(gc_DRAPatternNumPerPatternType[EMERGENCY]);
	m_DRAEmergencyTransimitScheduleInfoList = vector<list<DRAScheduleInfo*>>(gc_DRAPatternNumPerPatternType[EMERGENCY]);
	/*  EMERGENCY  */

	m_DRAPatternIsAvailable = vector<vector<bool>>(m_This->m_GTAT->m_ClusterNum, vector<bool>(gc_DRATotalPatternNum - gc_DRAPatternNumPerPatternType[EMERGENCY], true));
	m_DRAScheduleInfoTable = vector<vector<DRAScheduleInfo*>>(m_This->m_GTAT->m_ClusterNum, vector<DRAScheduleInfo*>(gc_DRATotalPatternNum - gc_DRAPatternNumPerPatternType[EMERGENCY], nullptr));
	m_DRATransimitScheduleInfoList = vector<list<DRAScheduleInfo*>>(gc_DRATotalPatternNum - gc_DRAPatternNumPerPatternType[EMERGENCY], list<DRAScheduleInfo*>(0, nullptr));

}


int RSU::RRM_DRA::DRAGetClusterIdx(int TTI) {
	int roundATTI = TTI%gc_DRA_NTTI; //将TTI映射到[0-gc_DRA_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < m_This->m_GTAT->m_ClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_DRAClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


string RSU::RRM_DRA::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << m_This->m_GTAT->m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < m_This->m_GTAT->m_ClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : m_This->m_GTAT->m_DRAClusterVeUEIdList[clusterIdx]) {
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
	for (const tuple<int, int, int>&t : m_This->m_RRM_DRA->m_DRAClusterTDRInfo)
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << endl;
	ss << indent << "    " << "}" << endl;


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}



std::string RSU::RRM_RR::RRScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ eventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


std::string RSU::RRM_RR::RRScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ " << endl;
	ss << indent << " eventId = " << eventId << endl;
	ss << indent << " VeUEId = " << VeUEId << endl;
	ss << indent << " currentPackageIdx = " << currentPackageIdx << endl;
	ss << indent << " remainBitNum = " << remainBitNum << endl;
	ss << indent << " transimitBitNum = " << transimitBitNum << endl;
	ss << indent << "}";
	return ss.str();
}




RSU::RRM_RR::RRM_RR(RSU* t_this) {
	m_This = t_this;
	m_RRAdmitEventIdList = vector<vector<int>>(m_This->m_GTAT->m_ClusterNum);
	m_RRWaitEventIdList= vector<list<int>>(m_This->m_GTAT->m_ClusterNum);
	m_RRScheduleInfoTable = vector<vector<RRScheduleInfo*>>(m_This->m_GTAT->m_ClusterNum, vector<RRScheduleInfo*>(gc_RRTotalPatternNum, nullptr));
}