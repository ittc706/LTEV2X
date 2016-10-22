#pragma once

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include"Config.h"


class cLane
{
public:
	void initializeHigh(sLaneConfigure &t_LaneConfigure);
public:
	int m_wLaneID;

	double m_fAbsX;
	double m_fAbsY;
	int  m_upr;
};