#pragma once
//WT:Wireless transmission
#include<vector>
#include"Matrix.h"
#include"VUE.h"


class WT_Basic {
public:

	WT_Basic(cVeUE* systemVeUEAry) :
		m_VeUEAry(systemVeUEAry) {}

	//���ݳ�Ա
	cVeUE* m_VeUEAry;//VeUE����

	//�ӿ�
	virtual void SINRCalculate(int VeUEId, int subCarrierIdxStart, int subCarrierIdxEnd) = 0;
};