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
	m_GTT = new GTT();
	m_GTT_Urban = new GTT_Urban();
	m_GTT_HighSpeed = new GTT_HighSpeed();
}


void RSU::initializeGTT_Urban(RSUConfigure &t_RSUConfigure){
	m_GTT->m_RSUId = t_RSUConfigure.RSUId;
	m_GTT_Urban->m_AbsX = ns_GTT_Urban::gc_RSUTopoRatio[m_GTT->m_RSUId * 2 + 0] * ns_GTT_Urban::gc_Width;
	m_GTT_Urban->m_AbsY = ns_GTT_Urban::gc_RSUTopoRatio[m_GTT->m_RSUId * 2 + 1] * ns_GTT_Urban::gc_Length;
	randomUniform(&m_GTT_Urban->m_FantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU：");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_GTT->m_RSUId, m_GTT_Urban->m_AbsX, m_GTT_Urban->m_AbsY);

	m_GTT->m_ClusterNum = ns_GTT_Urban::gc_RSUClusterNum[m_GTT->m_RSUId];
	m_GTT->m_ClusterVeUEIdList = vector<list<int>>(m_GTT->m_ClusterNum);

}


void RSU::initializeGTT_HighSpeed(RSUConfigure &t_RSUConfigure) {
	m_GTT->m_RSUId = t_RSUConfigure.RSUId;
	m_GTT_HighSpeed->m_AbsX = ns_GTT_HighSpeed::gc_RSUTopoRatio[m_GTT->m_RSUId * 2 + 0] * 100;
	m_GTT_HighSpeed->m_AbsY = ns_GTT_HighSpeed::gc_RSUTopoRatio[m_GTT->m_RSUId * 2 + 1];
	randomUniform(&m_GTT_HighSpeed->m_FantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU：");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_GTT->m_RSUId, m_GTT_HighSpeed->m_AbsX, m_GTT_HighSpeed->m_AbsY);

	m_GTT->m_ClusterNum = ns_GTT_HighSpeed::gc_RSUClusterNum;
	m_GTT->m_ClusterVeUEIdList = vector<list<int>>(m_GTT->m_ClusterNum);

}


void RSU::initializeRRM_TDM_DRA() {
	m_RRM = new RRM();
	m_RRM_TDM_DRA = new RRM_TDM_DRA(this);
}


void RSU::initializeRRM_RR() {
	m_RRM = new RRM();
	m_RRM_RR = new RRM_RR(this);
}


void RSU::initializeWT() {
	m_WT = new WT();
}


void RSU::initializeTMC() {
	m_TMC = new TMC();
}



RSU::~RSU() {
	if (m_GTT != nullptr) {
		delete m_GTT;
		m_GTT = nullptr;
	}
	if (m_GTT_Urban != nullptr) {
		delete m_GTT_Urban;
		m_GTT_Urban = nullptr;
	}
	if (m_GTT_HighSpeed != nullptr) {
		delete m_GTT_HighSpeed;
		m_GTT_HighSpeed = nullptr;
	}
	if (m_RRM != nullptr) {
		delete m_RRM;
		m_RRM = nullptr;
	}
	if (m_RRM_TDM_DRA != nullptr) {
		delete m_RRM_TDM_DRA;
		m_RRM_TDM_DRA = nullptr;
	}
	if (m_RRM_RR != nullptr) {
		delete m_RRM_RR;
		m_RRM_RR = nullptr;
	}
	if (m_WT != nullptr) {
		delete m_WT;
		m_WT = nullptr;
	}
	if (m_TMC != nullptr) {
		delete m_TMC;
		m_TMC = nullptr;
	}
}



string RSU::RRM_TDM_DRA::ScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ EventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


string RSU::RRM_TDM_DRA::ScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ " << endl;
	ss << indent << " EventId = " << eventId << endl;
	ss << indent << " VeUEId = " << VeUEId << endl;
	ss << indent << " currentPackageIdx = " << currentPackageIdx << endl;
	ss << indent << " remainBitNum = " << remainBitNum << endl;
	ss << indent << " transimitBitNum = " << transimitBitNum << endl;
	ss << indent << "}";
	return ss.str();
}



RSU::RRM_TDM_DRA::RRM_TDM_DRA(RSU* t_this) {
	m_This = t_this;

	/*  EMERGENCY  */
	m_EmergencyPatternIsAvailable = vector<bool>(ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY], true);
	m_EmergencyScheduleInfoTable = vector<ScheduleInfo*>(ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]);
	m_EmergencyTransimitScheduleInfoList = vector<list<ScheduleInfo*>>(ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]);
	/*  EMERGENCY  */

	m_PatternIsAvailable = vector<vector<bool>>(m_This->m_GTT->m_ClusterNum, vector<bool>(ns_RRM_TDM_DRA::gc_TotalPatternNum - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY], true));
	m_ScheduleInfoTable = vector<vector<ScheduleInfo*>>(m_This->m_GTT->m_ClusterNum, vector<ScheduleInfo*>(ns_RRM_TDM_DRA::gc_TotalPatternNum - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY], nullptr));
	m_TransimitScheduleInfoList = vector<list<ScheduleInfo*>>(ns_RRM_TDM_DRA::gc_TotalPatternNum - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY], list<ScheduleInfo*>(0, nullptr));

}


int RSU::RRM_TDM_DRA::getClusterIdx(int TTI) {
	int roundATTI = TTI%ns_RRM_TDM_DRA::gc_NTTI; //将TTI映射到[0-gc_DRA_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < m_This->m_GTT->m_ClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_ClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


string RSU::RRM_TDM_DRA::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << m_This->m_GTT->m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < m_This->m_GTT->m_ClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : m_This->m_GTT->m_ClusterVeUEIdList[clusterIdx]) {
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
	for (const tuple<int, int, int>&t : m_This->m_RRM_TDM_DRA->m_ClusterTDRInfo)
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << endl;
	ss << indent << "    " << "}" << endl;


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}



string RSU::RRM_RR::ScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ EventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


string RSU::RRM_RR::ScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ " << endl;
	ss << indent << " EventId = " << eventId << endl;
	ss << indent << " VeUEId = " << VeUEId << endl;
	ss << indent << " currentPackageIdx = " << currentPackageIdx << endl;
	ss << indent << " remainBitNum = " << remainBitNum << endl;
	ss << indent << " transimitBitNum = " << transimitBitNum << endl;
	ss << indent << "}";
	return ss.str();
}




RSU::RRM_RR::RRM_RR(RSU* t_this) {
	m_This = t_this;
	m_AdmitEventIdList = vector<vector<int>>(m_This->m_GTT->m_ClusterNum);
	m_WaitEventIdList= vector<list<int>>(m_This->m_GTT->m_ClusterNum);
	m_ScheduleInfoTable = vector<vector<ScheduleInfo*>>(m_This->m_GTT->m_ClusterNum, vector<ScheduleInfo*>(ns_RRM_RR::gc_RRTotalPatternNum, nullptr));
}