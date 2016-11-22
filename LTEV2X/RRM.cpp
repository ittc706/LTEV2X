/*
* =====================================================================================
*
*       Filename:  RRM.cpp
*
*    Description:  TMCÄ£¿é
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
#include"RRM.h"
#include"Function.h"

using namespace std;

RRM_VeUE::RRM_VeUE(int t_TotalPatternNum){
	m_InterferenceVeUENum = vector<int>(t_TotalPatternNum);
	m_InterferenceVeUEIdVec = vector<vector<int>>(t_TotalPatternNum);
	m_PreInterferenceVeUEIdVec = vector<vector<int>>(t_TotalPatternNum);
	m_PreSINR = vector<double>(t_TotalPatternNum, (numeric_limits<double>::min)());
}

RRM::~RRM() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++)
		Delete::safeDelete(m_VeUEAry[VeUEId]);
	Delete::safeDelete(m_VeUEAry, true);
}


bool RRM_VeUE::isNeedRecalculateSINR(int t_PatternIdx) {
	if (m_InterferenceVeUEIdVec[t_PatternIdx].size() != m_PreInterferenceVeUEIdVec[t_PatternIdx].size()) return true;
	for (int i = 0; i < m_InterferenceVeUEIdVec[t_PatternIdx].size(); i++) {
		if (m_InterferenceVeUEIdVec[t_PatternIdx][i] != m_PreInterferenceVeUEIdVec[t_PatternIdx][i]) return true;
	}
	return false;
}