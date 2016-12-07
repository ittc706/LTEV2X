/*
* =====================================================================================
*
*       Filename:  GTT_RSU.cpp
*
*    Description:  GTTÄ£¿éÖÐRSUÊÓÍ¼
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
#include<string>
#include"GTT_HighSpeed.h"
#include"GTT_Urban.h"

#include"GTT_RSU.h"

#include"IMTA.h"
#include"Function.h"

using namespace std;

GTT_RSU::~GTT_RSU() {
	Delete::safeDelete(m_IMTA, true);
}

int GTT_RSU::s_RSUCount = 0;

std::string GTT_RSU::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "Road[" << m_RSUId << "]: (" << m_AbsX << "," << m_AbsY << ")" << endl;
	return ss.str();
}


GTT_Urban_RSU::GTT_Urban_RSU() {
	m_AbsX = GTT_Urban::s_RSU_TOPO_RATIO[m_RSUId * 2 + 0] * (GTT_Urban::s_ROAD_LENGTH_SN + 2 * GTT_Urban::s_ROAD_WIDTH);
	m_AbsY = GTT_Urban::s_RSU_TOPO_RATIO[m_RSUId * 2 + 1] * (GTT_Urban::s_ROAD_LENGTH_EW + 2 * GTT_Urban::s_ROAD_WIDTH);
	IMTA::randomUniform(&m_FantennaAngle, 1, 180.0f, -180.0f, false);
	//g_FileLocationInfo << toString(0);

	m_ClusterNum = GTT_Urban::s_RSU_CLUSTER_NUM[m_RSUId];
	m_ClusterVeUEIdList = vector<list<int>>(m_ClusterNum);
	m_ZoneNum = GTT_Urban::s_RSU_ZONE_NUM[m_RSUId];
    m_ZoneVeUEIdList = vector<list<int>>(m_ZoneNum);
}


GTT_HighSpeed_RSU::GTT_HighSpeed_RSU() {
	m_AbsX = GTT_HighSpeed::s_RSU_TOPO_RATIO[m_RSUId * 2 + 0] * 100;
	m_AbsY = GTT_HighSpeed::s_RSU_TOPO_RATIO[m_RSUId * 2 + 1];
	IMTA::randomUniform(&m_FantennaAngle, 1, 180.0f, -180.0f, false);
	m_ClusterNum = GTT_HighSpeed::s_RSU_CLUSTER_NUM;
	m_ClusterVeUEIdList = vector<list<int>>(m_ClusterNum);
}
