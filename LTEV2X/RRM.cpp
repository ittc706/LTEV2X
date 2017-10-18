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

const double RRM::s_CODE_RATE= 0.5;

const double RRM::s_DROP_SINR_BOUNDARY= 1.99;

RRM::RRM(System* t_Context) : m_Context(t_Context) {
	m_FileScheduleInfo.open("Log/RRMLog/ScheduleInfo.txt");
	m_FileClasterPerformInfo.open("Log/RRMLog/ClasterPerformInfo.txt");
	m_FileTTILogInfo.open("Log/RRMLog/TTILogInfo.txt");
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