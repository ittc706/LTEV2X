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

	//数据成员
	int& m_TTI;//当前的TTI时刻
	Configure& m_Config;//系统参数配置
	RSU* m_RSUAry;//RSU容器
	VeUE* m_VeUEAry;//VeUE容器
	std::vector<Event>& m_EventVec;//事件容器
	std::vector<std::list<int>>& m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	std::vector<std::vector<int>>& m_TTIRSUThroughput;//吞吐率，外层下标为TTI，内层下标为RSUId

	//第三方模块时间统计
	long double m_GTTTimeConsume = 0;
	long double m_WTTimeConsume = 0;

	//接口
	virtual void initialize() = 0;//初始化RSU VeUE内该单元的内部类
	virtual void cleanWhenLocationUpdate() = 0;//当发生位置更新时，清除缓存的调度相关信息
	virtual void schedule() = 0;//流程总控函数
};


