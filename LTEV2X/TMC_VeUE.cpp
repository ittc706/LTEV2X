/*
* =====================================================================================
*
*       Filename:  TMC_VeUE.cpp
*
*    Description:  TMCģ����VeUE��ͼ
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

	//����ǰ����һ����ʼ���Ĵ����¼���������С��ӵ���ȼ����趨��
	int initializeTrigger = u(s_Engine);

	//��ʼ������ʱ��
	for (int congestionLevel = 0; congestionLevel < TMC::s_CONGESTION_LEVEL_NUM; congestionLevel++)
		m_NextPeriodEventTriggerTTI[congestionLevel] = initializeTrigger;
}