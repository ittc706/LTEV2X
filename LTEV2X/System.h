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
public:
	/*------------------��------------------*/
	int m_TTI;//��ǰ��TTIʱ��
	SystemConfig m_Config;//ϵͳ��������
	eNB* m_eNBAry = nullptr;//��վ����
	Road* m_RoadAry = nullptr;//��·����
	RSU* m_RSUAry = nullptr;//RSU����
	VeUE* m_VeUEAry = nullptr;//VeUE����

	std::vector<Event> m_EventVec;//�¼��������±�����¼�ID
	std::vector<std::list<int>> m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���
	std::vector<std::vector<int>> m_TTIRSUThroughput;//�����ʣ�����±�ΪTTI���ڲ��±�ΪRSUId


	/*---ģ�������---*/
	GTTMode m_GTTMode;//��������ģʽѡ��
	GTT_Basic* m_GTTPoint = nullptr;//���������봫�䵥Ԫ

	WT_Basic* m_WTPoint = nullptr;//���ߴ��䵥Ԫ
	WTMode m_WTMode;

	TMC_Basic* m_TMCPoint = nullptr;//ҵ��ģ������Ƶ�Ԫ

	RRMMode m_RRMMode;//����ģʽѡ��
	RRM_Basic* m_RRMPoint = nullptr;//������Դ����Ԫ


	/*------------------����------------------*/
public:/*---�ӿ�---*/
	void process();//ϵͳ��������
	~System();
private:/*---ʵ��---*/
	void configure();//ϵͳ�����������
	void initialization();//ϵͳ�������ã����ϵͳ��ʼ��
	void initializeGTTModule();//GTTģ������ʼ��
	void initializeWTModule();//WTģ������ʼ��
	void initializeRRMModule();//RRMģ������ʼ��
	void initializeTMCModule();//TMCģ������ʼ��
};



