#pragma once
#include"VUE.h"
#include"RSU.h"
//RRM:radio resource management

class RRM_Basic {
public:
	RRM_Basic() = delete;
	RRM_Basic(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList) :
		m_TTI(systemTTI),
		m_Config(systemConfig),
		m_RSUAry(systemRSUAry),
		m_VeUEAry(systemVeUEAry),
		m_EventVec(systemEventVec),
		m_EventTTIList(systemEventTTIList) {}

	//���ݳ�Ա
	int& m_TTI;//��ǰ��TTIʱ��
	sConfigure& m_Config;//ϵͳ��������
	cRSU* m_RSUAry;//RSU����
	cVeUE* m_VeUEAry;//VeUE����
	std::vector<sEvent>& m_EventVec;//�¼�����
	std::vector<std::list<int>>& m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���

	//�ӿ�
	virtual void schedule() = 0;//�����ܿغ���
};


