#include"Road.h"


Road::Road() {
	m_GTAT_Urban = new GTAT_Urban();
	m_GTAT_HighSpeed = new GTAT_HighSpeed();
}

Road::~Road() {
	if (m_GTAT_Urban != nullptr) {
		delete m_GTAT_Urban;
		m_GTAT_Urban = nullptr;
	}
	if (m_GTAT_HighSpeed != nullptr) {
		delete m_GTAT_HighSpeed;
		m_GTAT_HighSpeed = nullptr;
	}
}

void Road::initializeUrban(UrbanRoadConfigure &t_RoadConfigure) {
	m_GTAT_Urban->m_RoadId = t_RoadConfigure.roadId;
	m_GTAT_Urban->m_AbsX = ns_GTAT_Urban::gc_RoadTopoRatio[m_GTAT_Urban->m_RoadId * 2 + 0] * ns_GTAT_Urban::gc_Width;
	m_GTAT_Urban->m_AbsY = ns_GTAT_Urban::gc_RoadTopoRatio[m_GTAT_Urban->m_RoadId * 2 + 1] * ns_GTAT_Urban::gc_Length;
	printf("Road£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n", m_GTAT_Urban->m_AbsX, m_GTAT_Urban->m_AbsY);

	m_GTAT_Urban->m_eNBNum = t_RoadConfigure.eNBNum;
	if (m_GTAT_Urban->m_eNBNum == 1) {
		m_GTAT_Urban->m_eNB = (eNB *)t_RoadConfigure.peNB + t_RoadConfigure.eNBOffset;
		eNBConfigure eNBConfigure;
		eNBConfigure.systemConfig = t_RoadConfigure.systemConfig;
		eNBConfigure.roadId = m_GTAT_Urban->m_RoadId;
		eNBConfigure.X = 42.0f;
		eNBConfigure.Y = -88.5f;
		eNBConfigure.AbsX = m_GTAT_Urban->m_AbsX + eNBConfigure.X;
		eNBConfigure.AbsY = m_GTAT_Urban->m_AbsY + eNBConfigure.Y;
		eNBConfigure.eNBId = t_RoadConfigure.eNBOffset;
		m_GTAT_Urban->m_eNB->initializeUrban(eNBConfigure);
	}
}


void Road::initializeHighSpeed(HighSpeedRodeConfigure &t_RoadHighSpeedConfigure) {
	m_GTAT_HighSpeed->m_RoadId = t_RoadHighSpeedConfigure.roadId;
	m_GTAT_HighSpeed->m_AbsX = 0.0f;
	m_GTAT_HighSpeed->m_AbsY = ns_GTAT_HighSpeed::gc_LaneTopoRatio[m_GTAT_HighSpeed->m_RoadId * 2 + 1] * ns_GTAT_HighSpeed::gc_LaneWidth;
}