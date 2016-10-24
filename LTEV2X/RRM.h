#pragma once
#include"VUE.h"
#include"RSU.h"
//RRM:radio resource management

class RRM_Basic {
public:
	RRM_Basic() = delete;
	RRM_Basic(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput) :
		m_TTI(systemTTI),
		m_Config(systemConfig),
		m_RSUAry(systemRSUAry),
		m_VeUEAry(systemVeUEAry),
		m_EventVec(systemEventVec),
		m_EventTTIList(systemEventTTIList),
		m_TTIRSUThroughput(systemTTIRSUThroughput) {}

	//���ݳ�Ա
	int& m_TTI;//��ǰ��TTIʱ��
	Configure& m_Config;//ϵͳ��������
	RSU* m_RSUAry;//RSU����
	VeUE* m_VeUEAry;//VeUE����
	std::vector<Event>& m_EventVec;//�¼�����
	std::vector<std::list<int>>& m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���
	std::vector<std::vector<int>>& m_TTIRSUThroughput;//�����ʣ�����±�ΪTTI���ڲ��±�ΪRSUId

	//������ģ��ʱ��ͳ��
	long double m_GTATTimeConsume = 0;
	long double m_WTTimeConsume = 0;

	//�ӿ�
	virtual void schedule() = 0;//�����ܿغ���
};


