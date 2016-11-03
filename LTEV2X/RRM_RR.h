#pragma once
#include<vector>
#include<list>
#include<thread>
#include"RRM.h"
#include"GTAT.h"
#include"WT.h"
#include"Exception.h"
#include"Global.h"
//RRM_TDM_DRA:Radio Resource Management Round-Robin

class RRM_RR :public RRM_Basic {
public:
	RRM_RR() = delete;
	RRM_RR(int &systemTTI, 
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
	void initialize() override;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	void cleanWhenLocationUpdate()override;//������λ�ø���ʱ���������ĵ��������Ϣ
	void schedule() override;//DRA�����ܿأ����ǻ�����麯��

	void informationClean();//��Դ������Ϣ���

	void updateAdmitEventIdList(bool clusterFlag);//���½����
	void processEventList();
	void processWaitEventIdListWhenLocationUpdate();
	void processSwitchListWhenLocationUpdate();
	void processWaitEventIdList();

	void roundRobin();//��ѯ���ȣ����䵱ǰTTI����Դ(���Ǹ���ScheduleTable)
	void transimitPreparation();//ͳ�Ƹ�����Ϣ
	void transimitStart();//ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	void transimitStartThread(int fromRSUId, int toRSUId);//ģ�⴫�俪ʼ�����µ�����Ϣ
	void writeScheduleInfo(std::ofstream& out);//��¼������Ϣ��־
	void writeTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int patternIdx);//��ʱ��Ϊ��λ��¼��־
	void delaystatistics();//ʱ��ͳ��

	void transimitEnd();//ģ�⴫���������ͳ��������(���Ǹ���ScheduleTable)


private:
	std::pair<int, int> getOccupiedSubCarrierRange(int patternIdx);
};



