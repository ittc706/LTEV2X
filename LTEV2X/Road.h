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

	struct GTT_Urban;
	struct GTT_HighSpeed;

	GTT_Urban* m_GTT_Urban = nullptr;//���ڴ洢���򳡾����ض�����
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;//���ڴ洢���ٳ������ض�����

	struct GTT_Urban {
		int m_RoadId;
		int m_eNBNum;
		int m_eNBId;
		eNB *m_eNB;
		double m_AbsX;
		double m_AbsY;
		int  m_upr;
	};

	struct GTT_HighSpeed {
		int m_RoadId;
		double m_AbsX;
		double m_AbsY;
		int  m_upr;
	};
};