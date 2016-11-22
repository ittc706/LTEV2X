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

class RRM_ICC_DRA_VeUE :public RRM_VeUE{
	/*------------------��̬------------------*/
public:
	static std::default_random_engine s_Engine;
	/*------------------��------------------*/
public:
	/*
	* RRM_ICC_DRA���õ�GTT����ز���
	* ��C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ
	*/
	VeUE* m_This;

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	RRM_ICC_DRA_VeUE() = delete;

	/*
	* ���캯��
	*/
	RRM_ICC_DRA_VeUE(VeUE* t_This) :m_This(t_This) {}

	void initialize()override;

	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { throw Exp("RuntimeException"); }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { return this; }
	RRM_RR_VeUE *const getRRPoint()override { throw Exp("RuntimeException"); }

	/*
	* ���ѡ����Դ��
	*/
	int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);
};


class RRM_ICC_DRA :public RRM_Basic {
	/*------------------��------------------*/
public:
	/*
	* �������˵�Ԫģ��ָ��
	* �ڼ����õ������˵�Ԫ����������ŵ���Ӧ
	*/
	GTT_Basic* m_GTTPoint;

	/*
	* ���ߴ��䵥Ԫģ��ָ��
	* �ڼ��������ߴ��䵥Ԫ������SINR
	*/
	WT_Basic* m_WTPoint;

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

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	RRM_ICC_DRA() = delete;

	/*
	* ���캯��
	* �ù��캯�������˸�ģ�����ͼ
	* ����ָ���Ա����ϵͳ���еĶ�Ӧ��Աָ�룬����ͬһʵ��
	*/
	RRM_ICC_DRA(int &t_TTI,
		SystemConfig& t_Config,
		RSU* t_RSUAry,
		VeUE* t_VeUEAry,
		std::vector<Event>& t_EventVec,
		std::vector<std::list<int>>& t_EventTTIList,
		std::vector<std::vector<int>>& t_TTIRSUThroughput,
		GTT_Basic* t_GTTPoint,
		WT_Basic* t_WTPoint,
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
	* ����λ�ø���ʱ��������ȱ�
	*/
	void processScheduleInfoTableWhenLocationUpdate();

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
	* ����P1/P2/P3����Դѡ��
	*/
	void selectRBBasedOnP123();

	/*
	* ʱ��ͳ��
	*/
	void delaystatistics();

	/*
	* ֡����ͻ����ͻ�����
	*/
	void conflictListener();

	/*
	* ��������ŵ���Ӧ
	*/
	void transimitPreparation();

	/*
	* ���俪ʼ
	*/
	void transimitStart();
	void transimitStartThread(int t_FromRSUId, int t_ToRSUId);

	/*
	* �������
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


inline
int RRM_ICC_DRA_VeUE::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}