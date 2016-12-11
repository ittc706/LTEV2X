#pragma once
#include<vector>
#include<utility>
#include<random>
#include<thread>
#include<stdexcept>
#include"RRM.h"

//<RRM_TDM_DRA> :Radio Resource Management Time Division Dultiplexing based Distributed Resource Allocation

class RRM_TDM_DRA :public RRM {
	/*------------------��̬------------------*/
public:
	/*
	* ���дؽ���һ��DRA��ռ�õ�TTI������(NTTI:Number of TTI)
	*/
	static int s_NTTI;

	/*
	* �¼���Pattern����������
	* �������¼��������¼�������ҵ���¼�
	*/
	static int s_PATTERN_TYPE_NUM;

	/*
	* ÿ��Pattern������ռ��RB����
	*/
	static std::vector<int> s_RB_NUM_PER_PATTERN_TYPE;

	/*
	* ÿ��Pattern�����Ӧ��Pattern����
	*/
	static std::vector<int> s_PATTERN_NUM_PER_PATTERN_TYPE;

	/*
	* ÿ��������¼�������Ե�Pattern�Ŀ�ʼ�������ţ���[startIdx,endIdx]��������
	*/
	static std::vector<std::pair<int, int>> s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL;

	/*
	* ����Pattern���������������¼�
	*/
	static int s_TOTAL_PATTERN_NUM;

	/*
	* ���س��򳡾����ò���
	*/
	static void loadConfig(Platform t_Platform);
	/*------------------��------------------*/
public:
	/*
	* ���ڴ�Ž���RSU�л��ĳ�������ʱ���������
	*/
	std::list<int> m_SwitchEventIdList;

	/*
	* ���ڴ��ָ������ָ��Pattern�ĸ����б�(ֻ����RSU�ڴؼ����)
	* ����±�ΪVeUEId
	* �ڲ��±�ΪPatternIdx(������)
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
	RRM_TDM_DRA() = delete;

	/*
	* ���캯��
	* �ù��캯�������˸�ģ�����ͼ
	* ����ָ���Ա����ϵͳ���еĶ�Ӧ��Աָ�룬����ͬһʵ��
	*/
	RRM_TDM_DRA(System* t_Context);

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	void initialize() override;

	/*
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	void cleanWhenLocationUpdate()override;

	/*
	* RRM_TDM_DRA�����ܿأ����ǻ�����麯��
	*/
	void schedule() override;


private:
	/*
	* ��Դ������Ϣ���
	*/
	void informationClean();

	/*
	* ���ڴش�С��ʱ�ָ���
	*/
	void groupSizeBasedTDM(bool t_ClusterFlag);

	/*
	* ���ȷ����ÿ����ʱ����Դ
	*/
	void uniformTDM(bool t_ClusterFlag);

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
	void writeScheduleInfo();

	/*
	* ��TTIΪ������¼��־
	*/
	void writeTTILogInfo(int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);

	/*
	* д��ִ���Ϣ����־
	*/
	void writeClusterPerformInfo();


	/*
	* ���ظ������������ֵ���±�
	*/
	int getMaxIndex(const std::vector<double>&t_ClusterSize);

	/*
	* ����ָ��Pattern��Ŷ�Ӧ��Pattern���ͱ�ţ����¼����ͱ��
	*/
	int getPatternType(int t_PatternIdx);

	/*
	* ����ָ��Pattern��Ŷ�Ӧ�����ز����
	*/
	std::pair<int, int> getOccupiedSubCarrierRange(MessageType t_MessageType, int t_PatternIdx);

};



