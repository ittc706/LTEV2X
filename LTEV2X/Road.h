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
	/*---------------ǰ������---------------*/
public:
	class GTT;
	class GTT_Urban;
	class GTT_HighSpeed;
	/*------------------��------------------*/
public:
	/*
	* GTT�ڲ���ָ��
	* ���ڴ洢�������������Ĳ���
	*/
	GTT* m_GTT = nullptr;

	/*
	* GTT_Urban�ڲ���ָ��
	* ���ڴ洢���򳡾����ض�����
	*/
	GTT_Urban* m_GTT_Urban = nullptr;

	/*
	* GTT_HighSpeed�ڲ���ָ��
	* ���ڴ洢���ٳ������ض�����
	*/
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯��
	* ��ʼ���ڲ���ָ��
	*/
	Road();

	/*
	* Ĭ�Ϲ��캯��
	* �����ڲ���ָ��
	*/
	~Road();

	/*
	* ���򳡾��£���·�����ʼ������
	* ��GTTģ���ʼ��ʱ����
	*/
	void initializeUrban(UrbanRoadConfig &t_RoadConfig);

	/*
	* ���ٳ����£���·�����ʼ������
	* ��GTTģ���ʼ��ʱ����
	*/
	void initializeHighSpeed(HighSpeedRodeConfig &t_RoadHighSpeedConfig);

	/*-----------------�ڲ���-----------------*/
	class GTT{
		/*--------��--------*/
	public:
		/*
		* ��·Id
		*/
		int m_RoadId;

		/*
		* ���Ժ�������������
		*/
		double m_AbsX;
		double m_AbsY;

		/*
		* <?>
		*/
		int  m_upr;

		/*-------����-------*/
	public:
		/*
		* ���ɸ�ʽ���ַ���
		*/
		std::string toString(int t_NumTab);
	};

	class GTT_Urban {
		/*--------��--------*/
	public:
		/*
		* ��վ��Ŀ
		*/
		int m_eNBNum;

		/*
		* ��վId
		*/
		int m_eNBId;

		/*
		* ����ָ�������ڻ�վ�����ָ��
		*/
		eNB *m_eNB;
	};

	class GTT_HighSpeed {
	public:

	};
};