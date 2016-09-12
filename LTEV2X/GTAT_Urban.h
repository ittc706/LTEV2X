#pragma once
#include"GTAT.h"

struct sMobility {
	int tmp;
};
struct sGeography {
	int tmp;
};


class GTAT_Urban :public GTAT_Basic {
public:
	static default_random_engine s_Engine;
	GTAT_Urban() = delete;
	GTAT_Urban(int &systemTTI, sConfigure& systemConfig, ceNB* &systemeNBAry, cRoad* &systemRoadAry, cRSU* &systemRSUAry, cVeUE* &systemVeUEAry);

	void configure()override;//参数配置
	void initialize()override;//初始化

	void channelGeneration()override;//信道产生
	void freshLoc() override;//信道刷新
	void writeVeUELocationUpdateLogInfo(std::ofstream &out) override; //写入地理位置更新日志
};