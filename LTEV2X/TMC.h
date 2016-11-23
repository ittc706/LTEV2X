#pragma once
#include<list>
#include"VUE.h"
#include"RSU.h"
#include"Config.h"
#include"Event.h"

//<TMC>: Traffic Model and Control

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

class TMC {
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
	TMC_RSU** m_RSUAry;

	/*
	* VeUE容器
	* 第一维度的指针指向数组，该数组存放指向TMC_VeUE实体的指针
	* 为什么数组存的是指针，因为需要实现多态(虽然暂时TMC单元并没有多态)
	*/
	TMC_VeUE** m_VeUEAry;

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
	TMC() = delete;

	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
	*/
	TMC(int &t_TTI, SystemConfig& t_Config, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
		m_TTI(t_TTI),
		m_Config(t_Config),
		m_EventVec(t_EventVec),
		m_EventTTIList(t_EventTTIList),
		m_TTIRSUThroughput(t_TTIRSUThroughput) {}

	/*
	* 析构函数
	*/
	~TMC();

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
