#include<vector>
#include<iomanip>
#include<sstream>
#include<math.h>
#include"VUE.h"
#include"RSU.h"

using namespace std;

int cVeUE::m_VeUECount = 0;


cVeUE::cVeUE() {
	m_GTAT = new GTAT();
	m_GTATUrban = new GTATUrban();
    m_GTATHighSpeed = new GTATHighSpeed();
}


void cVeUE::initializeUrban(sUEConfigure &t_UEConfigure) {

	m_GTATUrban->m_RoadId = t_UEConfigure.wRoadID;
	m_GTATUrban->m_LocationId = t_UEConfigure.locationID;
	m_GTATUrban->m_X = t_UEConfigure.fX;
	m_GTATUrban->m_Y = t_UEConfigure.fY;
	m_GTATUrban->m_AbsX = t_UEConfigure.fAbsX;
	m_GTATUrban->m_AbsY = t_UEConfigure.fAbsY;
	m_GTATUrban->m_V = t_UEConfigure.fv;

	if ((0 < m_GTATUrban->m_LocationId) && (m_GTATUrban->m_LocationId <= 61))
		m_GTATUrban->m_VAngle = 90;
	else if ((61 < m_GTATUrban->m_LocationId) && (m_GTATUrban->m_LocationId <= 96))
		m_GTATUrban->m_VAngle = 0;
	else if ((96 < m_GTATUrban->m_LocationId) && (m_GTATUrban->m_LocationId <= 157))
		m_GTATUrban->m_VAngle = -90;
	else
		m_GTATUrban->m_VAngle = -180;

	RandomUniform(&m_GTATUrban->m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTAT->m_Nt = 1;
	m_GTAT->m_Nr = 2;
	m_GTAT->m_H = new double[2 * 1024 * 2];

	initializeElse();
}


void cVeUE::initializeHighSpeed(sUEConfigure &t_UEConfigure) {
	m_GTATHighSpeed->m_RoadId = t_UEConfigure.wLaneID;
	m_GTATHighSpeed->m_X = t_UEConfigure.fX;
	m_GTATHighSpeed->m_Y = t_UEConfigure.fY;
	m_GTATHighSpeed->m_AbsX = t_UEConfigure.fAbsX;
	m_GTATHighSpeed->m_AbsY = t_UEConfigure.fAbsY;
	m_GTATHighSpeed->m_V = t_UEConfigure.fv / 3.6f;

	if (m_GTATHighSpeed->m_RoadId <= 2)
		m_GTATHighSpeed->m_VAngle = 0;
	else
		m_GTATHighSpeed->m_VAngle = 180;

	RandomUniform(&m_GTATHighSpeed->m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTAT->m_Nt = 1;
	m_GTAT->m_Nr = 2;
	m_GTAT->m_H = new double[2 * 1024 * 2];

	initializeElse();
}


void cVeUE::initializeElse() {
	m_RRM = new RRM();
	m_RRMDRA = new RRMDRA(this);
	m_RRMRR = new RRMRR();
	m_WT = new WT();
	m_TMAC = new TMAC();
}


cVeUE::~cVeUE() {
	delete m_GTAT;
	delete m_GTATUrban;
	delete m_GTATHighSpeed;
	delete m_RRM;
	delete m_RRMDRA;
	delete m_RRMRR;
	delete m_WT;
	delete m_TMAC;
}


default_random_engine cVeUE::RRMDRA::s_Engine((unsigned)time(NULL));

cVeUE::RRMDRA::RRMDRA(cVeUE* t_this) {
	m_This = t_this;
}

string cVeUE::RRMDRA::toString(int n) {
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