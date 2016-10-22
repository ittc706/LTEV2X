#include<vector>
#include<iomanip>
#include<math.h>
#include"VUE.h"
#include"RSU.h"

using namespace std;

int cVeUE::m_VeUECount = 0;


cVeUE::cVeUE() {
	m_GTAT = new GTAT();
	m_GTATUrban = new GTATUrban();
    m_GTATHigh = new GTATHigh();
	m_RRMDRA = new RRMDRA();
	m_RRMRR = new RRMRR();
	m_WT = new WT();
	m_TMAC = new TMAC();
}


void cVeUE::initializeUrban(sUEConfigure &t_UEConfigure) {

	m_GTATUrban->m_wRoadID = t_UEConfigure.wRoadID;
	m_GTATUrban->m_locationID = t_UEConfigure.locationID;
	m_GTATUrban->m_fX = t_UEConfigure.fX;
	m_GTATUrban->m_fY = t_UEConfigure.fY;
	m_GTATUrban->m_fAbsX = t_UEConfigure.fAbsX;
	m_GTATUrban->m_fAbsY = t_UEConfigure.fAbsY;
	m_GTATUrban->m_fv = t_UEConfigure.fv;

	if ((0 < m_GTATUrban->m_locationID) && (m_GTATUrban->m_locationID <= 61))
		m_GTATUrban->m_fvAngle = 90;
	else if ((61 < m_GTATUrban->m_locationID) && (m_GTATUrban->m_locationID <= 96))
		m_GTATUrban->m_fvAngle = 0;
	else if ((96 < m_GTATUrban->m_locationID) && (m_GTATUrban->m_locationID <= 157))
		m_GTATUrban->m_fvAngle = -90;
	else
		m_GTATUrban->m_fvAngle = -180;

	RandomUniform(&m_GTATUrban->m_fantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTAT->m_Nt = 1;
	m_GTAT->m_Nr = 2;
	m_PreModulation = 4;
	m_GTAT->m_H = new double[2 * 1024 * 2];
}


void cVeUE::initializeHigh(sUEConfigure &t_UEConfigure) {
	m_GTATHigh->m_wLaneID = t_UEConfigure.wLaneID;
	m_GTATHigh->m_fX = t_UEConfigure.fX;
	m_GTATHigh->m_fY = t_UEConfigure.fY;
	m_GTATHigh->m_fAbsX = t_UEConfigure.fAbsX;
	m_GTATHigh->m_fAbsY = t_UEConfigure.fAbsY;
	m_GTATHigh->m_fv = t_UEConfigure.fv / 3.6f;

	if (m_GTATHigh->m_wLaneID <= 2)
		m_GTATHigh->m_fvAngle = 0;
	else
		m_GTATHigh->m_fvAngle = 180;

	RandomUniform(&m_GTATHigh->m_fantennaAngle, 1, 180.0f, -180.0f, false);

	m_GTAT->m_Nt = 1;
	m_GTAT->m_Nr = 2;
	m_PreModulation = 4;
	m_GTAT->m_H = new double[2 * 1024 * 2];
}


cVeUE::~cVeUE() {
	delete m_GTAT;
	delete m_GTATUrban;
	delete m_GTATHigh;
	delete m_RRM;
	delete m_RRMDRA;
	delete m_RRMRR;
	delete m_WT;
	delete m_TMAC;
}
