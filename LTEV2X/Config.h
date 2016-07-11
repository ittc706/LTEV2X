#pragma once
#include"Enumeration.h"

struct sConfig {//系统仿真参数
	int periodicEventNTTI = 20;//周期性事件的周期（单位TTI）

	int locationUpdateNTTI = 50;

	int VUENum = 40;
	int RSUNum = 2;
	int eNBNum = 1;
};