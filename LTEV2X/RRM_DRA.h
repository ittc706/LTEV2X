#pragma once
#include<vector>
#include<random>
#include<thread>
#include"RRM.h"
#include"RSU.h"
#include"VUE.h"
#include"Enumeration.h"
#include"Exception.h"
#include"WT.h"
#include"GTAT.h"

//RRM_DRA:Radio Resource Management Distributed Resource Allocation


/*===========================================
*        ���ڸ�ģ���ö�����Ͷ���
* ==========================================*/
enum DRAMode {
	//P1:  Collision avoidance based on sensing
	//P2:  Enhanced random resource selection
	//P3:  Location-based resource selection
	P13,               //Combination of P1 and P3
	P23,               //Combination of P2 and P3
	P123               //Combination of P1 and P2 and P3
};


/*===========================================
*                DRAģ��
* ==========================================*/
class RRM_DRA :public RRM_Basic {
public:
	RRM_DRA() = delete;
	RRM_DRA(int &systemTTI, 
		Configure& systemConfig, 
		RSU* systemRSUAry, 
		VeUE* systemVeUEAry, 
		std::vector<Event>& systemEventVec, 
		std::vector<std::list<int>>& systemEventTTIList,
		std::vector<std::vector<int>>& systemTTIRSUThroughput,
		DRAMode systemDRAMode,
		WT_Basic* systemWTPoint,
		GTAT_Basic* systemGTATPoint,
		int threadNum
	);

	/*------------------���ݳ�Ա------------------*/

	DRAMode m_DRAMode;//��Դ��ѡ��Ĳ���
	GTAT_Basic* m_GTATPoint;//�������˵�Ԫģ��ָ��
	WT_Basic* m_WTPoint;//���ߴ��䵥Ԫģ��ָ��
	std::list<int> m_DRASwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������

	/*
	* ���ڴ��ָ��Pattern��[�¼�������Ϣ,��Pattern�ĸ����б�]
	* ����±�ΪPatternId(������)
	*/
	std::vector<std::list<int>> m_DRAInterferenceVec;
	
	int m_NewCount = 0;//��¼��̬�����Ķ���Ĵ���

	int m_DeleteCount = 0;//��¼ɾ����̬��������Ĵ���

	//���߳��йز���
	int m_ThreadNum;
	//std::vector<std::thread> m_Threads;
	std::vector<std::pair<int, int>> m_ThreadsRSUIdRange;

	/*------------------��Ա����------------------*/

public:
	/*�ӿں���*/
	void initialize() override;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	void cleanWhenLocationUpdate()override;//������λ�ø���ʱ���������ĵ��������Ϣ
	void schedule() override;//DRA�����ܿأ����ǻ�����麯��
private:
	/*ʵ�ֺ���*/
	void DRAInformationClean();//��Դ������Ϣ���
	void DRAGroupSizeBasedTDM(bool clusterFlag);//���ڴش�С��ʱ�ָ���

	void DRAUpdateAdmitEventIdList(bool clusterFlag);//���½�������
	void DRAProcessEventList();
	void DRAProcessScheduleInfoTableWhenLocationUpdate();
	void DRAProcessWaitEventIdListWhenLocationUpdate();
	void DRAProcessSwitchListWhenLocationUpdate();
	void DRAProcessWaitEventIdList();


	void DRASelectBasedOnP13();//����P1��P3����Դ����
	void DRASelectBasedOnP23();//����P2��P3����Դ����
	void DRASelectBasedOnP123();//����P1��P2��P3����Դ����


	void DRADelaystatistics();//ʱ��ͳ��
	void DRAConflictListener();//֡����ͻ

	void DRATransimitPreparation();//ͳ�Ƹ�����Ϣ
	void DRATransimitStart();//ģ�⴫�俪ʼ�����µ�����Ϣ
	void DRATransimitStartThread(int fromRSUId,int toRSUId);//ģ�⴫�俪ʼ�����µ�����Ϣ
	void DRATransimitEnd();//ģ�⴫���������ͳ��������

	//��־��¼����
	void DRAWriteScheduleInfo(std::ofstream& out);//��¼������Ϣ��־
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx);
	void DRAWriteClusterPerformInfo(std::ofstream &out);//д��ִ���Ϣ����־


	//���ߺ���
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);
	int DRAGetPatternType(int patternIdx);
	std::pair<int, int> DRAGetOccupiedSubCarrierRange(MessageType messageType, int patternIdx);


	void f() {
		;
	}
};

