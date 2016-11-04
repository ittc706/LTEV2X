#pragma once
#include<vector>
#include<tuple>
#include"Matrix.h"
#include"VUE.h"
#include"RSU.h"
#include"Enumeration.h"

//<WT>: Wireless Transmission

class WT_Basic {
public:

	WT_Basic(Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry) :
		m_Config(systemConfig), m_RSUAry(systemRSUAry), m_VeUEAry(systemVeUEAry) {}

	//���ݳ�Ա
	Configure& m_Config;//ϵͳ��������
	RSU* m_RSUAry;//RSU����
	VeUE* m_VeUEAry;//VeUE����

	//�ӿ�
	virtual void initialize() = 0;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	virtual WT_Basic* getCopy() = 0;//��ȡ��ģ���һ������
	virtual std::tuple<ModulationType, int, double> SINRCalculate(int VeUEId, int subCarrierIdxStart, int subCarrierIdxEnd, int patternIdx) = 0;
};