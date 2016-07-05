#pragma once
#include"Enumeration.h"

struct sConfig {//系统仿真参数
public:
	int userNum;//汽车数量
	int sectorNum;//扇区总数
	eLinkType linkType;//链路类型
	unsigned int antennaNumOfeNB;//基站天线数


	int HNum;//H数量
	int HNumPereNB;//每RB的H数量
	eTransMode transType;
	int subbandNum;//子带数量
};