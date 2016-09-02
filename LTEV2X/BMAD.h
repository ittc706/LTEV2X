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

	/*------------------数据成员------------------*/

	int& m_TTI;//当前的TTI时刻
	sConfigure& m_Config;//系统参数配置
	cRSU* m_RSUAry;//RSU容器
	cVeUE* m_VeUEAry;//VeUE容器
	std::vector<sEvent>& m_EventVec;//事件容器
	std::vector<std::list<int>>& m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表


	/*--------------------接口--------------------*/
	virtual void buildEventList(std::ofstream& out)=0;//生成事件链表
	virtual void processStatistics(std::ofstream& outDelay, std::ofstream& outPossion, std::ofstream& outConflict, std::ofstream& outEventLog)=0;//仿真结束后统计各种数据
	
};
