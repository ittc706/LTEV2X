#include"Road.h"


void cRoad::initializeUrban(sRoadConfigure &t_RoadConfigure) {
	m_wRoadID = t_RoadConfigure.wRoadID;
	m_fAbsX = ns_GTAT_Urban::c_roadTopoRatio[m_wRoadID * 2 + 0] * ns_GTAT_Urban::c_wide;
	m_fAbsY = ns_GTAT_Urban::c_roadTopoRatio[m_wRoadID * 2 + 1] * ns_GTAT_Urban::c_length;
	printf("Road£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n", m_fAbsX, m_fAbsY);

	m_weNBNum = t_RoadConfigure.weNBNum;
	if (m_weNBNum == 1) {
		m_peNB = (ceNB *)t_RoadConfigure.peNB + t_RoadConfigure.weNBOffset;
		seNBConfigure eNBConfigure;
		eNBConfigure.sys_config = t_RoadConfigure.sys_config;
		eNBConfigure.wRoadID = m_wRoadID;
		eNBConfigure.fX = 42.0f;
		eNBConfigure.fY = -88.5f;
		eNBConfigure.fAbsX = m_fAbsX + eNBConfigure.fX;
		eNBConfigure.fAbsY = m_fAbsY + eNBConfigure.fY;
		eNBConfigure.weNBID = t_RoadConfigure.weNBOffset;
		m_peNB->initializeUrban(eNBConfigure);
	}
}

