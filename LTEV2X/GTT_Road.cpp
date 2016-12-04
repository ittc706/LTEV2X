/*
* =====================================================================================
*
*       Filename:  GTT_Road.cpp
*
*    Description:  GTT模块中Road视图
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

#include<sstream>
#include"GTT_HighSpeed.h"
#include"GTT_Urban.h"
#include"GTT_Road.h"

using namespace std;

std::string GTT_Road::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "Road[" << m_RoadId << "]: (" << m_AbsX << "," << m_AbsY << ")" << endl;
	return ss.str();
}


GTT_Urban_Road::GTT_Urban_Road(UrbanRoadConfig &t_RoadConfig) {
	m_RoadId = t_RoadConfig.roadId;
	m_AbsX = GTT_Urban::s_ROAD_TOPO_RATIO[m_RoadId * 2 + 0] * (GTT_Urban::s_ROAD_LENGTH_SN + 2 * GTT_Urban::s_ROAD_WIDTH);
	m_AbsY = GTT_Urban::s_ROAD_TOPO_RATIO[m_RoadId * 2 + 1] * (GTT_Urban::s_ROAD_LENGTH_EW + 2 * GTT_Urban::s_ROAD_WIDTH);

	m_eNBNum = t_RoadConfig.eNBNum;
	if (m_eNBNum == 1) {
		/*
		* 这里比较绕，解释一下
		* 由于t_RoadConfig.eNB存的是一个(GTT_eNB**)类型，双重指针
		* 第一重指针指向数组
		*/
		m_eNB = *((GTT_eNB **)t_RoadConfig.eNB + t_RoadConfig.eNBOffset);
		eNBConfig eNBConfig;
		eNBConfig.roadId = m_RoadId;
		eNBConfig.X = 42.0f;
		eNBConfig.Y = -88.5f;
		eNBConfig.AbsX = m_AbsX + eNBConfig.X;
		eNBConfig.AbsY = m_AbsY + eNBConfig.Y;
		eNBConfig.eNBId = t_RoadConfig.eNBOffset;
		m_eNB->initialize(eNBConfig);
	}
}


GTT_HighSpeed_Road::GTT_HighSpeed_Road(HighSpeedRodeConfig &t_RoadHighSpeedConfig) {
	m_RoadId = t_RoadHighSpeedConfig.roadId;
	m_AbsX = 0.0f;
	m_AbsY = GTT_HighSpeed::s_ROAD_TOPO_RATIO[m_RoadId * 2 + 1] * GTT_HighSpeed::s_ROAD_WIDTH;
}
