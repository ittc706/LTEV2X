#pragma once
#include<vector>
#include<fstream>
#include"Schedule.h"
#include"Config.h"
#include"eNB.h"
#include"RSU.h"
#include"VUE.h"
#include"Traffic.h"
#include"Road.h"


class cSystem{
public:

	/*--------------------------------------------------------------
	*                      ȫ�ֿ��Ƶ�Ԫ
	* -------------------------------------------------------------*/

	/*------------------���ݳ�Ա------------------*/
	sConfigure m_Config;//ϵͳ��������
	int m_TTI;//��ǰ��TTIʱ��
	int m_NTTI;//�����ܹ���TTI
	ceNB* m_eNBAry;//��վ����
	cRoad *m_RoadAry;//��·����
	cRSU* m_RSUAry;//RSU����
	cVeUE* m_VeUEAry;//VeUE����
	/*------------------��Ա����------------------*/
public:
	/*�ӿں���*/
	void configure();//ϵͳ�����������
	void initialization();//ϵͳ�������ã����ϵͳ��ʼ��
	void destroy();//�ͷ���Դ��UNDONE
	void process();//ϵͳ��������

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
	

	/*--------------------------------------------------------------
	*                      ������Դ����Ԫ
	* -------------------------------------------------------------*/
	eScheduleMode m_ScheduleMode;

	/*----------------------------------------------------
	*                      RR����
	* ---------------------------------------------------*/
	/*------------------���ݳ�Ա------------------*/
	std::list<int> m_RRSwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������

	/*------------------��Ա����------------------*/
public:
	/*�ӿں���*/
	void RRSchedule();//RR�����ܿ�
private:
	/*ʵ�ֺ���*/
	void RRInformationClean();//��Դ������Ϣ���
	void RRUpdateAdmitEventIdList(bool clusterFlag);//���½����

	void RRWriteScheduleInfo();//��¼������Ϣ��־
	void RRDelaystatistics();//ʱ��ͳ��


	/*----------------------------------------------------
	*                      PF����
	* ---------------------------------------------------*/

	/*------------------��Ա����------------------*/
public:
	/*�ӿں���*/
	void PFSchedule();//�����ܿ���

private:
	/*ʵ�ֺ���*/
	void PFInformationClean();//�����ǰ���������û��ĵ�����Ϣ
	void schedulePF_RP_CSI_UL();//����PF-RP����

	//����ʱ��ѡȡ�㷨
	sPFInfo selectKthPF(std::vector<sPFInfo>& vecF, int k, int p, int r);
	int partition(std::vector<sPFInfo>& vecF, int p, int r);
	void exchange(std::vector<sPFInfo>& vecF, int i, int j);



	/*----------------------------------------------------
	*                   �ֲ�ʽ��Դ����
	*          DRA:Distributed Resource Allocation
	* ---------------------------------------------------*/
public :
	/*------------------���ݳ�Ա------------------*/
	eDRAMode m_DRAMode;
	std::list<int> m_DRASwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������


	/*------------------��Ա����------------------*/
public:
	/*�ӿں���*/
	void DRASchedule();//DRA�����ܿ�

private:
	/*ʵ�ֺ���*/
	void DRAInformationClean();//��Դ������Ϣ���
	void DRAGroupSizeBasedTDM(bool clusterFlag);//���ڴش�С��ʱ�ָ���
	void DRAUpdateAdmitEventIdList(bool clusterFlag);//���½�������


	void DRASelectBasedOnP13();//����P1��P3����Դ����
	void DRASelectBasedOnP23();//����P2��P3����Դ����
	void DRASelectBasedOnP123();//����P1��P2��P3����Դ����

	void DRAWriteScheduleInfo();//��¼������Ϣ��־
	void DRADelaystatistics();//ʱ��ͳ��
	void DRAConflictListener();//֡����ͻ

    //��־��¼����
	void writeClusterPerformInfo(std::ofstream &out);//д��ִ���Ϣ����־
	void writeEventListInfo(std::ofstream &out);//д��ʱ���б����Ϣ
	void writeEventLogInfo(std::ofstream &out);//д�����¼�����־��Ϣ
	void writeVeUELocationUpdateLogInfo(std::ofstream &out); //д�����λ�ø�����־
};



