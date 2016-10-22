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

	void initializeUrban(RoadConfigure &t_RoadConfigure);
	void initializeHighSpeed(HighSpeedRodeConfigure &t_LaneConfigure);

	struct GTAT_Urban;
	struct GTAT_HighSpeed;

	GTAT_Urban* m_GTAT_Urban = nullptr;//���ڴ洢���򳡾����ض�����
	GTAT_HighSpeed* m_GTAT_HighSpeed = nullptr;//���ڴ洢���ٳ������ض�����

	struct GTAT_Urban {
		int m_RoadId;
		int m_eNBNum;
		int m_eNBId;
		eNB *m_eNB;
		double m_AbsX;
		double m_AbsY;
		int  m_upr;
	};

	struct GTAT_HighSpeed {
		int m_RoadId;
		double m_AbsX;
		double m_AbsY;
		int  m_upr;
	};
};