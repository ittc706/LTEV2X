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
	* 系统配置参数,指向系统的该参数
	*/
	SystemConfig& m_Config;

	/*
	* RSU容器,指向系统的该参数
	*/
	RSU* m_RSUAry;

	/*
	* VeUE容器,指向系统的该参数
	*/
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

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	TMC_Basic() = delete;

	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
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
	virtual void buildEventList(std::ofstream& t_File)=0;

	/*
	* 仿真结束后统计各种数据
	*/
	virtual void processStatistics(std::ofstream& t_FileDelay, std::ofstream& t_FileEmergencyPossion, std::ofstream& t_FileDataPossion, std::ofstream& t_FileConflict, std::ofstream& t_FileEventLog)=0;
};
