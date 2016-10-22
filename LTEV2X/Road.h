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
	cRoad();
	~cRoad();

	void initializeUrban(sRoadConfigure &t_RoadConfigure);
	void initializeHighSpeed(sLaneConfigure &t_LaneConfigure);

	struct GTATUrban;
	struct GTATHighSpeed;

	GTATUrban* m_GTATUrban = nullptr;//用于存储城镇场景的特定参数
	GTATHighSpeed* m_GTATHighSpeed = nullptr;//用于存储高速场景的特定参数

	struct GTATUrban {
		int m_RoadId;
		int m_eNBNum;
		int m_eNBId;
		eNB *m_eNB;
		double m_AbsX;
		double m_AbsY;
		int  m_upr;
	};

	struct GTATHighSpeed {
		int m_RoadId;
		double m_AbsX;
		double m_AbsY;
		int  m_upr;
	};
};