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
	* ϵͳ���ò���,ָ��ϵͳ�ĸò���
	*/
	SystemConfig& m_Config;

	/*
	* RSU����,ָ��ϵͳ�ĸò���
	*/
	RSU* m_RSUAry;

	/*
	* VeUE����,ָ��ϵͳ�ĸò���
	*/
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

	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	TMC_Basic() = delete;

	/*
	* ���캯��
	* �ù��캯�������˸�ģ�����ͼ
	* ����ָ���Ա����ϵͳ���еĶ�Ӧ��Աָ�룬����ͬһʵ��
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
	virtual void buildEventList(std::ofstream& t_File)=0;

	/*
	* ���������ͳ�Ƹ�������
	*/
	virtual void processStatistics(std::ofstream& t_FileDelay, std::ofstream& t_FileEmergencyPossion, std::ofstream& t_FileDataPossion, std::ofstream& t_FileConflict, std::ofstream& t_FileEventLog)=0;
};
