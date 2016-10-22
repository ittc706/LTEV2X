#pragma once
#include"VUE.h"
#include"RSU.h"

//TMAC:traffic model and control

class TMAC_Basic {
public:
	TMAC_Basic() = delete;
	TMAC_Basic(int &systemTTI, sConfigure& systemConfig, RSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput) :
		m_TTI(systemTTI),
		m_Config(systemConfig),
		m_RSUAry(systemRSUAry),
		m_VeUEAry(systemVeUEAry),
		m_EventVec(systemEventVec),
		m_EventTTIList(systemEventTTIList),
		m_TTIRSUThroughput(systemTTIRSUThroughput) {}

	/*------------------���ݳ�Ա------------------*/

	int& m_TTI;//��ǰ��TTIʱ��
	sConfigure& m_Config;//ϵͳ��������
	RSU* m_RSUAry;//RSU����
	cVeUE* m_VeUEAry;//VeUE����
	std::vector<Event>& m_EventVec;//�¼�����
	std::vector<std::list<int>>& m_EventTTIList;//�¼���������m_EventList[i]�����i��TTI���¼���
	std::vector<std::vector<int>>& m_TTIRSUThroughput;//�����ʣ�����±�ΪTTI���ڲ��±�ΪRSUId


	/*--------------------�ӿ�--------------------*/
	virtual void buildEventList(std::ofstream& out)=0;//�����¼�����
	virtual void processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog)=0;//���������ͳ�Ƹ�������
	
};
