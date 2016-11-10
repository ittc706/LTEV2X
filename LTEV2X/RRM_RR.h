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
	* ���ڴ��ָ��Pattern�ĸ����б�(ͬRSU�µĴؼ���Ż���RSU�����)
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

private:
	void informationClean();//��Դ������Ϣ���

	void updateAdmitEventIdList(bool t_ClusterFlag);//���½����
	void processEventList();
	void processWaitEventIdListWhenLocationUpdate();
	void processSwitchListWhenLocationUpdate();

	void roundRobin();//��ѯ���ȣ����䵱ǰTTI����Դ(���Ǹ���ScheduleTable)
	void delaystatistics();//ʱ��ͳ��

	void transimitPreparation();//ͳ�Ƹ�����Ϣ
	void transimitStart();//ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	void transimitStartThread(int t_FromRSUId, int t_ToRSUId);//ģ�⴫�俪ʼ�����µ�����Ϣ
	void transimitEnd();//ģ�⴫���������ͳ��������(���Ǹ���ScheduleTable)

	void writeScheduleInfo(std::ofstream& t_File);//��¼������Ϣ��־
	void writeTTILogInfo(std::ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);//��ʱ��Ϊ��λ��¼��־
	void writeClusterPerformInfo(bool isLocationUpdate, std::ofstream& t_File);//д��ִ���Ϣ����־

	std::pair<int, int> getOccupiedSubCarrierRange(int t_PatternIdx);
};



