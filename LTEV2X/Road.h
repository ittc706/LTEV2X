#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <sstream>
#include "Enumeration.h"
#include "Config.h"
#include "Global.h"
#include "Schedule.h"
#include "eNB.h"


class cRoad{
public:

	void initialize(sRoadConfigure &t_RoadConfigure);

	unsigned short m_wRoadID;
    unsigned short m_weNBNum;
	unsigned short m_weNBID;
	ceNB *m_peNB;
    float m_fAbsX;
	float m_fAbsY;
	int  m_upr;
};