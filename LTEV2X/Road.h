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

	GTATUrban* m_GTATUrban = nullptr;//���ڴ洢���򳡾����ض�����
	GTATHighSpeed* m_GTATHighSpeed = nullptr;//���ڴ洢���ٳ������ض�����

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