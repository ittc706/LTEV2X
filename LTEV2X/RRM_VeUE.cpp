/*
* =====================================================================================
*
*       Filename:  RRM_VeUE.cpp
*
*    Description:  GTTÄ£¿éÖÐVeUEÊÓÍ¼
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

#include"VUE.h"
#include"RRM_VeUE.h"

using namespace std;

RRM_VeUE::RRM_VeUE(int t_TotalPatternNum) :m_ModulationType(RRM::s_MODULATION_TYPE), m_CodeRate(RRM::s_CODE_RATE) {
	m_InterferenceVeUENum = vector<int>(t_TotalPatternNum);
	m_InterferenceVeUEIdVec = vector<vector<int>>(t_TotalPatternNum);
	m_PreInterferenceVeUEIdVec = vector<vector<int>>(t_TotalPatternNum);
	m_PreSINR = vector<double>(t_TotalPatternNum, (numeric_limits<double>::min)());
}


VeUE* RRM_VeUE::getSystemPoint() { return m_This; }

void RRM_VeUE::setSystemPoint(VeUE* t_Point) { m_This = t_Point; }


default_random_engine RRM_ICC_DRA_VeUE::s_Engine((unsigned)time(NULL));


RRM_ICC_DRA_VeUE::RRM_ICC_DRA_VeUE() :RRM_VeUE(RRM_ICC_DRA::s_TOTAL_PATTERN_NUM) {}


int RRM_ICC_DRA_VeUE::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}


std::string RRM_ICC_DRA_VeUE::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_ClusterIdx << " }";
	return ss.str();
}


RRM_RR_VeUE::RRM_RR_VeUE() :RRM_VeUE(RRM_RR::s_TOTAL_PATTERN_NUM) {}


std::string RRM_RR_VeUE::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_ClusterIdx << " }";
	return ss.str();
}


default_random_engine RRM_TDM_DRA_VeUE::s_Engine((unsigned)time(NULL));


RRM_TDM_DRA_VeUE::RRM_TDM_DRA_VeUE() :RRM_VeUE(RRM_TDM_DRA::s_TOTAL_PATTERN_NUM) {}


int RRM_TDM_DRA_VeUE::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}

string RRM_TDM_DRA_VeUE::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_ClusterIdx;
	ss << " , ScheduleInterval = [" << left << setw(3) << get<0>(m_ScheduleInterval) << "," << left << setw(3) << get<1>(m_ScheduleInterval) << "] }";
	return ss.str();
}