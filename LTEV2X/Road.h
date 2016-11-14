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


class Road{
public:
	Road();
	~Road();

	void initializeUrban(UrbanRoadConfigure &t_RoadConfigure);
	void initializeHighSpeed(HighSpeedRodeConfigure &t_LaneConfigure);

	class GTT;
	class GTT_Urban;
	class GTT_HighSpeed;

	GTT* m_GTT = nullptr;
	GTT_Urban* m_GTT_Urban = nullptr;//用于存储城镇场景的特定参数
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;//用于存储高速场景的特定参数

	
	class GTT{
	public:
		int m_RoadId;
		double m_AbsX;
		double m_AbsY;
		int  m_upr;

		std::string toString(int t_NumTab);
	};

	class GTT_Urban {
	public:
		int m_eNBNum;
		int m_eNBId;
		eNB *m_eNB;
	};

	class GTT_HighSpeed {
	public:

	};
};