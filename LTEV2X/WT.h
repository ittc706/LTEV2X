#pragma once
#include<vector>
#include<tuple>
#include"Matrix.h"
#include"VUE.h"
#include"RSU.h"
#include"Enumeration.h"
#include"GTT.h"
#include"RRM.h"
//<WT>: Wireless Transmission

class WT_VeUE {
public:
	VeUE* m_This;
	void initialize() {}
};

class WT {
	/*------------------域------------------*/
public:
	VeUE* m_This;

	/*
	* 系统配置参数,指向系统的该参数
	*/
	SystemConfig& m_Config;

	/*
	* RSU容器,指向系统的该参数
	*/
	RSU* m_RSUAry;

	
	WT_VeUE** m_VeUEAry;

	/*
	* 计算SINR的模式
	*/
	WTMode m_SINRMode;

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	WT() = delete;

	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
	*/
	WT(SystemConfig& t_Config, RSU* t_RSUAry, WTMode t_SINRMode) :
		m_Config(t_Config), m_RSUAry(t_RSUAry), m_SINRMode(t_SINRMode) {}

	/*
	* 析构函数
	*/
	~WT();

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	virtual void initialize() = 0;

	/*
	* 获取该模块的一个拷贝
	*/
	virtual WT* getCopy() = 0;

	/*
	* 释放该模块的拷贝
	*/
	virtual void freeCopy() = 0;

	/*
	* 计算载干比
	*/
	virtual double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) = 0;
};