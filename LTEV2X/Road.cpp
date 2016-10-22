#include"Road.h"


cRoad::cRoad() {
	m_GTATUrban = new GTATUrban();
	m_GTATHighSpeed = new GTATHighSpeed();
}

cRoad::~cRoad() {
	delete m_GTATUrban;
	delete m_GTATHighSpeed;
}

void cRoad::initializeUrban(sRoadConfigure &t_RoadConfigure) {
	m_GTATUrban->m_RoadId = t_RoadConfigure.wRoadID;
	m_GTATUrban->m_AbsX = ns_GTAT_Urban::c_roadTopoRatio[m_GTATUrban->m_RoadId * 2 + 0] * ns_GTAT_Urban::c_wide;
	m_GTATUrban->m_AbsY = ns_GTAT_Urban::c_roadTopoRatio[m_GTATUrban->m_RoadId * 2 + 1] * ns_GTAT_Urban::c_length;
	printf("Road£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n", m_GTATUrban->m_AbsX, m_GTATUrban->m_AbsY);

	m_GTATUrban->m_eNBNum = t_RoadConfigure.weNBNum;
	if (m_GTATUrban->m_eNBNum == 1) {
		m_GTATUrban->m_eNB = (eNB *)t_RoadConfigure.peNB + t_RoadConfigure.weNBOffset;
		seNBConfigure eNBConfigure;
		eNBConfigure.sys_config = t_RoadConfigure.sys_config;
		eNBConfigure.wRoadID = m_GTATUrban->m_RoadId;
		eNBConfigure.fX = 42.0f;
		eNBConfigure.fY = -88.5f;
		eNBConfigure.fAbsX = m_GTATUrban->m_AbsX + eNBConfigure.fX;
		eNBConfigure.fAbsY = m_GTATUrban->m_AbsY + eNBConfigure.fY;
		eNBConfigure.weNBID = t_RoadConfigure.weNBOffset;
		m_GTATUrban->m_eNB->initializeUrban(eNBConfigure);
	}
}


void cRoad::initializeHighSpeed(sLaneConfigure &t_RoadHighSpeedConfigure) {
	m_GTATHighSpeed->m_RoadId = t_RoadHighSpeedConfigure.wLaneID;
	m_GTATHighSpeed->m_AbsX = 0.0f;
	m_GTATHighSpeed->m_AbsY = ns_GTAT_HighSpeed::c_laneTopoRatio[m_GTATHighSpeed->m_RoadId * 2 + 1] * ns_GTAT_HighSpeed::c_lane_wide;
}