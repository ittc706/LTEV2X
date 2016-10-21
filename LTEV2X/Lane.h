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
	unsigned short m_wLaneID;

	float m_fAbsX;
	float m_fAbsY;
	int  m_upr;
};