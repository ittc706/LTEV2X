#pragma once
#include<vector>
#include<list>
#include"RRM_VeUE.h"
#include"RRM_RSU.h"
#include"Enumeration.h"

//<RRM>: Radio Resource Management

class System;

class RRM {
	/*------------------静态------------------*/
public:
	/*
	* 10MHz，总带宽(Hz)
	*/
	static const int s_TOTAL_BANDWIDTH = 10 * 1000 * 1000;

	/*
	* 每个RB的带宽(Hz)
	*/
	static const int s_BANDWIDTH_OF_RB = 12 * 1000 * 15;

	/*
	* 单位(个),由于RB带宽为180kHz，TTI为1ms，因此单位TTI单位RB传输的比特数为180k*1ms=180
	*/
	static const int s_BIT_NUM_PER_RB = 180;

	/*
	* 调制方式
	*/
	static const ModulationType s_MODULATION_TYPE = QPSK;

	/*
	* 信道编码码率
	*/
	static const double s_CODE_RATE;

	/*
	* 不丢包传输的最小载干比
	* 用于判断是否丢包之用
	*/
	static const double s_DROP_SINR_BOUNDARY;
	/*------------------域------------------*/
private:
	/*
	* 指向系统的指针
	*/
	System* m_Context;
public:
	/*
	* RRM视图下的RSU容器
	*/
	RRM_RSU** m_RSUAry;

	/*
	* RRM视图下的VeUE容器
	*/
	RRM_VeUE** m_VeUEAry;
													  
	/*
	* 其他模块调用时间记录
	*/
	long double m_GTTTimeConsume = 0;
	long double m_WTTimeConsume = 0;

	/*
	* 日志文件
	*/
	std::ofstream m_FileScheduleInfo;
	std::ofstream m_FileClasterPerformInfo;
	std::ofstream m_FileTTILogInfo;
	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	RRM() = delete;

	/*
	* 构造函数
	*/
	RRM(System* t_Context);

	/*
	* 析构函数
	*/
	~RRM();

	/*
	* 获取系统类的指针
	*/
	System* getContext() { return m_Context; }

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	virtual void initialize() = 0;

	/*
	* 当发生位置更新时，清除缓存的调度相关信息
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* 流程总控函数
	*/
	virtual void schedule() = 0;
};


