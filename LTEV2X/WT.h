#pragma once
//WT:Wireless transmission
#include<vector>
#include"Matrix.h"
#include"VUE.h"


class WT_Basic {
public:

	WT_Basic(VeUE* systemVeUEAry) :
		m_VeUEAry(systemVeUEAry) {}

	//数据成员
	VeUE* m_VeUEAry;//VeUE容器

	//接口
	virtual void SINRCalculate(int VeUEId, int subCarrierIdxStart, int subCarrierIdxEnd) = 0;
};