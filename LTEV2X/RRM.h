#pragma once
#include"VUE.h"
#include"RSU.h"

//<RRM>: Radio Resource Management

class RRM_Basic {
public:
	RRM_Basic() = delete;
	RRM_Basic(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
		m_TTI(t_TTI),
		m_Config(t_Config),
		m_RSUAry(t_RSUAry),
		m_VeUEAry(t_VeUEAry),
		m_EventVec(t_EventVec),
		m_EventTTIList(t_EventTTIList),
		m_TTIRSUThroughput(t_TTIRSUThroughput) {}

	//���ݳ�Ա
	int& m_TTI;//��ǰ��TTIʱ��
	Configure& m_Config;//ϵͳ��������
	RSU* m_RSUAry;//RSU����
	VeUE* m_VeUEAry;//VeUE����
	std::vector<Event>& m_EventVec;//�¼�����
	std::vector<std::list<int>>& m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���
	std::vector<std::vector<int>>& m_TTIRSUThroughput;//�����ʣ�����±�ΪTTI���ڲ��±�ΪRSUId

	//������ģ��ʱ��ͳ��
	long double m_GTTTimeConsume = 0;
	long double m_WTTimeConsume = 0;

	//�ӿ�
	virtual void initialize() = 0;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	virtual void cleanWhenLocationUpdate() = 0;//������λ�ø���ʱ���������ĵ��������Ϣ
	virtual void schedule() = 0;//�����ܿغ���
};


