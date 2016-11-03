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

//RRM_TDM_DRA:Radio Resource Management Distributed Resource Allocation



/*===========================================
*                RRM_TDM_DRAģ��
* ==========================================*/
class RRM_TDM_DRA :public RRM_Basic {
public:
	RRM_TDM_DRA() = delete;
	RRM_TDM_DRA(int &systemTTI,
		Configure& systemConfig,
		RSU* systemRSUAry,
		VeUE* systemVeUEAry,
		std::vector<Event>& systemEventVec,
		std::vector<std::list<int>>& systemEventTTIList,
		std::vector<std::vector<int>>& systemTTIRSUThroughput,
		GTAT_Basic* systemGTATPoint,
		WT_Basic* systemWTPoint,
		int threadNum
		);

	/*------------------���ݳ�Ա------------------*/

	GTAT_Basic* m_GTATPoint;//�������˵�Ԫģ��ָ��
	WT_Basic* m_WTPoint;//���ߴ��䵥Ԫģ��ָ��
	std::list<int> m_SwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������

	/*
	* ���ڴ��ָ��Pattern�ĸ����б�
	* ����±�ΪPatternId(������)
	*/
	std::vector<std::list<int>> m_InterferenceVec;

	//���߳��йز���
	int m_ThreadNum;
	std::vector<std::thread> m_Threads;
	std::vector<std::pair<int, int>> m_ThreadsRSUIdRange;

	/*------------------��Ա����------------------*/

public:
	/*�ӿں���*/
	void initialize() override;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	void cleanWhenLocationUpdate()override;//������λ�ø���ʱ���������ĵ��������Ϣ
	void schedule() override;//RRM_TDM_DRA�����ܿأ����ǻ�����麯��
private:
	/*ʵ�ֺ���*/
	void informationClean();//��Դ������Ϣ���
	void groupSizeBasedTDM(bool clusterFlag);//���ڴش�С��ʱ�ָ���
	void uniformTDM(bool clusterFlag);//���ڴش�С��ʱ�ָ���

	void updateAdmitEventIdList(bool clusterFlag);//���½�������
	void processEventList();
	void processScheduleInfoTableWhenLocationUpdate();
	void processWaitEventIdListWhenLocationUpdate();
	void processSwitchListWhenLocationUpdate();
	void processWaitEventIdList();

	void selectBasedOnP123();//����P1��P2��P3����Դ����


	void delaystatistics();//ʱ��ͳ��
	void conflictListener();//֡����ͻ

	void transimitPreparation();//ͳ�Ƹ�����Ϣ
	void transimitStart();//ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	void transimitStartThread(int fromRSUId,int toRSUId);//ģ�⴫�俪ʼ�����µ�����Ϣ
	void transimitEnd();//ģ�⴫�����

	//��־��¼����
	void writeScheduleInfo(std::ofstream& out);//��¼������Ϣ��־
	void writeTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx);
	void writeClusterPerformInfo(std::ofstream &out);//д��ִ���Ϣ����־


	//���ߺ���
	int getMaxIndex(const std::vector<double>&clusterSize);
	int getPatternType(int patternIdx);
	std::pair<int, int> getOccupiedSubCarrierRange(MessageType messageType, int patternIdx);

};

