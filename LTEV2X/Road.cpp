#include"Road.h"
#include"Function.h"

using namespace std;

Road::Road() {
	m_GTT = new GTT();
	m_GTT_Urban = new GTT_Urban();
	m_GTT_HighSpeed = new GTT_HighSpeed();
}

Road::~Road() {
	Delete::safeDelete(m_GTT);
	Delete::safeDelete(m_GTT_Urban);
	Delete::safeDelete(m_GTT_HighSpeed);
}

void Road::initializeUrban(UrbanRoadConfigure &t_RoadConfigure) {
	m_GTT->m_RoadId = t_RoadConfigure.roadId;
	m_GTT->m_AbsX = ns_GTT_Urban::gc_RoadTopoRatio[m_GTT->m_RoadId * 2 + 0] * ns_GTT_Urban::gc_Width;
	m_GTT->m_AbsY = ns_GTT_Urban::gc_RoadTopoRatio[m_GTT->m_RoadId * 2 + 1] * ns_GTT_Urban::gc_Length;
	g_FileLocationInfo << m_GTT->toString(0);

	m_GTT_Urban->m_eNBNum = t_RoadConfigure.eNBNum;
	if (m_GTT_Urban->m_eNBNum == 1) {
		m_GTT_Urban->m_eNB = (eNB *)t_RoadConfigure.eNB + t_RoadConfigure.eNBOffset;
		eNBConfigure eNBConfigure;
		eNBConfigure.systemConfig = t_RoadConfigure.systemConfig;
		eNBConfigure.roadId = m_GTT->m_RoadId;
		eNBConfigure.X = 42.0f;
		eNBConfigure.Y = -88.5f;
		eNBConfigure.AbsX = m_GTT->m_AbsX + eNBConfigure.X;
		eNBConfigure.AbsY = m_GTT->m_AbsY + eNBConfigure.Y;
		eNBConfigure.eNBId = t_RoadConfigure.eNBOffset;
		m_GTT_Urban->m_eNB->initializeUrban(eNBConfigure);
	}
}


void Road::initializeHighSpeed(HighSpeedRodeConfigure &t_RoadHighSpeedConfigure) {
	m_GTT->m_RoadId = t_RoadHighSpeedConfigure.roadId;
	m_GTT->m_AbsX = 0.0f;
	m_GTT->m_AbsY = ns_GTT_HighSpeed::gc_LaneTopoRatio[m_GTT->m_RoadId * 2 + 1] * ns_GTT_HighSpeed::gc_LaneWidth;
}


std::string Road::GTT::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "Road[" << m_RoadId << "]: (" << m_AbsX << "," << m_AbsY << ")" << endl;
	return ss.str();
}