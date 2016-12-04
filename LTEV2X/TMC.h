#pragma once
#include<list>
#include<vector>
#include<fstream>
#include"TMC_VeUE.h"
#include"TMC_RSU.h"
#include"Event.h"

//<TMC>: Traffic Model and Control

class System;

class TMC {
	/*------------------静态------------------*/
public:
	/*
	* 拥塞等级
	*/
	static int s_CONGESTION_LEVEL_NUM;

	/*
	* 对应拥塞等级下周期性事件的周期(单位TTI)
	*/
	static std::vector<int> s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL;

	/*
	* 紧急事件泊松过程Lamda,单位次/TTI
	*/
	static double s_EMERGENCY_POISSON;

	/*
	* 数据业务事件泊松过程Lamda,单位次/TTI
	*/
	static double s_DATA_POISSON;

	/*
	* 紧急事件/周期事件/数据业务事件 的数据包数量
	* 下标以MessageType的定义为准
	*/
	static std::vector<int> s_MESSAGE_PACKAGE_NUM;

	/*
	* 紧急事件/周期事件/数据业务事件 每个数据包的bit数量
	* 下标以MessageType的定义为准
	*/
	static std::vector<std::vector<int>> s_MESSAGE_BIT_NUM_PER_PACKAGE;

	/*
	* 紧急事件/周期事件/数据业务事件 初始的退避窗大小
	* 下标以MessageType的定义为准
	*/
	static std::vector<int> s_INITIAL_WINDOW_SIZE;

	/*
	* 紧急事件/周期事件/数据业务事件 最大的退避窗大小
	* 下标以MessageType的定义为准
	*/
	static std::vector<int> s_MAX_WINDOW_SIZE;

	/*
	* 加载TMC模块配置参数
	*/
	static void loadConfig(Platform t_Platform);
	/*------------------域------------------*/
private:
	/*
	* 指向系统的指针
	*/
	System* m_Context;
public:
	/*
	* TMC视图下的RSU容器
	*/
	TMC_RSU** m_RSUAry;

	/*
	* TMC视图下的VeUE容器
	*/
	TMC_VeUE** m_VeUEAry;

	/*
	* 事件容器，下标代表事件ID
	*/
	std::vector<Event> m_EventVec;

	/*
	* 以TTI为下标的事件容器
	* 事件触发链表，m_EventTTIList[i]代表第i个TTI的事件表
	*/
	std::vector<std::list<int>> m_EventTTIList;

	/*
	* 紧急事件触发列表
	* 外层下标代表TTI
	* 内层list存放对应TTI触发紧急事件的车辆Id
	*/
	std::vector<std::list<int>> m_EergencyVeUEIdListOfTriggerTTI;

	/*
	* 数据事件触发列表
	* 外层下标代表TTI
	* 内层list存放对应TTI触发数据事件的车辆Id
	*/
	std::vector<std::list<int>> m_DataVeUEIdListOfTriggerTTI;

	/*
	* 吞吐率
	* 外层下标为TTI，内层下标为RSUId
	*/
	std::vector<std::vector<int>> m_TTIRSUThroughput;

	/*
	* 每辆车紧急事件触发的次数
	* 用于验证泊松分布，仿真中并无用处
	*/
	std::vector<int> m_VeUEEmergencyNum;

	/*
	* 每辆车数据业务事件触发的次数
	* 用于验证泊松分布，仿真中并无用处
	*/
	std::vector<int> m_VeUEDataNum;

	/*
	* 每类事件成功传输的数目
	* 外层下标为事件种类
	*/
	std::vector<int> m_TransimitSucceedEventNumPerEventType;

	/*
	* 日志文件
	*/
	std::ofstream m_FileEventLogInfo;
	std::ofstream m_FileEventListInfo;
	std::ofstream m_FileStatisticsDescription;
	std::ofstream m_FileEmergencyDelayStatistics;
	std::ofstream m_FilePeriodDelayStatistics;
	std::ofstream m_FileDataDelayStatistics;
	std::ofstream m_FileEmergencyPossion;
	std::ofstream m_FileDataPossion;
	std::ofstream m_FileEmergencyConflictNum;
	std::ofstream m_FilePeriodConflictNum;
	std::ofstream m_FileDataConflictNum;
	std::ofstream m_FileTTIThroughput;
	std::ofstream m_FileRSUThroughput;
	std::ofstream m_FilePackageLoss;
	std::ofstream m_FilePackageTransimit;
	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	TMC() = delete;

	/*
	* 构造函数
	*/
	TMC(System* t_Context);

	/*
	* 析构函数
	*/
	~TMC();

	/*
	* 获取系统类的指针
	*/
	System* getContext() { return m_Context; }

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	void initialize();

	/*
	* 生成事件链表
	*/
	void eventTrigger();

	/*
	* 生成事件链表
	*/
	void buildEmergencyDataEventTriggerTTI();

	/*
	* 仿真结束后统计各种数据
	*/
	void processStatistics();

private:
	/*
	* 写入事件列表的信息
	*/
	void writeEventListInfo();

	/*
	* 写入以事件的日志信息
	*/
	void writeEventLogInfo();
};
