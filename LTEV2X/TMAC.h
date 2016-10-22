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

	/*------------------数据成员------------------*/

	int& m_TTI;//当前的TTI时刻
	sConfigure& m_Config;//系统参数配置
	RSU* m_RSUAry;//RSU容器
	cVeUE* m_VeUEAry;//VeUE容器
	std::vector<Event>& m_EventVec;//事件容器
	std::vector<std::list<int>>& m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	std::vector<std::vector<int>>& m_TTIRSUThroughput;//吞吐率，外层下标为TTI，内层下标为RSUId


	/*--------------------接口--------------------*/
	virtual void buildEventList(std::ofstream& out)=0;//生成事件链表
	virtual void processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog)=0;//仿真结束后统计各种数据
	
};
