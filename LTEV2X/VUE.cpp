#include<vector>
#include<iomanip>
#include<iostream>
#include<sstream>
#include<math.h>
#include"VUE.h"
#include"RSU.h"

using namespace std;

int VeUE::m_VeUECount = 0;


void VeUE::initializeGTT_Urban(VeUEConfigure &t_UEConfigure) {
	m_GTT = new GTT();
	m_GTT_Urban = new GTT_Urban();

	m_GTT_Urban->m_RoadId = t_UEConfigure.roadId;
	m_GTT_Urban->m_LocationId = t_UEConfigure.locationId;
	m_GTT_Urban->m_X = t_UEConfigure.X;
	m_GTT_Urban->m_Y = t_UEConfigure.Y;
	m_GTT_Urban->m_AbsX = t_UEConfigure.AbsX;
	m_GTT_Urban->m_AbsY = t_UEConfigure.AbsY;
	m_GTT_Urban->m_V = t_UEConfigure.V;

	if ((0 < m_GTT_Urban->m_LocationId) && (m_GTT_Urban->m_LocationId <= 61))
		m_GTT_Urban->m_VAngle = 90;
	else if ((61 < m_GTT_Urban->m_LocationId) && (m_GTT_Urban->m_LocationId <= 96))
		m_GTT_Urban->m_VAngle = 0;
	else if ((96 < m_GTT_Urban->m_LocationId) && (m_GTT_Urban->m_LocationId <= 157))
		m_GTT_Urban->m_VAngle = -90;
	else
		m_GTT_Urban->m_VAngle = -180;

	randomUniform(&m_GTT_Urban->m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTT->m_Nt = 1;
	m_GTT->m_Nr = 2;
	m_GTT->m_H = new double[2 * 1024 * 2];

}


void VeUE::initializeGTT_HighSpeed(VeUEConfigure &t_UEConfigure) {
	m_GTT = new GTT();
	m_GTT_HighSpeed = new GTT_HighSpeed();

	m_GTT_HighSpeed->m_RoadId = t_UEConfigure.laneId;
	m_GTT_HighSpeed->m_X = t_UEConfigure.X;
	m_GTT_HighSpeed->m_Y = t_UEConfigure.Y;
	m_GTT_HighSpeed->m_AbsX = t_UEConfigure.AbsX;
	m_GTT_HighSpeed->m_AbsY = t_UEConfigure.AbsY;
	m_GTT_HighSpeed->m_V = t_UEConfigure.V / 3.6f;

	if (m_GTT_HighSpeed->m_RoadId <= 2)
		m_GTT_HighSpeed->m_VAngle = 0;
	else
		m_GTT_HighSpeed->m_VAngle = 180;

	randomUniform(&m_GTT_HighSpeed->m_FantennaAngle, 1, 180.0f, -180.0f, false);

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
	m_RRM->m_WTInfo = vector<tuple<ModulationType,int,double>>(ns_RRM_TDM_DRA::gc_TotalPatternNum, tuple<ModulationType, int, double>(_16QAM,0,0));
	m_RRM->m_isWTCached = vector<bool>(ns_RRM_TDM_DRA::gc_TotalPatternNum, false);

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
	m_RRM->m_WTInfo = vector<tuple<ModulationType, int, double>>(ns_RRM_ICC_DRA::gc_TotalPatternNum, tuple<ModulationType, int, double>(_16QAM, 0, 0));
	m_RRM->m_isWTCached = vector<bool>(ns_RRM_ICC_DRA::gc_TotalPatternNum, false);

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
	m_RRM->m_WTInfo = vector<tuple<ModulationType, int, double>>(ns_RRM_RR::gc_TotalPatternNum, tuple<ModulationType, int, double>(_16QAM, 0, 0));
	m_RRM->m_isWTCached = vector<bool>(ns_RRM_RR::gc_TotalPatternNum, false);

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
	if (m_H != nullptr) {
		delete[] m_H;
		m_H = nullptr;
	}
	for (double*& p : m_InterferenceH) {
		if (p != nullptr) {
			delete[] p;
			p = nullptr;
		}
	}
}


VeUE::GTT_Urban::~GTT_Urban() {
	if (m_IMTA != nullptr) {
		delete[] m_IMTA;
		m_IMTA = nullptr;
	}
}


VeUE::GTT_HighSpeed::~GTT_HighSpeed() {
	if (m_IMTA != nullptr) {
		delete[] m_IMTA;
		m_IMTA = nullptr;
	}
}


VeUE::~VeUE() {
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
	if (m_RRM_ICC_DRA != nullptr) {
		delete m_RRM_TDM_DRA;
		m_RRM_ICC_DRA = nullptr;
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


bool VeUE::RRM::isNeedRecalculateSINR(int patternIdx) {
	if (m_InterferenceVeUEIdVec[patternIdx].size() != m_PreInterferenceVeUEIdVec[patternIdx].size()) return true;
	for (int i = 0; i < m_InterferenceVeUEIdVec[patternIdx].size(); i++) {
		if (m_InterferenceVeUEIdVec[patternIdx][i] != m_PreInterferenceVeUEIdVec[patternIdx][i]) return true;
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

