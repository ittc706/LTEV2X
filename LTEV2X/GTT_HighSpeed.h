#pragma once
#include"GTT.h"

// <GTT_HighSpeed>: Geographical Topology and Transport HighSpeed

class GTT_HighSpeed :public GTT_Basic {
public:
	static std::default_random_engine s_Engine;
	GTT_HighSpeed() = delete;
	GTT_HighSpeed(int &t_TTI, Configure& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry, VeUE* &t_VeUEAry);

	std::vector<std::vector<int>> m_VeUENumPerRSU;//统计每个RSU下的车辆数目，外层下标代表第几次位置更新(从0开始)，内层下标代表RSU编号

	void configure()override;//参数配置
	void cleanWhenLocationUpdate()override;//当发生位置更新时，清除缓存的调度相关信息
	void initialize()override;//初始化

	void channelGeneration()override;//信道产生
	void freshLoc() override;//信道刷新
	void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) override; //写入地理位置更新日志
	void calculateInterference(const std::vector<std::list<int>>& RRMInterferenceVec) override;//计算干扰矩阵
};