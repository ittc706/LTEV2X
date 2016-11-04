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
public:
	GTT_Basic() = delete;
	GTT_Basic(int &systemTTI, Configure& systemConfig, eNB* &systemeNBAry, Road* &systemRoadAry, RSU* &systemRSUAry, VeUE* &systemVeUEAry) :
		m_TTI(systemTTI), m_Config(systemConfig), m_eNBAry(systemeNBAry), m_RoadAry(systemRoadAry), m_RSUAry(systemRSUAry), m_VeUEAry(systemVeUEAry) {}
	
	int& m_TTI;//当前的TTI时刻
	Configure& m_Config;//系统参数配置
	eNB* &m_eNBAry;//基站容器，这里为什么必须是引用类型，因为系统的这些数组指针必须靠该模块来初始化，因此不能传入拷贝
	Road* &m_RoadAry;//道路容器
	RSU* &m_RSUAry;//RSU容器
	VeUE* &m_VeUEAry;//VeUE容器


	virtual void configure() = 0;//参数配置
	virtual void cleanWhenLocationUpdate() = 0;//当发生位置更新时，清除缓存的调度相关信息
	virtual void initialize() = 0;//初始化

	virtual void channelGeneration() = 0;//信道产生
	virtual void freshLoc() = 0;//信道刷新
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &ou2) = 0; //写入地理位置更新日志
	virtual void calculateInterference(const std::vector<std::list<int>>& RRMInterferenceVec) = 0;//计算干扰矩阵
};
