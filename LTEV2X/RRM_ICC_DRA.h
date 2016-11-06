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
#include"GTT.h"

//<RRM_ICC_DRA> :Radio Resource Management Inter-Cluster Concurrency based Distributed Resource Allocation

class RRM_ICC_DRA :public RRM_Basic {
public:
	RRM_ICC_DRA() = delete;
	RRM_ICC_DRA(int &t_TTI,
		Configure& t_Config,
		RSU* t_RSUAry,
		VeUE* t_VeUEAry,
		std::vector<Event>& t_EventVec,
		std::vector<std::list<int>>& t_EventTTIList,
		std::vector<std::vector<int>>& t_TTIRSUThroughput,
		GTT_Basic* t_GTTPoint,
		WT_Basic* t_WTPoint,
		int t_ThreadNum
	);

	/*------------------���ݳ�Ա------------------*/

	GTT_Basic* m_GTTPoint;//�������˵�Ԫģ��ָ��
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

	void updateWaitEventIdList(bool t_ClusterFlag);//���½�������
	void processEventList();
	void processScheduleInfoTableWhenLocationUpdate();
	void processWaitEventIdListWhenLocationUpdate();
	void processSwitchListWhenLocationUpdate();

	void selectBasedOnP123();//����P1��P2��P3����Դ����

	void delaystatistics();//ʱ��ͳ��
	void conflictListener();//֡����ͻ

	void transimitPreparation();//ͳ�Ƹ�����Ϣ
	void transimitStart();//ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	void transimitStartThread(int t_FromRSUId, int t_ToRSUId);//ģ�⴫�俪ʼ�����µ�����Ϣ
	void transimitEnd();//ģ�⴫�����

						//��־��¼����
	void writeScheduleInfo(std::ofstream& t_File);//��¼������Ϣ��־
	void writeTTILogInfo(std::ofstream& t_File, int TTI, EventLogType t_EventLogType, int t_EventId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx);
	void writeClusterPerformInfo(std::ofstream& t_File);//д��ִ���Ϣ����־


	//���ߺ���
	std::pair<int, int> getOccupiedSubCarrierRange(MessageType t_MessageType, int t_PatternIdx);

};