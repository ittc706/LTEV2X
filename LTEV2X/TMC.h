#pragma once
#include<list>
#include<vector>

//<TMC>: Traffic Model and Control
class VeUE;

class TMC_VeUE {
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元VeUE数据交互的系统级VeUE对象
	*/
	VeUE* m_This;

	/*------------------方法------------------*/
public:
	/*
	* 取得系统级System的VeUE的指针
	*/
	VeUE* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的VeUE的指针
	*/
	void setSystemPoint(VeUE* t_Point) { m_This = t_Point; }
};

class RSU;

class TMC_RSU {
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元RSU数据交互的系统级RSU对象
	*/
	RSU* m_This;

	/*------------------方法------------------*/
public:
	/*
	* 取得系统级System的RSU的指针
	*/
	RSU* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的RSU的指针
	*/
	void setSystemPoint(RSU* t_Point) { m_This = t_Point; }
};

class System;
class TMC {
	/*------------------静态------------------*/
public:
	/*
	* 紧急事件/周期事件/数据业务事件 的数据包数量
	* 下标以MessageType的定义为准
	*/
	static const std::vector<int> s_MESSAGE_PACKAGE_NUM;

	/*
	* 紧急事件/周期事件/数据业务事件 每个数据包的bit数量
	* 下标以MessageType的定义为准
	*/
	static const std::vector<std::vector<int>> s_MESSAGE_BIT_NUM_PER_PACKAGE;

	/*
	* 紧急事件/周期事件/数据业务事件 初始的退避窗大小
	* 下标以MessageType的定义为准
	*/
	static const std::vector<int> s_INITIAL_WINDOW_SIZE;

	/*
	* 紧急事件/周期事件/数据业务事件 最大的退避窗大小
	* 下标以MessageType的定义为准
	*/
	static const std::vector<int> s_MAX_WINDOW_SIZE;
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

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	TMC() = delete;

	/*
	* 构造函数
	*/
	TMC(System* t_Context) : m_Context(t_Context) {}

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
	virtual void initialize() = 0;

	/*
	* 生成事件链表
	*/
	virtual void buildEventList(std::ofstream& t_File)=0;

	/*
	* 仿真结束后统计各种数据
	*/
	virtual void processStatistics(
		std::ofstream& t_FileStatisticsDescription,
		std::ofstream& t_FileEmergencyDelay, std::ofstream& t_FilePeriodDelay, std::ofstream& t_FileDataDelay, 
		std::ofstream& t_FileEmergencyPossion, std::ofstream& t_FileDataPossion, 
		std::ofstream& t_FileEmergencyConflict, std::ofstream& t_FilePeriodConflict, std::ofstream& t_FileDataConflict,
		std::ofstream& t_FilePackageLoss, std::ofstream& t_FilePackageTransimit,
		std::ofstream& t_FileEventLog
	)=0;
};
