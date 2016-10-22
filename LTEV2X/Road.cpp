#include"Road.h"


Road::Road() {
	m_GTAT_Urban = new GTAT_Urban();
	m_GTAT_HighSpeed = new GTAT_HighSpeed();
}

Road::~Road() {
	delete m_GTAT_Urban;
	delete m_GTAT_HighSpeed;
}

void Road::initializeUrban(RoadConfigure &t_RoadConfigure) {
	m_GTAT_Urban->m_RoadId = t_RoadConfigure.wRoadID;
	m_GTAT_Urban->m_AbsX = ns_GTAT_Urban::c_roadTopoRatio[m_GTAT_Urban->m_RoadId * 2 + 0] * ns_GTAT_Urban::c_wide;
	m_GTAT_Urban->m_AbsY = ns_GTAT_Urban::c_roadTopoRatio[m_GTAT_Urban->m_RoadId * 2 + 1] * ns_GTAT_Urban::c_length;
	printf("Road£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n", m_GTAT_Urban->m_AbsX, m_GTAT_Urban->m_AbsY);

	m_GTAT_Urban->m_eNBNum = t_RoadConfigure.weNBNum;
	if (m_GTAT_Urban->m_eNBNum == 1) {
		m_GTAT_Urban->m_eNB = (eNB *)t_RoadConfigure.peNB + t_RoadConfigure.weNBOffset;
		eNBConfigure eNBConfigure;
		eNBConfigure.sys_config = t_RoadConfigure.sys_config;
		eNBConfigure.wRoadID = m_GTAT_Urban->m_RoadId;
		eNBConfigure.fX = 42.0f;
		eNBConfigure.fY = -88.5f;
		eNBConfigure.fAbsX = m_GTAT_Urban->m_AbsX + eNBConfigure.fX;
		eNBConfigure.fAbsY = m_GTAT_Urban->m_AbsY + eNBConfigure.fY;
		eNBConfigure.weNBID = t_RoadConfigure.weNBOffset;
		m_GTAT_Urban->m_eNB->initializeUrban(eNBConfigure);
	}
}


void Road::initializeHighSpeed(HighSpeedRodeConfigure &t_RoadHighSpeedConfigure) {
	m_GTAT_HighSpeed->m_RoadId = t_RoadHighSpeedConfigure.wLaneID;
	m_GTAT_HighSpeed->m_AbsX = 0.0f;
	m_GTAT_HighSpeed->m_AbsY = ns_GTAT_HighSpeed::c_laneTopoRatio[m_GTAT_HighSpeed->m_RoadId * 2 + 1] * ns_GTAT_HighSpeed::c_lane_wide;
}