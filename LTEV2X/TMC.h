#pragma once
#include"VUE.h"
#include"RSU.h"

//<TMC>: Traffic Model and Control

class TMC_Basic {
	/*------------------域------------------*/
public:
	/*
	* 系统当前的TTI时刻
	*/
	int& m_TTI;

	/*
	* 系统参数配置
	*/
	SystemConfig& m_Config;

	/*
	* 实体类容器
	* RSU，车辆
	*/
	RSU* m_RSUAry;
	VeUE* m_VeUEAry;

	/*
	* 事件容器，下标代表事件ID
	*/
	std::vector<Event>& m_EventVec;

	/*
	* 以TTI为下标的事件容器
	* 事件触发链表，m_EventTTIList[i]代表第i个TTI的事件表
	*/
	std::vector<std::list<int>>& m_EventTTIList;

	/*
	* 吞吐率
	* 外层下标为TTI，内层下标为RSUId
	*/
	std::vector<std::vector<int>>& m_TTIRSUThroughput;

	/*------------------方法------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	TMC_Basic() = delete;

	/*
	* 构造函数
	*/
	TMC_Basic(int &t_TTI, SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
		m_TTI(t_TTI),
		m_Config(t_Config),
		m_RSUAry(t_RSUAry),
		m_VeUEAry(t_VeUEAry),
		m_EventVec(t_EventVec),
		m_EventTTIList(t_EventTTIList),
		m_TTIRSUThroughput(t_TTIRSUThroughput) {}

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	virtual void initialize() = 0;

	/*
	* 生成事件链表
	*/
	virtual void buildEventList(std::ofstream& out)=0;

	/*
	* 仿真结束后统计各种数据
	*/
	virtual void processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog)=0;
};
