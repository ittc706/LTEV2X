#pragma once
#include<vector>
#include<fstream>
#include"ConfigLoader.h"
#include"Config.h"
#include"eNB.h"
#include"RSU.h"
#include"VUE.h"
#include"Event.h"
#include"Road.h"
#include"GTT.h"
#include"GTT_Urban.h"
#include"GTT_HighSpeed.h"
#include"WT.h"
#include"WT_B.h"
#include"RRM.h"
#include"RRM_TDM_DRA.h"
#include"RRM_ICC_DRA.h"
#include"RRM_RR.h"
#include"TMC.h"
#include"TMC_B.h"

class System{
	/*------------------��------------------*/
public:
	/*
	* ϵͳ��ǰ��TTIʱ��
	*/
	int m_TTI;

	/*
	* ϵͳ��������
	*/
	SystemConfig m_Config;

	/*
	* �ĸ�ʵ��������
	* �ֱ��ǻ�վ����·��RSU������
	*/
	eNB* m_eNBAry = nullptr;
	Road* m_RoadAry = nullptr;
	RSU* m_RSUAry = nullptr;
	VeUE* m_VeUEAry = nullptr;

	/*
	* �¼��������±�����¼�ID
	*/
	std::vector<Event> m_EventVec;

	/*
	* ��TTIΪ�±���¼�����
	* �¼���������m_EventTTIList[i]�����i��TTI���¼���
	*/
	std::vector<std::list<int>> m_EventTTIList;

	/*
	* ������
	* ����±�ΪTTI���ڲ��±�ΪRSUId
	*/
	std::vector<std::vector<int>> m_TTIRSUThroughput;


	/*
	* ģ�������
	* GTTģ�飬RRMģ�飬WTģ�飬TMCģ��
	*/
	GTT_Basic* m_GTTPoint = nullptr;
	RRM_Basic* m_RRMPoint = nullptr;
	TMC_Basic* m_TMCPoint = nullptr;
	WT_Basic* m_WTPoint = nullptr;

	/*
	* ģ��ʵ�ֵľ������
	*/
	GTTMode m_GTTMode;
	WTMode m_WTMode;
	RRMMode m_RRMMode;


	/*------------------����------------------*/
public:
	/*
	* ϵͳ���������ܿ�
	*/	
	void process();

	/*
	* �����������������ʵ�����������
	*/
	~System();
private:
	/*
	* ϵͳ�����������
	*/
	void configure();

	/*
	* ϵͳ�������ã����ϵͳ��ʼ��
	*/
	void initialization();

	/*
	* GTTģ������ʼ��
	* ��initialization()����
	*/
	void initializeGTTModule();

	/*
	* WTģ������ʼ��
	* ��initialization()����
	*/
	void initializeWTModule();

	/*
	* RRMģ������ʼ��
	* ��initialization()����
	*/
	void initializeRRMModule();

	/*
	* TMCģ������ʼ��
	* ��initialization()����
	*/
	void initializeTMCModule();
};



