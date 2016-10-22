#pragma once

#include<iostream>
#include<fstream>
#include<string>
#include<list>
#include<sstream>
#include"Enumeration.h"
#include"Config.h"
#include"Global.h"
#include"eNB.h"


class cRoad{
public:

	void initializeUrban(sRoadConfigure &t_RoadConfigure);

	unsigned short m_wRoadID;
    unsigned short m_weNBNum;
	unsigned short m_weNBID;
	ceNB *m_peNB;
    double m_fAbsX;
	double m_fAbsY;
	int  m_upr;
};