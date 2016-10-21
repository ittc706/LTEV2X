#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Event.h"
#include"Global.h"


using namespace std;


void cRSU::initialize(sRSUConfigure &t_RSUConfigure){
	m_RSUId = t_RSUConfigure.wRSUID;
	m_fAbsX = ns_GTAT_Urban::c_RSUTopoRatio[m_RSUId * 2 + 0] * ns_GTAT_Urban::c_wide;
	m_fAbsY = ns_GTAT_Urban::c_RSUTopoRatio[m_RSUId * 2 + 1] * ns_GTAT_Urban::c_length;
	RandomUniform(&m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU£º");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_RSUId, m_fAbsX, m_fAbsY);

	m_DRAClusterNum = ns_GTAT_Urban::c_RSUClusterNum[m_RSUId];
	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);
}
