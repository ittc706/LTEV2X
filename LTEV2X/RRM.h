#pragma once
#include"VUE.h"
#include"RSU.h"

//<RRM>: Radio Resource Management

class RRM_Basic {
	/*------------------��------------------*/
public:
	/*
	* ��ǰ��TTIʱ��,ָ��ϵͳ�ĸò���
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
	* �¼�����,ָ��ϵͳ�ĸò���
	*/
	std::vector<Event>& m_EventVec;

	/*
	* �¼���������,ָ��ϵͳ�ĸò���
	*/
	std::vector<std::list<int>>& m_EventTTIList;

	/*
	* �����ʣ�����±�ΪTTI���ڲ��±�ΪRSUId,ָ��ϵͳ�ĸò���
	*/
	std::vector<std::vector<int>>& m_TTIRSUThroughput;

													  
	/*
	* ����ģ�����ʱ���¼
	*/
	long double m_GTTTimeConsume = 0;
	long double m_WTTimeConsume = 0;
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	RRM_Basic() = delete;

	/*
	* ���캯��
	* �ù��캯�������˸�ģ�����ͼ
	* ����ָ���Ա����ϵͳ���еĶ�Ӧ��Աָ�룬����ͬһʵ��
	*/
	RRM_Basic(int &t_TTI, SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
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
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* �����ܿغ���
	*/
	virtual void schedule() = 0;
};


