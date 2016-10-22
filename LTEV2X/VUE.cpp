#include<vector>
#include<iomanip>
#include<sstream>
#include<math.h>
#include"VUE.h"
#include"RSU.h"

using namespace std;

int VeUE::m_VeUECount = 0;


VeUE::VeUE() {
	m_GTAT = new GTAT();
	m_GTAT_Urban = new GTAT_Urban();
    m_GTAT_HighSpeed = new GTAT_HighSpeed();
}


void VeUE::initializeUrban(VeUEConfigure &t_UEConfigure) {

	m_GTAT_Urban->m_RoadId = t_UEConfigure.wRoadID;
	m_GTAT_Urban->m_LocationId = t_UEConfigure.locationID;
	m_GTAT_Urban->m_X = t_UEConfigure.fX;
	m_GTAT_Urban->m_Y = t_UEConfigure.fY;
	m_GTAT_Urban->m_AbsX = t_UEConfigure.fAbsX;
	m_GTAT_Urban->m_AbsY = t_UEConfigure.fAbsY;
	m_GTAT_Urban->m_V = t_UEConfigure.fv;

	if ((0 < m_GTAT_Urban->m_LocationId) && (m_GTAT_Urban->m_LocationId <= 61))
		m_GTAT_Urban->m_VAngle = 90;
	else if ((61 < m_GTAT_Urban->m_LocationId) && (m_GTAT_Urban->m_LocationId <= 96))
		m_GTAT_Urban->m_VAngle = 0;
	else if ((96 < m_GTAT_Urban->m_LocationId) && (m_GTAT_Urban->m_LocationId <= 157))
		m_GTAT_Urban->m_VAngle = -90;
	else
		m_GTAT_Urban->m_VAngle = -180;

	RandomUniform(&m_GTAT_Urban->m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTAT->m_Nt = 1;
	m_GTAT->m_Nr = 2;
	m_GTAT->m_H = new double[2 * 1024 * 2];

	initializeElse();
}


void VeUE::initializeHighSpeed(VeUEConfigure &t_UEConfigure) {
	m_GTAT_HighSpeed->m_RoadId = t_UEConfigure.wLaneID;
	m_GTAT_HighSpeed->m_X = t_UEConfigure.fX;
	m_GTAT_HighSpeed->m_Y = t_UEConfigure.fY;
	m_GTAT_HighSpeed->m_AbsX = t_UEConfigure.fAbsX;
	m_GTAT_HighSpeed->m_AbsY = t_UEConfigure.fAbsY;
	m_GTAT_HighSpeed->m_V = t_UEConfigure.fv / 3.6f;

	if (m_GTAT_HighSpeed->m_RoadId <= 2)
		m_GTAT_HighSpeed->m_VAngle = 0;
	else
		m_GTAT_HighSpeed->m_VAngle = 180;

	RandomUniform(&m_GTAT_HighSpeed->m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTAT->m_Nt = 1;
	m_GTAT->m_Nr = 2;
	m_GTAT->m_H = new double[2 * 1024 * 2];

	initializeElse();
}


void VeUE::initializeElse() {
	m_RRM = new RRM();
	m_RRM_DRA = new RRM_DRA(this);
	m_RRM_RR = new RRM_RR();
	m_WT = new WT();
	m_TMAC = new TMAC();
}


VeUE::~VeUE() {
	delete m_GTAT;
	delete m_GTAT_Urban;
	delete m_GTAT_HighSpeed;
	delete m_RRM;
	delete m_RRM_DRA;
	delete m_RRM_RR;
	delete m_WT;
	delete m_TMAC;
}


default_random_engine VeUE::RRM_DRA::s_Engine((unsigned)time(NULL));

VeUE::RRM_DRA::RRM_DRA(VeUE* t_this) {
	m_This = t_this;
}

string VeUE::RRM_DRA::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << m_This->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << m_This->m_GTAT->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << m_This->m_GTAT->m_ClusterIdx;
	ss << " , ScheduleInterval = [" << left << setw(3) << get<0>(m_ScheduleInterval) << "," << left << setw(3) << get<1>(m_ScheduleInterval) << "] }";
	return ss.str();
}