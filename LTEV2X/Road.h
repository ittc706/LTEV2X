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

	void initializeUrban(UrbanRoadConfig &t_RoadConfig);
	void initializeHighSpeed(HighSpeedRodeConfig &t_RoadHighSpeedConfig);

	class GTT;
	class GTT_Urban;
	class GTT_HighSpeed;

	GTT* m_GTT = nullptr;
	GTT_Urban* m_GTT_Urban = nullptr;//���ڴ洢���򳡾����ض�����
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;//���ڴ洢���ٳ������ض�����

	
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