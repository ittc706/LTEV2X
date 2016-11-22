#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Function.h"


using namespace std;


void RSU::initializeGTT_Urban(RSUConfig &t_RSUConfig){
	m_GTT = new GTT();
	m_GTT_Urban = new GTT_Urban();

	m_GTT->m_RSUId = t_RSUConfig.RSUId;
	m_GTT->m_AbsX = ns_GTT_Urban::gc_RSUTopoRatio[m_GTT->m_RSUId * 2 + 0] * ns_GTT_Urban::gc_Width;
	m_GTT->m_AbsY = ns_GTT_Urban::gc_RSUTopoRatio[m_GTT->m_RSUId * 2 + 1] * ns_GTT_Urban::gc_Length;
	randomUniform(&m_GTT->m_FantennaAngle, 1, 180.0f, -180.0f, false);
	g_FileLocationInfo << m_GTT->toString(0);

	m_GTT->m_ClusterNum = ns_GTT_Urban::gc_RSUClusterNum[m_GTT->m_RSUId];
	m_GTT->m_ClusterVeUEIdList = vector<list<int>>(m_GTT->m_ClusterNum);

}


void RSU::initializeGTT_HighSpeed(RSUConfig &t_RSUConfig) {
	m_GTT = new GTT();
	m_GTT_HighSpeed = new GTT_HighSpeed();

	m_GTT->m_RSUId = t_RSUConfig.RSUId;
	m_GTT->m_AbsX = ns_GTT_HighSpeed::gc_RSUTopoRatio[m_GTT->m_RSUId * 2 + 0] * 100;
	m_GTT->m_AbsY = ns_GTT_HighSpeed::gc_RSUTopoRatio[m_GTT->m_RSUId * 2 + 1];
	randomUniform(&m_GTT->m_FantennaAngle, 1, 180.0f, -180.0f, false);
	g_FileLocationInfo << m_GTT->toString(0);

	m_GTT->m_ClusterNum = ns_GTT_HighSpeed::gc_RSUClusterNum;
	m_GTT->m_ClusterVeUEIdList = vector<list<int>>(m_GTT->m_ClusterNum);

}


void RSU::initializeRRM_TDM_DRA() {
	m_RRM = new RRM();
	m_RRM_TDM_DRA = new RRM_TDM_DRA(this);
}


void RSU::initializeRRM_ICC_DRA() {
	m_RRM = new RRM();
	m_RRM_ICC_DRA = new RRM_ICC_DRA(this);
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


RSU::GTT::~GTT() {
	Delete::safeDelete(m_IMTA, true);
}


std::string RSU::GTT::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "Road[" << m_RSUId << "]: (" << m_AbsX << "," << m_AbsY << ")" << endl;
	return ss.str();
}


RSU::~RSU() {
	Delete::safeDelete(m_GTT);
	Delete::safeDelete(m_GTT_Urban);
	Delete::safeDelete(m_GTT_HighSpeed);
	Delete::safeDelete(m_RRM);
	Delete::safeDelete(m_RRM_TDM_DRA);
	Delete::safeDelete(m_RRM_ICC_DRA);
	Delete::safeDelete(m_RRM_RR);
	Delete::safeDelete(m_WT);
	Delete::safeDelete(m_TMC);
}



string RSU::RRM::ScheduleInfo::toLogString() {
	ostringstream ss;
	ss << "[ EventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


string RSU::RRM::ScheduleInfo::toScheduleString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ " << endl;
	ss << indent << " EventId = " << eventId << endl;
	ss << indent << " VeUEId = " << VeUEId << endl;
	ss << indent << " ClusterIdx = " << clusterIdx << endl;
	ss << indent << " currentPackageIdx = " << currentPackageIdx << endl;
	ss << indent << " remainBitNum = " << remainBitNum << endl;
	ss << indent << " transimitBitNum = " << transimitBitNum << endl;
	ss << indent << "}";
	return ss.str();
}



RSU::RRM_TDM_DRA::RRM_TDM_DRA(RSU* t_This) {
	m_This = t_This;

	m_AccessEventIdList = vector<pair<list<int>, list<int>>>(m_This->m_GTT->m_ClusterNum);
	m_WaitEventIdList = vector<pair<list<int>,list<int>>>(m_This->m_GTT->m_ClusterNum);
	m_PatternIsAvailable = vector<vector<bool>>(m_This->m_GTT->m_ClusterNum, vector<bool>(ns_RRM_TDM_DRA::gc_TotalPatternNum, true));
	m_ScheduleInfoTable = vector<vector<RSU::RRM::ScheduleInfo*>>(m_This->m_GTT->m_ClusterNum, vector<RSU::RRM::ScheduleInfo*>(ns_RRM_TDM_DRA::gc_TotalPatternNum, nullptr));
	m_TransimitScheduleInfoList = vector<vector<list<RSU::RRM::ScheduleInfo*>>>(m_This->m_GTT->m_ClusterNum, vector<list<RSU::RRM::ScheduleInfo*>>(ns_RRM_TDM_DRA::gc_TotalPatternNum));

}


int RSU::RRM_TDM_DRA::getClusterIdx(int t_TTI) {
	int roundATTI = t_TTI%ns_RRM_TDM_DRA::gc_NTTI; //将TTI映射到[0-gc_DRA_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < m_This->m_GTT->m_ClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_ClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


string RSU::RRM_TDM_DRA::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
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


RSU::RRM_ICC_DRA::RRM_ICC_DRA(RSU* t_This) {
	m_This = t_This;
	
	m_AccessEventIdList= vector<list<int>>(m_This->m_GTT->m_ClusterNum);
	m_WaitEventIdList = vector<list<int>>(m_This->m_GTT->m_ClusterNum);
	m_PatternIsAvailable = vector<vector<bool>>(m_This->m_GTT->m_ClusterNum, vector<bool>(ns_RRM_ICC_DRA::gc_TotalPatternNum, true));
	m_ScheduleInfoTable = vector<vector<RSU::RRM::ScheduleInfo*>>(m_This->m_GTT->m_ClusterNum, vector<RSU::RRM::ScheduleInfo*>(ns_RRM_ICC_DRA::gc_TotalPatternNum, nullptr));
	m_TransimitScheduleInfoList = vector<vector<list<RSU::RRM::ScheduleInfo*>>>(m_This->m_GTT->m_ClusterNum, vector<list<RSU::RRM::ScheduleInfo*>>(ns_RRM_ICC_DRA::gc_TotalPatternNum));

}


string RSU::RRM_ICC_DRA::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
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


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}



RSU::RRM_RR::RRM_RR(RSU* t_This) {
	m_This = t_This;

	m_AccessEventIdList = vector<list<int>>(m_This->m_GTT->m_ClusterNum);
	m_WaitEventIdList= vector<list<int>>(m_This->m_GTT->m_ClusterNum);
	m_TransimitScheduleInfoTable = vector<vector<RSU::RRM::ScheduleInfo*>>(m_This->m_GTT->m_ClusterNum, vector<RSU::RRM::ScheduleInfo*>(ns_RRM_RR::gc_TotalPatternNum, nullptr));
}


std::string RSU::RRM_RR::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
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


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}