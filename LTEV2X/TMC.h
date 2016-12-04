#pragma once
#include<list>
#include<vector>
#include<fstream>
#include"TMC_VeUE.h"
#include"TMC_RSU.h"
#include"Event.h"

//<TMC>: Traffic Model and Control

class System;

class TMC {
	/*------------------��̬------------------*/
public:
	/*
	* ӵ���ȼ�
	*/
	static int s_CONGESTION_LEVEL_NUM;

	/*
	* ��Ӧӵ���ȼ����������¼�������(��λTTI)
	*/
	static std::vector<int> s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL;

	/*
	* �����¼����ɹ���Lamda,��λ��/TTI
	*/
	static double s_EMERGENCY_POISSON;

	/*
	* ����ҵ���¼����ɹ���Lamda,��λ��/TTI
	*/
	static double s_DATA_POISSON;

	/*
	* �����¼�/�����¼�/����ҵ���¼� �����ݰ�����
	* �±���MessageType�Ķ���Ϊ׼
	*/
	static std::vector<int> s_MESSAGE_PACKAGE_NUM;

	/*
	* �����¼�/�����¼�/����ҵ���¼� ÿ�����ݰ���bit����
	* �±���MessageType�Ķ���Ϊ׼
	*/
	static std::vector<std::vector<int>> s_MESSAGE_BIT_NUM_PER_PACKAGE;

	/*
	* �����¼�/�����¼�/����ҵ���¼� ��ʼ���˱ܴ���С
	* �±���MessageType�Ķ���Ϊ׼
	*/
	static std::vector<int> s_INITIAL_WINDOW_SIZE;

	/*
	* �����¼�/�����¼�/����ҵ���¼� �����˱ܴ���С
	* �±���MessageType�Ķ���Ϊ׼
	*/
	static std::vector<int> s_MAX_WINDOW_SIZE;

	/*
	* ����TMCģ�����ò���
	*/
	static void loadConfig(Platform t_Platform);
	/*------------------��------------------*/
private:
	/*
	* ָ��ϵͳ��ָ��
	*/
	System* m_Context;
public:
	/*
	* TMC��ͼ�µ�RSU����
	*/
	TMC_RSU** m_RSUAry;

	/*
	* TMC��ͼ�µ�VeUE����
	*/
	TMC_VeUE** m_VeUEAry;

	/*
	* �¼��������±�����¼�ID
	*/
	std::vector<Event> m_EventVec;

	/*
	* ��TTIΪ�±���¼�����
	* �¼���������m_EventTTIList[i]�����i��TTI���¼���
	*/
	std::vector<std::list<int>> m_EventTTIList;

	/*
	* �����¼������б�
	* ����±����TTI
	* �ڲ�list��Ŷ�ӦTTI���������¼��ĳ���Id
	*/
	std::vector<std::list<int>> m_EergencyVeUEIdListOfTriggerTTI;

	/*
	* �����¼������б�
	* ����±����TTI
	* �ڲ�list��Ŷ�ӦTTI���������¼��ĳ���Id
	*/
	std::vector<std::list<int>> m_DataVeUEIdListOfTriggerTTI;

	/*
	* ������
	* ����±�ΪTTI���ڲ��±�ΪRSUId
	*/
	std::vector<std::vector<int>> m_TTIRSUThroughput;

	/*
	* ÿ���������¼������Ĵ���
	* ������֤���ɷֲ��������в����ô�
	*/
	std::vector<int> m_VeUEEmergencyNum;

	/*
	* ÿ��������ҵ���¼������Ĵ���
	* ������֤���ɷֲ��������в����ô�
	*/
	std::vector<int> m_VeUEDataNum;

	/*
	* ÿ���¼��ɹ��������Ŀ
	* ����±�Ϊ�¼�����
	*/
	std::vector<int> m_TransimitSucceedEventNumPerEventType;

	/*
	* ��־�ļ�
	*/
	std::ofstream m_FileEventLogInfo;
	std::ofstream m_FileEventListInfo;
	std::ofstream m_FileStatisticsDescription;
	std::ofstream m_FileEmergencyDelayStatistics;
	std::ofstream m_FilePeriodDelayStatistics;
	std::ofstream m_FileDataDelayStatistics;
	std::ofstream m_FileEmergencyPossion;
	std::ofstream m_FileDataPossion;
	std::ofstream m_FileEmergencyConflictNum;
	std::ofstream m_FilePeriodConflictNum;
	std::ofstream m_FileDataConflictNum;
	std::ofstream m_FileTTIThroughput;
	std::ofstream m_FileRSUThroughput;
	std::ofstream m_FilePackageLoss;
	std::ofstream m_FilePackageTransimit;
	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	TMC() = delete;

	/*
	* ���캯��
	*/
	TMC(System* t_Context);

	/*
	* ��������
	*/
	~TMC();

	/*
	* ��ȡϵͳ���ָ��
	*/
	System* getContext() { return m_Context; }

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	void initialize();

	/*
	* �����¼�����
	*/
	void eventTrigger();

	/*
	* �����¼�����
	*/
	void buildEmergencyDataEventTriggerTTI();

	/*
	* ���������ͳ�Ƹ�������
	*/
	void processStatistics();

private:
	/*
	* д���¼��б����Ϣ
	*/
	void writeEventListInfo();

	/*
	* д�����¼�����־��Ϣ
	*/
	void writeEventLogInfo();
};
