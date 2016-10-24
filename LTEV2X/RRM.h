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

	//数据成员
	int& m_TTI;//当前的TTI时刻
	Configure& m_Config;//系统参数配置
	RSU* m_RSUAry;//RSU容器
	VeUE* m_VeUEAry;//VeUE容器
	std::vector<Event>& m_EventVec;//事件容器
	std::vector<std::list<int>>& m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	std::vector<std::vector<int>>& m_TTIRSUThroughput;//吞吐率，外层下标为TTI，内层下标为RSUId

	//第三方模块时间统计
	long double m_GTATTimeConsume = 0;
	long double m_WTTimeConsume = 0;

	//接口
	virtual void schedule() = 0;//流程总控函数
};


