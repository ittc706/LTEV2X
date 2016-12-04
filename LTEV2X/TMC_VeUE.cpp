/*
* =====================================================================================
*
*       Filename:  TMC_VeUE.cpp
*
*    Description:  TMC模块中VeUE视图
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<iomanip>
#include"TMC.h"

#include"TMC_VeUE.h"

using namespace std;

default_random_engine TMC_VeUE::s_Engine((unsigned)time(NULL));

TMC_VeUE::TMC_VeUE() {
	m_NextPeriodEventTriggerTTI = vector<int>(TMC::s_CONGESTION_LEVEL_NUM);

	uniform_int_distribution<int> u(0, TMC::s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL[0]);

	//给当前车辆一个初始化的触发事件，就以最小的拥塞等级来设定吧
	int initializeTrigger = u(s_Engine);

	//初始化触发时刻
	for (int congestionLevel = 0; congestionLevel < TMC::s_CONGESTION_LEVEL_NUM; congestionLevel++)
		m_NextPeriodEventTriggerTTI[congestionLevel] = initializeTrigger;
}