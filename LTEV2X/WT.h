#pragma once
//WT:Wireless transmission
#include<vector>
#include<tuple>
#include"Matrix.h"
#include"VUE.h"


enum ModulationType {
	QPSK,
	_16QAM,
	_64QAM,
};


class WT_Basic {
public:

	WT_Basic(VeUE* systemVeUEAry) :
		m_VeUEAry(systemVeUEAry) {}

	//数据成员
	VeUE* m_VeUEAry;//VeUE容器

	//接口
	virtual std::tuple<ModulationType,int,double> SINRCalculate(int VeUEId, int subCarrierIdxStart, int subCarrierIdxEnd) = 0;
};