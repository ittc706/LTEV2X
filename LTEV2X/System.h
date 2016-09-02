#pragma once
#include <vector>
#include <fstream>
#include "Config.h"
#include "eNB.h"
#include "RSU.h"
#include "VUE.h"
#include "Traffic.h"
#include "Road.h"
#include "RRMBasic.h"
#include "RRM_DRA.h"
#include "RRM_RR.h"

class cSystem{
public:

	/*--------------------------------------------------------------
	*                      ȫ�ֿ��Ƶ�Ԫ
	* -------------------------------------------------------------*/

	/*------------------���ݳ�Ա------------------*/
	int m_TTI;//��ǰ��TTIʱ��
	int m_NTTI;//�����ܹ���TTI
	sConfigure m_Config;//ϵͳ��������
	ceNB* m_eNBAry;//��վ����
	cRoad *m_RoadAry;//��·����
	cRSU* m_RSUAry;//RSU����
	cVeUE* m_VeUEAry;//VeUE����

	/*����ģ���ܿز���*/
	eScheduleMode m_ScheduleMode;//����ģʽѡ��
	RRM_Basic* RRMPoint;

	/*------------------��Ա����------------------*/
public:
	/*�ӿں���*/
	void configure();//ϵͳ�����������
	void initialization();//ϵͳ�������ã����ϵͳ��ʼ��
	void RRMInitialization();
	void process();//ϵͳ��������
	void dispose();//�ڴ�����

	/*--------------------------------------------------------------
	*                      ҵ��ģ������Ƶ�Ԫ
	* -------------------------------------------------------------*/
public:
	/*------------------���ݳ�Ա------------------*/
	std::vector<sEvent> m_EventVec;//�¼�����
	/*
	* ����±�Ϊʱ��ۣ�����TTI��
	* ���¼�������ͬ���¼�����������ͬʱ�̴������¼���Id������ͬ��ʱ�����
	*/
	std::vector<std::list<int>> m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���
	
	std::vector<int> m_VeUEEmergencyNum;//ÿ���������¼������Ĵ�����������֤���ɷֲ��������в����ô�
	/*------------------��Ա����------------------*/
	void buildEventList();
	void processStatistics();
	void writeEventListInfo(std::ofstream &out);//д���¼��б����Ϣ
	void writeEventLogInfo(std::ofstream &out);//д�����¼�����־��Ϣ
	void writeVeUELocationUpdateLogInfo(std::ofstream &out); //д�����λ�ø�����־



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
	
};



