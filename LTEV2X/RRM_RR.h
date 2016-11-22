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


class RRM_RR_VeUE :public RRM_VeUE {
public:

	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { throw Exp("RuntimeException"); }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { throw Exp("RuntimeException"); }
	RRM_RR_VeUE *const getRRPoint()override { return this; }

	void initialize()override;

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);
};


class RRM_RR :public RRM {
	/*------------------��------------------*/
public:
	/*
	* �������˵�Ԫģ��ָ��
	* �ڼ����õ������˵�Ԫ����������ŵ���Ӧ
	*/
	GTT* m_GTTPoint;

	/*
	* ���ߴ��䵥Ԫģ��ָ��
	* �ڼ��������ߴ��䵥Ԫ������SINR
	*/
	WT* m_WTPoint;

	/*
	* ���ڴ�Ž���RSU�л��ĳ�������ʱ���������
	*/
	std::list<int> m_SwitchEventIdList;

	/*
	* ���ڴ��ָ������ָ��Pattern�ĸ����б�(ֻ����RSU�ڴؼ����)
	* ����±�ΪVeUEId
	* �ڲ��±�ΪPatternIdx
	*/
	std::vector<std::vector<std::list<int>>> m_InterferenceVec;

	/*
	* ���߳�����
	*/
	int m_ThreadNum;

	/*
	* ���߳�����
	*/
	std::vector<std::thread> m_Threads;

	/*
	* �����ÿ���̵߳�RSUId��Χ
	*/
	std::vector<std::pair<int, int>> m_ThreadsRSUIdRange;
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	RRM_RR() = delete;

	/*
	* ���캯��
	* �ù��캯�������˸�ģ�����ͼ
	* ����ָ���Ա����ϵͳ���еĶ�Ӧ��Աָ�룬����ͬһʵ��
	*/
	RRM_RR(int &t_TTI,
		SystemConfig& t_Config,
		RSU* t_RSUAry,
		std::vector<Event>& t_EventVec,
		std::vector<std::list<int>>& t_EventTTIList,
		std::vector<std::vector<int>>& t_TTIRSUThroughput,
		GTT* t_GTTPoint,
		WT* t_WTPoint,
		int t_ThreadNum
	);

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	void initialize() override;

	/*
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	void cleanWhenLocationUpdate()override;

	/*
	* RRM_ICC_DRA�����ܿأ����ǻ�����麯��
	*/
	void schedule() override;

private:
	/*
	* ��Դ������Ϣ���
	*/
	void informationClean();

	/*
	* ���µȴ�����
	*/
	void updateAccessEventIdList(bool t_ClusterFlag);

	/*
	* �����¼�����
	* ����ʱ�̴������¼�����RSU�ĵȴ�������
	*/
	void processEventList();

	/*
	* ����λ�ø���ʱ������ȴ�����
	*/
	void processWaitEventIdListWhenLocationUpdate();

	/*
	* ����λ�ø���ʱ������ת�ӱ�
	*/
	void processSwitchListWhenLocationUpdate();

	/*
	* ����ȴ��������ɽ�������
	*/
	void processWaitEventIdList();

	/*
	* ��ѯ���ȣ����䵱ǰTTI����Դ(���Ǹ���ScheduleTable)
	*/
	void roundRobin();

	/*
	* ʱ��ͳ��
	*/
	void delaystatistics();

	/*
	* ��������ŵ���Ӧ
	*/
	void transimitPreparation();

	/*
	* ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	*/
	void transimitStart();
	void transimitStartThread(int t_FromRSUId, int t_ToRSUId);

	/*
	* ģ�⴫�����������״̬
	*/
	void transimitEnd();

	/*
	* ��¼������Ϣ��־
	*/
	void writeScheduleInfo(std::ofstream& t_File);

	/*
	* ��TTIΪ������¼��־
	*/
	void writeTTILogInfo(std::ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);

	/*
	* д��ִ���Ϣ����־
	*/
	void writeClusterPerformInfo(bool isLocationUpdate, std::ofstream& t_File);

	/*
	* ����ָ��Pattern��Ŷ�Ӧ�����ز����
	*/
	std::pair<int, int> getOccupiedSubCarrierRange(int t_PatternIdx);
};



