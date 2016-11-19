#pragma once
#include"VUE.h"
#include"RSU.h"

//<TMC>: Traffic Model and Control

class TMC_Basic {
	/*------------------��------------------*/
public:
	/*
	* ϵͳ��ǰ��TTIʱ��
	*/
	int& m_TTI;

	/*
	* ϵͳ��������
	*/
	SystemConfig& m_Config;

	/*
	* ʵ��������
	* RSU������
	*/
	RSU* m_RSUAry;
	VeUE* m_VeUEAry;

	/*
	* �¼��������±�����¼�ID
	*/
	std::vector<Event>& m_EventVec;

	/*
	* ��TTIΪ�±���¼�����
	* �¼���������m_EventTTIList[i]�����i��TTI���¼���
	*/
	std::vector<std::list<int>>& m_EventTTIList;

	/*
	* ������
	* ����±�ΪTTI���ڲ��±�ΪRSUId
	*/
	std::vector<std::vector<int>>& m_TTIRSUThroughput;

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	TMC_Basic() = delete;

	/*
	* ���캯��
	*/
	TMC_Basic(int &t_TTI, SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
		m_TTI(t_TTI),
		m_Config(t_Config),
		m_RSUAry(t_RSUAry),
		m_VeUEAry(t_VeUEAry),
		m_EventVec(t_EventVec),
		m_EventTTIList(t_EventTTIList),
		m_TTIRSUThroughput(t_TTIRSUThroughput) {}

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	virtual void initialize() = 0;

	/*
	* �����¼�����
	*/
	virtual void buildEventList(std::ofstream& out)=0;

	/*
	* ���������ͳ�Ƹ�������
	*/
	virtual void processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog)=0;
};
