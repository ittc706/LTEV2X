#pragma once
#include"Enumeration.h"

struct sConfig {//系统仿真参数
	int periodicEventNTTI;//周期性事件的周期（单位TTI）
	double emergencyLamda;//紧急事件的期望值（在给定的仿真周期内的期望值）

	int locationUpdateNTTI;//车辆刷新位置的周期

	int VUENum;//车辆总数
	int RSUNum;//RSU总数
	int eNBNum;//基站总数

};