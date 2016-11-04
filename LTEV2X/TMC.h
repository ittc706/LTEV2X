#pragma once
#include"VUE.h"
#include"RSU.h"

//<TMC>: Traffic Model and Control

class TMC_Basic {
public:
	TMC_Basic() = delete;
	TMC_Basic(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
		m_TTI(t_TTI),
		m_Config(t_Config),
		m_RSUAry(t_RSUAry),
		m_VeUEAry(t_VeUEAry),
		m_EventVec(t_EventVec),
		m_EventTTIList(t_EventTTIList),
		m_TTIRSUThroughput(t_TTIRSUThroughput) {}

	/*------------------数据成员------------------*/

	int& m_TTI;//当前的TTI时刻
	Configure& m_Config;//系统参数配置
	RSU* m_RSUAry;//RSU容器
	VeUE* m_VeUEAry;//VeUE容器
	std::vector<Event>& m_EventVec;//事件容器
	std::vector<std::list<int>>& m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	std::vector<std::vector<int>>& m_TTIRSUThroughput;//吞吐率，外层下标为TTI，内层下标为RSUId


	/*--------------------接口--------------------*/
	virtual void initialize() = 0;//初始化RSU VeUE内该单元的内部类
	virtual void buildEventList(std::ofstream& out)=0;//生成事件链表
	virtual void processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog)=0;//仿真结束后统计各种数据
	
};
