#pragma once
#include<vector>
#include"TMC.h"

//<TMC_B>: Traffic Model and Control Base

class TMC_B :public TMC_Basic {
	/*------------------��------------------*/
public:
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

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	TMC_B() = delete;

	/*
	* ���캯��
	*/
	TMC_B(int &t_TTI, 
		SystemConfig& t_Config, 
		RSU* t_RSUAry, 
		VeUE* t_VeUEAry, 
		std::vector<Event>& t_EventVec, 
		std::vector<std::list<int>>& t_EventTTIList, 
		std::vector<std::vector<int>>& t_TTIRSUThroughput);

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	void initialize()override;

	/*
	* �����¼�����
	*/
	void buildEventList(std::ofstream& t_File) override;

	/*
	* ���������ͳ�Ƹ�������
	*/
	void processStatistics(std::ofstream& t_FileDelay, std::ofstream& t_FileEmergencyPossion, std::ofstream& t_FileDataPossion, std::ofstream& t_FileConflict, std::ofstream& t_FileEventLog) override;
private:
	/*
	* д���¼��б����Ϣ
	*/
	void writeEventListInfo(std::ofstream &t_File);

	/*
	* д�����¼�����־��Ϣ
	*/
	void writeEventLogInfo(std::ofstream &t_File);
};