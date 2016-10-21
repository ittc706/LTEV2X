#pragma once
#include"GTAT.h"
#include"Lane.h"

class GTAT_High :public GTAT_Basic {
public:
	static std::default_random_engine s_Engine;
	GTAT_High() = delete;
	GTAT_High(int &systemTTI, sConfigure& systemConfig, ceNB* &systemeNBAry, cRoad* &systemRoadAry, cRSU* &systemRSUAry, cVeUE* &systemVeUEAry, cLane* &systemLaneAry);

	cLane* &m_LaneAry;

	std::vector<std::vector<int>> m_VeUENumPerRSU;//统计每个RSU下的车辆数目，外层下标代表第几次位置更新(从0开始)，内层下标代表RSU编号

	void configure()override;//参数配置
	void initialize()override;//初始化

	void channelGeneration()override;//信道产生
	void freshLoc() override;//信道刷新
	void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) override; //写入地理位置更新日志
	void calculateInter() override;//计算干扰矩阵
};