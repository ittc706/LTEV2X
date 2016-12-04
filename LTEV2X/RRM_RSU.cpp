/*
* =====================================================================================
*
*       Filename:  RRM_RSU.cpp
*
*    Description:  GTT模块中RSU视图
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<iomanip>
#include<sstream>
#include"GTT.h"
#include"RRM_ICC_DRA.h"
#include"RRM_RR.h"
#include"RRM_TDM_DRA.h"

#include"RSU.h"
#include"RRM_RSU.h"

using namespace std;

string RRM_RSU::ScheduleInfo::toLogString() {
	ostringstream ss;
	ss << "[ EventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


string RRM_RSU::ScheduleInfo::toScheduleString(int t_NumTab) {
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

RRM_ICC_DRA_RSU::RRM_ICC_DRA_RSU() {}


void RRM_ICC_DRA_RSU::initialize() {
	m_AccessEventIdList = vector<list<int>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_WaitEventIdList = vector<list<int>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_PatternIsAvailable = vector<vector<bool>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<bool>(RRM_ICC_DRA::s_TOTAL_PATTERN_NUM, true));
	m_ScheduleInfoTable = vector<vector<ScheduleInfo*>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<ScheduleInfo*>(RRM_ICC_DRA::s_TOTAL_PATTERN_NUM, nullptr));
	m_TransimitScheduleInfoList = vector<vector<list<ScheduleInfo*>>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<list<ScheduleInfo*>>(RRM_ICC_DRA::s_TOTAL_PATTERN_NUM));
}


string RRM_ICC_DRA_RSU::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << getSystemPoint()->getGTTPoint()->m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx]) {
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


void RRM_ICC_DRA_RSU::pushToAccessEventIdList(int t_ClusterIdx, int t_EventId) {
	m_AccessEventIdList[t_ClusterIdx].push_back(t_EventId);
}


void RRM_ICC_DRA_RSU::pushToWaitEventIdList(int t_ClusterIdx, int t_EventId) {
	m_WaitEventIdList[t_ClusterIdx].push_back(t_EventId);
}


void RRM_ICC_DRA_RSU::pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}


void RRM_ICC_DRA_RSU::pushToTransmitScheduleInfoList(ScheduleInfo* t_Info) {
	m_TransimitScheduleInfoList[t_Info->clusterIdx][t_Info->patternIdx].push_back(t_Info);
}


void RRM_ICC_DRA_RSU::pushToScheduleInfoTable(ScheduleInfo* t_Info) {
	m_ScheduleInfoTable[t_Info->clusterIdx][t_Info->patternIdx] = t_Info;
}


void RRM_ICC_DRA_RSU::pullFromScheduleInfoTable(int t_TTI) {
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		for (int patternIdx = 0; patternIdx < RRM_ICC_DRA::s_TOTAL_PATTERN_NUM; patternIdx++) {
			if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
				m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
				m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
			}
		}
	}
}


RRM_RR_RSU::RRM_RR_RSU() {}


void RRM_RR_RSU::initialize() {
	m_AccessEventIdList = vector<list<int>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_WaitEventIdList = vector<list<int>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_TransimitScheduleInfoTable = vector<vector<ScheduleInfo*>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<ScheduleInfo*>(RRM_RR::s_TOTAL_PATTERN_NUM, nullptr));
}

std::string RRM_RR_RSU::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << getSystemPoint()->getGTTPoint()->m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx]) {
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


void RRM_RR_RSU::pushToAccessEventIdList(int t_ClusterIdx, int t_EventId) {
	m_AccessEventIdList[t_ClusterIdx].push_back(t_EventId);
}


void RRM_RR_RSU::pushToWaitEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId) {
	if (t_IsEmergency)
		m_WaitEventIdList[t_ClusterIdx].insert(m_WaitEventIdList[t_ClusterIdx].begin(), t_EventId);
	else
		m_WaitEventIdList[t_ClusterIdx].push_back(t_EventId);
}


void RRM_RR_RSU::pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}


void RRM_RR_RSU::pushToTransimitScheduleInfoTable(ScheduleInfo* t_Info) {
	m_TransimitScheduleInfoTable[t_Info->clusterIdx][t_Info->patternIdx] = t_Info;
}


RRM_TDM_DRA_RSU::RRM_TDM_DRA_RSU() {}


void RRM_TDM_DRA_RSU::initialize() {
	m_AccessEventIdList = vector<pair<list<int>, list<int>>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_WaitEventIdList = vector<pair<list<int>, list<int>>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_PatternIsAvailable = vector<vector<bool>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<bool>(RRM_TDM_DRA::s_TOTAL_PATTERN_NUM, true));
	m_ScheduleInfoTable = vector<vector<ScheduleInfo*>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<ScheduleInfo*>(RRM_TDM_DRA::s_TOTAL_PATTERN_NUM, nullptr));
	m_TransimitScheduleInfoList = vector<vector<list<ScheduleInfo*>>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<list<ScheduleInfo*>>(RRM_TDM_DRA::s_TOTAL_PATTERN_NUM));
}


int RRM_TDM_DRA_RSU::getClusterIdx(int t_TTI) {
	int roundATTI = t_TTI%RRM_TDM_DRA::s_NTTI; //将TTI映射到[0-s_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_ClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


string RRM_TDM_DRA_RSU::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << getSystemPoint()->getGTTPoint()->m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx]) {
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
	for (const tuple<int, int, int>&t : getTDM_DRAPoint()->m_ClusterTDRInfo)
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << endl;
	ss << indent << "    " << "}" << endl;


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}


void RRM_TDM_DRA_RSU::pushToAccessEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId) {
	if (t_IsEmergency)
		m_AccessEventIdList[t_ClusterIdx].first.push_back(t_EventId);
	else
		m_AccessEventIdList[t_ClusterIdx].second.push_back(t_EventId);
}


void RRM_TDM_DRA_RSU::pushToWaitEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId) {
	if (t_IsEmergency)
		m_WaitEventIdList[t_ClusterIdx].first.push_back(t_EventId);
	else
		m_WaitEventIdList[t_ClusterIdx].second.push_back(t_EventId);
}


void RRM_TDM_DRA_RSU::pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}


void RRM_TDM_DRA_RSU::pushToTransimitScheduleInfoList(ScheduleInfo* t_Info) {
	m_TransimitScheduleInfoList[t_Info->clusterIdx][t_Info->patternIdx].push_back(t_Info);
}


void RRM_TDM_DRA_RSU::pushToScheduleInfoTable(ScheduleInfo* t_Info) {
	m_ScheduleInfoTable[t_Info->clusterIdx][t_Info->patternIdx] = t_Info;
}


void RRM_TDM_DRA_RSU::pullFromScheduleInfoTable(int t_TTI) {
	/*将处于调度表中当前可以传输的信息压入m_TransimitEventIdList*/

	/*  EMERGENCY  */
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		for (int patternIdx = 0; patternIdx < RRM_TDM_DRA::s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
			if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
				m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
				m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
			}
		}
	}
	/*  EMERGENCY  */

	int clusterIdx = getClusterIdx(t_TTI);
	for (int patternIdx = RRM_TDM_DRA::s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < RRM_TDM_DRA::s_TOTAL_PATTERN_NUM; patternIdx++) {
		if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
			m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
			m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
		}
	}
}

