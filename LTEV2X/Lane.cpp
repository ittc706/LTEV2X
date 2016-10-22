#include"Lane.h"
#include"Global.h"

using namespace ns_GTAT_High;

void cLane::initializeHigh(sLaneConfigure &t_RoadConfigure)
{
	m_wLaneID = t_RoadConfigure.wLaneID;
	m_fAbsX = 0.0f;
	m_fAbsY = c_laneTopoRatio[m_wLaneID * 2 + 1] * c_lane_wide;
	//printf("Road£º");
	//printf("m_fAbsX=%f,m_fAbsY=%f\n", m_fAbsX, m_fAbsY);
}
