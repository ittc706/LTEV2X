#pragma once
#include"VUE.h"
#include"RSU.h"

//BMAD:business model and control

class BMAD_Basic {
public:
	BMAD_Basic() = delete;
	BMAD_Basic(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList) :
		m_TTI(systemTTI),
		m_Config(systemConfig),
		m_RSUAry(systemRSUAry),
		m_VeUEAry(systemVeUEAry),
		m_EventVec(systemEventVec),
		m_EventTTIList(systemEventTTIList) {}

	/*------------------���ݳ�Ա------------------*/

	int& m_TTI;//��ǰ��TTIʱ��
	sConfigure& m_Config;//ϵͳ��������
	cRSU* m_RSUAry;//RSU����
	cVeUE* m_VeUEAry;//VeUE����
	std::vector<sEvent>& m_EventVec;//�¼�����
	std::vector<std::list<int>>& m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���


	/*--------------------�ӿ�--------------------*/
	virtual void buildEventList(std::ofstream& out)=0;//�����¼�����
	virtual void processStatistics(std::ofstream& outDelay, std::ofstream& outPossion, std::ofstream& outConflict, std::ofstream& outEventLog)=0;//���������ͳ�Ƹ�������
	
};
