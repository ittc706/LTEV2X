#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Traffic.h"
#include"Global.h"


using namespace std;

int newCount = 0;//记录动态创建的对象的次数

int deleteCount = 0;//记录删除动态创建对象的次数

void cRSU::initialize(sRSUConfigure &t_RSUConfigure){
	m_RSUId = t_RSUConfigure.wRSUID;
	m_fAbsX = c_RSUTopoRatio[m_RSUId * 2 + 0] * c_wide;
	m_fAbsY = c_RSUTopoRatio[m_RSUId * 2 + 1] * c_length;
	RandomUniform(&m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU：");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_RSUId, m_fAbsX, m_fAbsY);

	m_DRAClusterNum = c_RSUClusterNum[m_RSUId];
	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);

}
