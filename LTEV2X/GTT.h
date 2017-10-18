#pragma once
#include<random>
#include<vector>
#include<list>
#include<tuple>
#include<fstream>
#include"GTT_VeUE.h"
#include"GTT_RSU.h"
#include"GTT_eNB.h"
#include"GTT_Road.h"

#include"Enumeration.h"

// <GTT>: Geographical Topology and Transport

class System;

class GTT {
	/*------------------静态------------------*/
public:
	/*
	* 车辆总数
	*/
	static int s_VeUE_NUM;

	/*
	* RSU总数
	*/
	static int s_RSU_NUM;

	/*
	* 基站总数
	*/
	static int s_eNB_NUM;

	/*
	* 道路总数
	*/
	static int s_ROAD_NUM;

	/*
	* 拥塞等级总数
	*/
	static int s_CONGESTION_LEVEL_NUM;

	/*
	* 拥塞等级对应的车辆数目
	*/
	static std::vector<int> s_VEUE_NUM_PER_CONGESTION_LEVEL;

	/*
	* 加载GTT模块
	*/
	static void loadConfig();

	/*
	* 给定车辆数目，返回拥塞等级
	*/
	static int calcuateCongestionLevel(int t_VeUENum);
	/*------------------域------------------*/
private:
	/*
	* 指向系统的指针
	*/
	System* m_Context;
public:
	/*
	* GTT单元视图下的基站容器
	*/
	GTT_eNB** m_eNBAry;

	/*
	* GTT单元视图下的道路容器
	*/
	GTT_Road** m_RoadAry;

	/*
	* GTT单元视图下的RSU容器
	*/
	GTT_RSU** m_RSUAry;

	/*
	* VeUE容器
	* 第一维度的指针指向数组，该数组存放指向GTT_VeUE实体的指针
	* 为什么数组存的是指针，因为需要实现多态
	*/
	GTT_VeUE** m_VeUEAry;

	/*
	* 日志文件
	*/
	std::ofstream m_FileStatisticsDescription;
	std::ofstream m_FileVeUELocationUpdateLogInfo;
	std::ofstream m_FileVeUENumPerRSULogInfo;
	std::ofstream m_FileLocationInfo;
	std::ofstream m_FileVeUEMessage;
	std::ofstream m_FileVeUECongestionInfo;

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	GTT() = delete;

	/*
	* 构造函数
	*/
	GTT(System* t_Context);

	/*
	* 析构函数
	*/
	~GTT();

	/*
	* 获取系统类的指针
	*/
	System* getContext() { return m_Context; }

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
	virtual void writeVeUELocationUpdateLogInfo() = 0;

	/*
	* 计算干扰矩阵
	* 传入的参数解释
	*		外层下标为车辆编号
	*		内层下标为Pattern编号
	*		最内层list为该车辆在该Pattern下的干扰列表
	* 目前仅有簇间干扰，因为RSU间干扰太小，几乎可以忽略
	*/
	virtual void calculateInterference(const std::vector<std::vector<std::list<int>>>& t_RRMInterferenceVec) = 0;
};
