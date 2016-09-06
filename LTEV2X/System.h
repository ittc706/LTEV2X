#pragma once
#include<vector>
#include<fstream>
#include"Config.h"
#include"eNB.h"
#include"RSU.h"
#include"VUE.h"
#include"Event.h"
#include"Road.h"
#include"RRM.h"
#include"RRM_DRA.h"
#include"RRM_RR.h"
#include"TMAC.h"
#include"TMAC_B.h"

class cSystem{
public:

	/*--------------------------------------------------------------
	*                      ȫ�ֿ��Ƶ�Ԫ
	* -------------------------------------------------------------*/

	/*------------------���ݳ�Ա------------------*/
	int m_TTI;//��ǰ��TTIʱ��
	sConfigure m_Config;//ϵͳ��������
	ceNB* m_eNBAry;//��վ����
	cRoad *m_RoadAry;//��·����
	cRSU* m_RSUAry;//RSU����
	cVeUE* m_VeUEAry;//VeUE����

	std::vector<sEvent> m_EventVec;//�¼��������±�����¼�ID
	std::vector<std::list<int>> m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���
	std::vector<std::vector<int>> m_TTIRSUThroughput;//�����ʣ�����±�ΪTTI���ڲ��±�ΪRSUId


	/*-----------------ģ�������-----------------*/
	//ҵ��ģ������Ƶ�Ԫ
	TMAC_Basic* TMACPoint;

	//������Դ����Ԫ
	eScheduleMode m_ScheduleMode;//����ģʽѡ��
	RRM_Basic* RRMPoint;

	/*------------------��Ա����------------------*/

	void configure();//ϵͳ�����������
	void initialization();//ϵͳ�������ã����ϵͳ��ʼ��
	void moduleControlInitialization();//ģ������ʼ��
	void process();//ϵͳ��������
	void dispose();//�ڴ�����


	

	/*--------------------------------------------------------------
	*                      �������˵�Ԫ
	* -------------------------------------------------------------*/
public:
	/*------------------���ݳ�Ա------------------*/
	int m_AllUsers;
	int m_FreshNum;
	/*------------------��Ա����------------------*/

	void channelGeneration();//�ŵ�������ˢ��
	void freshLoc(void);
	void writeVeUELocationUpdateLogInfo(std::ofstream &out); //д�����λ�ø�����־
	
};



