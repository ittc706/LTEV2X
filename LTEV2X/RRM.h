#pragma once
#include"VUE.h"
#include"RSU.h"
//RRM:radio resource management

class RRM_Basic {
public:
	RRM_Basic() = delete;
	RRM_Basic(int &systemTTI, sConfigure& systemConfig, cRSU* &systemRSUAry, cVeUE* &systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput) :
		m_TTI(systemTTI),
		m_Config(systemConfig),
		m_RSUAry(systemRSUAry),
		m_VeUEAry(systemVeUEAry),
		m_EventVec(systemEventVec),
		m_EventTTIList(systemEventTTIList),
		m_TTIRSUThroughput(systemTTIRSUThroughput) {}

	//数据成员
	int& m_TTI;//当前的TTI时刻
	sConfigure& m_Config;//系统参数配置
	cRSU* &m_RSUAry;//RSU容器
	cVeUE* &m_VeUEAry;//VeUE容器
	std::vector<sEvent>& m_EventVec;//事件容器
	std::vector<std::list<int>>& m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	std::vector<std::vector<int>>& m_TTIRSUThroughput;//吞吐率，外层下标为TTI，内层下标为RSUId

	//接口
	virtual void schedule() = 0;//流程总控函数
};


