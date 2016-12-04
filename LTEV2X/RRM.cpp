/*
* =====================================================================================
*
*       Filename:  RRM.cpp
*
*    Description:  RRMÄ£¿é
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

#include<limits>
#include<sstream>
#include<iomanip>
#include"System.h"

#include"GTT.h"
#include"RRM.h"

#include"VUE.h"
#include"RSU.h"

#include"Function.h"
using namespace std;

RRM_VeUE::RRM_VeUE(int t_TotalPatternNum):m_ModulationType(RRM::s_MODULATION_TYPE), m_CodeRate(RRM::s_CODE_RATE){
	m_InterferenceVeUENum = vector<int>(t_TotalPatternNum);
	m_InterferenceVeUEIdVec = vector<vector<int>>(t_TotalPatternNum);
	m_PreInterferenceVeUEIdVec = vector<vector<int>>(t_TotalPatternNum);
	m_PreSINR = vector<double>(t_TotalPatternNum, (numeric_limits<double>::min)());
}


VeUE* RRM_VeUE::getSystemPoint() { return m_This; }

void RRM_VeUE::setSystemPoint(VeUE* t_Point) { m_This = t_Point; }


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


const double RRM::s_CODE_RATE= 0.5;

const double RRM::s_DROP_SINR_BOUNDARY= 1.99;

RRM::RRM(System* t_Context) : m_Context(t_Context) {
	if (getContext()->m_Config.platform == Windows) {
		m_FileScheduleInfo.open("Log\\RRMLog\\ScheduleInfo.txt");
		m_FileClasterPerformInfo.open("Log\\RRMLog\\ClasterPerformInfo.txt");
		m_FileTTILogInfo.open("Log\\RRMLog\\TTILogInfo.txt");
	}
	else if (getContext()->m_Config.platform == Linux) {
		m_FileScheduleInfo.open("Log/RRMLog/ScheduleInfo.txt");
		m_FileClasterPerformInfo.open("Log/RRMLog/ClasterPerformInfo.txt");
		m_FileTTILogInfo.open("Log/RRMLog/TTILogInfo.txt");
	}
	else {
		throw logic_error("Platform Config Error!");
	}
}

RRM::~RRM() {
	for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++)
		Delete::safeDelete(m_VeUEAry[VeUEId]);
	Delete::safeDelete(m_VeUEAry, true);

	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++)
		Delete::safeDelete(m_RSUAry[RSUId]);
	Delete::safeDelete(m_RSUAry, true);

	m_FileScheduleInfo.close();
	m_FileClasterPerformInfo.close();
	m_FileTTILogInfo.close();
}


bool RRM_VeUE::isNeedRecalculateSINR(int t_PatternIdx) {
	if (m_InterferenceVeUEIdVec[t_PatternIdx].size() != m_PreInterferenceVeUEIdVec[t_PatternIdx].size()) return true;
	for (int i = 0; i < m_InterferenceVeUEIdVec[t_PatternIdx].size(); i++) {
		if (m_InterferenceVeUEIdVec[t_PatternIdx][i] != m_PreInterferenceVeUEIdVec[t_PatternIdx][i]) return true;
	}
	return false;
}