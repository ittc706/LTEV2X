/*
* =====================================================================================
*
*       Filename:  Road.cpp
*
*    Description:  TMCÄ£¿é
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  LK,WYB
*            LIB:  ITTC
*
* =====================================================================================
*/

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

void Road::initializeUrban(UrbanRoadConfig &t_RoadConfig) {
	m_GTT->m_RoadId = t_RoadConfig.roadId;
	m_GTT->m_AbsX = ns_GTT_Urban::gc_RoadTopoRatio[m_GTT->m_RoadId * 2 + 0] * ns_GTT_Urban::gc_Width;
	m_GTT->m_AbsY = ns_GTT_Urban::gc_RoadTopoRatio[m_GTT->m_RoadId * 2 + 1] * ns_GTT_Urban::gc_Length;
	g_FileLocationInfo << m_GTT->toString(0);

	m_GTT_Urban->m_eNBNum = t_RoadConfig.eNBNum;
	if (m_GTT_Urban->m_eNBNum == 1) {
		m_GTT_Urban->m_eNB = (eNB *)t_RoadConfig.eNB + t_RoadConfig.eNBOffset;
		eNBConfig eNBConfig;
		eNBConfig.systemConfig = t_RoadConfig.systemConfig;
		eNBConfig.roadId = m_GTT->m_RoadId;
		eNBConfig.X = 42.0f;
		eNBConfig.Y = -88.5f;
		eNBConfig.AbsX = m_GTT->m_AbsX + eNBConfig.X;
		eNBConfig.AbsY = m_GTT->m_AbsY + eNBConfig.Y;
		eNBConfig.eNBId = t_RoadConfig.eNBOffset;
		m_GTT_Urban->m_eNB->initializeUrban(eNBConfig);
	}
}


void Road::initializeHighSpeed(HighSpeedRodeConfig &t_RoadHighSpeedConfig) {
	m_GTT->m_RoadId = t_RoadHighSpeedConfig.roadId;
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