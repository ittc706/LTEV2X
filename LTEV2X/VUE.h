#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include<random>
#include<map>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"

class GTT_VeUE;
class RRM_VeUE;

class VeUE {
	
	

	/*---------------ǰ������---------------*/
public:
	class TMC;

	/*------------------��------------------*/
public:

	GTT_VeUE * m_GTT = nullptr;
	RRM_VeUE * m_RRM = nullptr;


	/*
	* TMC�ڲ���ָ��
	* ���ڴ洢��ͬʵ�ֵĹ�ͬ����
	*/
	TMC* m_TMC = nullptr;

	/*------------------����------------------*/
public:
	/*
	* ��������
	* �����ڲ���ָ��
	*/
	~VeUE();
	
	/*
	* TMCģ����ͼ�У�RSU�����ʼ������
	* ��WTģ���ʼ��ʱ����
	*/
	void initializeTMC();

	/*-----------------�ڲ���-----------------*/
public:

	class TMC {
		/*--------��--------*/
	public:
		/*
		* ����λ�ø�����־��Ϣ
		*/
		std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;
	};
};






