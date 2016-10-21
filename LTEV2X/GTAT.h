#pragma once
// GTAT Geographical topology and transport
#include<random>
#include"RSU.h"
#include"VUE.h"
#include"eNB.h"
#include"Road.h"
#include"Config.h"


struct sMobility {
	int tmp;
};
struct sGeography {
	int tmp;
};


class GTAT_Basic {
public:
	GTAT_Basic() = delete;
	GTAT_Basic(int &systemTTI, sConfigure& systemConfig, ceNB* &systemeNBAry, cRoad* &systemRoadAry, cRSU* &systemRSUAry, cVeUE* &systemVeUEAry) :
		m_TTI(systemTTI), m_Config(systemConfig), m_eNBAry(systemeNBAry), m_RoadAry(systemRoadAry), m_RSUAry(systemRSUAry), m_VeUEAry(systemVeUEAry) {}


	int& m_TTI;//当前的TTI时刻
	sConfigure& m_Config;//系统参数配置
	ceNB* &m_eNBAry;//基站容器，这里为什么必须是引用类型，因为系统的这些数组指针必须靠该模块来初始化，因此不能传入拷贝
	cRoad* &m_RoadAry;//道路容器
	cRSU* &m_RSUAry;//RSU容器
	cVeUE* &m_VeUEAry;//VeUE容器


	virtual void configure() = 0;//参数配置
	virtual void initialize() = 0;//初始化

	virtual void channelGeneration() = 0;//信道产生
	virtual void freshLoc() = 0;//信道刷新
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &ou2) = 0; //写入地理位置更新日志
	virtual void calculateInter() = 0;//计算干扰矩阵
};
