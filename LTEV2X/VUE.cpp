#include<vector>
#include<iomanip>
#include<iostream>
#include<sstream>
#include<math.h>
#include"VUE.h"
#include"RSU.h"
#include"Function.h"

using namespace std;

int VeUE::m_VeUECount = 0;


void VeUE::initializeGTT_Urban(VeUEConfig &t_VeUEConfig) {
	m_GTT = new GTT();
	m_GTT_Urban = new GTT_Urban();

	m_GTT->m_RoadId = t_VeUEConfig.roadId;
	m_GTT_Urban->m_LocationId = t_VeUEConfig.locationId;
	m_GTT->m_X = t_VeUEConfig.X;
	m_GTT->m_Y = t_VeUEConfig.Y;
	m_GTT->m_AbsX = t_VeUEConfig.AbsX;
	m_GTT->m_AbsY = t_VeUEConfig.AbsY;
	m_GTT->m_V = t_VeUEConfig.V;

	if ((0 < m_GTT_Urban->m_LocationId) && (m_GTT_Urban->m_LocationId <= 61))
		m_GTT->m_VAngle = 90;
	else if ((61 < m_GTT_Urban->m_LocationId) && (m_GTT_Urban->m_LocationId <= 96))
		m_GTT->m_VAngle = 0;
	else if ((96 < m_GTT_Urban->m_LocationId) && (m_GTT_Urban->m_LocationId <= 157))
		m_GTT->m_VAngle = -90;
	else
		m_GTT->m_VAngle = -180;

	randomUniform(&m_GTT->m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTT->m_Nt = 1;
	m_GTT->m_Nr = 2;
	m_GTT->m_H = new double[2 * 1024 * 2];

}


void VeUE::initializeGTT_HighSpeed(VeUEConfig &t_VeUEConfig) {
	m_GTT = new GTT();
	m_GTT_HighSpeed = new GTT_HighSpeed();

	m_GTT->m_RoadId = t_VeUEConfig.laneId;
	m_GTT->m_X = t_VeUEConfig.X;
	m_GTT->m_Y = t_VeUEConfig.Y;
	m_GTT->m_AbsX = t_VeUEConfig.AbsX;
	m_GTT->m_AbsY = t_VeUEConfig.AbsY;
	m_GTT->m_V = t_VeUEConfig.V / 3.6f;

	if (m_GTT->m_RoadId <= 2)
		m_GTT->m_VAngle = 0;
	else
		m_GTT->m_VAngle = 180;

	randomUniform(&m_GTT->m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTT->m_Nt = 1;
	m_GTT->m_Nr = 2;
	m_GTT->m_H = new double[2 * 1024 * 2];
}


void VeUE::initializeRRM_TDM_DRA() {
	m_RRM = new RRM();
	m_RRM_TDM_DRA = new RRM_TDM_DRA(this);

	m_RRM->m_InterferenceVeUENum = vector<int>(ns_RRM_TDM_DRA::gc_TotalPatternNum);
	m_RRM->m_InterferenceVeUEIdVec = vector<vector<int>>(ns_RRM_TDM_DRA::gc_TotalPatternNum);
	m_RRM->m_PreInterferenceVeUEIdVec = vector<vector<int>>(ns_RRM_TDM_DRA::gc_TotalPatternNum);
	m_RRM->m_PreSINR = vector<double>(ns_RRM_TDM_DRA::gc_TotalPatternNum, (numeric_limits<double>::min)());

	//这两个数据比较特殊，必须等到GTT模块初始化完毕后，车辆的数目才能确定下来
	m_GTT->m_InterferencePloss = vector<double>(m_VeUECount,0);
	m_GTT->m_InterferenceH = vector<double*>(m_VeUECount, nullptr);
}

void VeUE::initializeRRM_ICC_DRA() {
	m_RRM = new RRM();
	m_RRM_ICC_DRA = new RRM_ICC_DRA(this);

	m_RRM->m_InterferenceVeUENum = vector<int>(ns_RRM_ICC_DRA::gc_TotalPatternNum);
	m_RRM->m_InterferenceVeUEIdVec = vector<vector<int>>(ns_RRM_ICC_DRA::gc_TotalPatternNum);
	m_RRM->m_PreInterferenceVeUEIdVec = vector<vector<int>>(ns_RRM_ICC_DRA::gc_TotalPatternNum);
	m_RRM->m_PreSINR = vector<double>(ns_RRM_ICC_DRA::gc_TotalPatternNum, (numeric_limits<double>::min)());

	//这两个数据比较特殊，必须等到GTT模块初始化完毕后，车辆的数目才能确定下来
	m_GTT->m_InterferencePloss = vector<double>(m_VeUECount, 0);
	m_GTT->m_InterferenceH = vector<double*>(m_VeUECount, nullptr);
}

void VeUE::initializeRRM_RR() {
	m_RRM = new RRM();
	m_RRM_RR = new RRM_RR(this);

	m_RRM->m_InterferenceVeUENum = vector<int>(ns_RRM_RR::gc_TotalPatternNum);
	m_RRM->m_InterferenceVeUEIdVec = vector<vector<int>>(ns_RRM_RR::gc_TotalPatternNum);
	m_RRM->m_PreInterferenceVeUEIdVec = vector<vector<int>>(ns_RRM_RR::gc_TotalPatternNum);
	m_RRM->m_PreSINR = vector<double>(ns_RRM_RR::gc_TotalPatternNum, (numeric_limits<double>::min)());

	//这两个数据比较特殊，必须等到GTT模块初始化完毕后，车辆的数目才能确定下来
	m_GTT->m_InterferencePloss = vector<double>(m_VeUECount, 0);
	m_GTT->m_InterferenceH = vector<double*>(m_VeUECount, nullptr);
}


void VeUE::initializeWT() {
	m_WT = new WT();
}


void VeUE::initializeTMC() {
	m_TMC = new TMC();
}


VeUE::GTT::~GTT() {
	Delete::safeDelete(m_IMTA, true);
	Delete::safeDelete(m_H, true);
	for (double*& p : m_InterferenceH) {
		Delete::safeDelete(p);
	}
	Delete::safeDelete(m_Distance, true);
}


VeUE::~VeUE() {
	Delete::safeDelete(m_GTT);
	Delete::safeDelete(m_GTT_Urban);
	Delete::safeDelete(m_GTT_HighSpeed);
	Delete::safeDelete(m_RRM);
	Delete::safeDelete(m_RRM_TDM_DRA);
	Delete::safeDelete(m_RRM_TDM_DRA);
	Delete::safeDelete(m_RRM_RR);
	Delete::safeDelete(m_WT);
	Delete::safeDelete(m_TMC);
}


bool VeUE::RRM::isNeedRecalculateSINR(int t_PatternIdx) {
	if (m_InterferenceVeUEIdVec[t_PatternIdx].size() != m_PreInterferenceVeUEIdVec[t_PatternIdx].size()) return true;
	for (int i = 0; i < m_InterferenceVeUEIdVec[t_PatternIdx].size(); i++) {
		if (m_InterferenceVeUEIdVec[t_PatternIdx][i] != m_PreInterferenceVeUEIdVec[t_PatternIdx][i]) return true;
	}
	return false;
}



default_random_engine VeUE::RRM_TDM_DRA::s_Engine((unsigned)time(NULL));


string VeUE::RRM_TDM_DRA::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << m_This->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << m_This->m_GTT->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << m_This->m_GTT->m_ClusterIdx;
	ss << " , ScheduleInterval = [" << left << setw(3) << get<0>(m_ScheduleInterval) << "," << left << setw(3) << get<1>(m_ScheduleInterval) << "] }";
	return ss.str();
}


default_random_engine VeUE::RRM_ICC_DRA::s_Engine((unsigned)time(NULL));

std::string VeUE::RRM_ICC_DRA::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << m_This->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << m_This->m_GTT->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << m_This->m_GTT->m_ClusterIdx << " }";
	return ss.str();
}



std::string VeUE::RRM_RR::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << m_This->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << m_This->m_GTT->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << m_This->m_GTT->m_ClusterIdx << " }";
	return ss.str();
}

