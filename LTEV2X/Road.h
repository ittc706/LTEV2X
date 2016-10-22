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

	int m_wRoadID;
    int m_weNBNum;
	int m_weNBID;
	ceNB *m_peNB;
    double m_fAbsX;
	double m_fAbsY;
	int  m_upr;
};