#pragma once
#include<vector>
#include<fstream>
#include"Config.h"
#include"eNB.h"
#include"RSU.h"
#include"VUE.h"
#include"Event.h"
#include"Road.h"
#include"GTAT.h"
#include"GTAT_Urban.h"
#include"GTAT_HighSpeed.h"
#include"WT.h"
#include"WT_B.h"
#include"RRM.h"
#include"RRM_DRA.h"
#include"RRM_RR.h"
#include"TMAC.h"
#include"TMAC_B.h"

class System{
public:
	/*------------------���ݳ�Ա------------------*/
	int m_TTI;//��ǰ��TTIʱ��
	Configure m_Config;//ϵͳ��������
	eNB* m_eNBAry;//��վ����
	Road* m_RoadAry;//��·����
	RSU* m_RSUAry;//RSU����
	VeUE* m_VeUEAry;//VeUE����

	std::vector<Event> m_EventVec;//�¼��������±�����¼�ID
	std::vector<std::list<int>> m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���
	std::vector<std::vector<int>> m_TTIRSUThroughput;//�����ʣ�����±�ΪTTI���ڲ��±�ΪRSUId


	/*-----------------ģ�������-----------------*/
	//���������봫�䵥Ԫ
	GTATMode m_GTATMode;//��������ģʽѡ��
	GTAT_Basic* m_GTATPoint;

    //���ߴ��䵥Ԫ
	WT_Basic* m_WTPoint;

	//ҵ��ģ������Ƶ�Ԫ
	TMAC_Basic* m_TMACPoint;

	//������Դ����Ԫ
	RRMMode m_RRMMode;//����ģʽѡ��
	RRM_Basic* m_RRMPoint;


	/*------------------��Ա����------------------*/
	void process();//ϵͳ��������

private:
	void configure();//ϵͳ�����������
	void initialization();//ϵͳ�������ã����ϵͳ��ʼ��
	void GTATModuleInitialize();//GTATģ������ʼ��
	void WTModuleInitialize();//WTģ������ʼ��
	void RRMModuleInitialize();//RRMģ������ʼ��
	void TMACModuleInitialize();//TMACģ������ʼ��
	void dispose();//�ڴ�����
};



