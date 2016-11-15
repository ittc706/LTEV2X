#pragma once
#include<random>
#include<set>
#include"RSU.h"
#include"VUE.h"
#include"eNB.h"
#include"Road.h"
#include"Config.h"

// <GTT>: Geographical Topology and Transport

class GTT_Basic {
	/*------------------域------------------*/
public:
	/*
	* 当前的TTI时刻,指向系统的该参数
	*/
	int& m_TTI;

	/*
	* 系统配置参数,指向系统的该参数
	*/
	SystemConfig& m_Config;

	/*
	* 基站容器,指向系统的该参数
	* 这里为什么必须是引用类型，因为系统的这些数组指针必须靠该模块来初始化，因此不能传入拷贝
	*/
	eNB* &m_eNBAry;

	/*
	* 道路容器,指向系统的该参数
	*/
	Road* &m_RoadAry;

	/*
	* RSU容器,指向系统的该参数
	*/
	RSU* &m_RSUAry;

	/*
	* VeUE容器,指向系统的该参数
	*/
	VeUE* &m_VeUEAry;

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	GTT_Basic() = delete;

	/*
	* 构造函数
	* 这里指针都是引用类型，因为需要初始化系统的各个实体数组
	* 该构造函数也定义了该模块的视图
	*/
	GTT_Basic(int &t_TTI, SystemConfig& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry, VeUE* &t_VeUEAry) :
		m_TTI(t_TTI), m_Config(t_Config), m_eNBAry(t_eNBAry), m_RoadAry(t_RoadAry), m_RSUAry(t_RSUAry), m_VeUEAry(t_VeUEAry) {}

	/*
	* 模块参数配置
	*/
	virtual void configure() = 0;

	/*
	* 当发生位置更新时，清除缓存的调度相关信息
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* 初始化各个实体数组
	*/
	virtual void initialize() = 0;

	/*
	* 信道产生
	*/
	virtual void channelGeneration() = 0;

	/*
	* 信道刷新
	*/
	virtual void freshLoc() = 0;

	/*
	* 写入地理位置更新日志
	*/
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &ou2) = 0;

	/*
	* 计算干扰矩阵
	* 传入的参数解释
	*		外层下标为车辆编号
	*		内层下标为Pattern编号
	*		最内层list为该车辆在该Pattern下的干扰列表
	* 目前仅有簇间干扰，因为RSU间干扰太小，几乎可以忽略
	*/
	virtual void calculateInterference(const std::vector<std::vector<std::list<int>>>& RRMInterferenceVec) = 0;
};
