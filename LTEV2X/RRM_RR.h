#pragma once
#include<vector>
#include<list>
#include<thread>
#include"RRM.h"
#include"GTT.h"
#include"WT.h"
#include"Exception.h"
#include"Global.h"

//<RRM_RR>: Radio Resource Management Round-Robin

class RRM_RR :public RRM_Basic {
public:
	RRM_RR() = delete;
	RRM_RR(int &t_TTI, 
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



