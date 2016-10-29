#pragma once
//WT:Wireless transmission
#include<vector>
#include<tuple>
#include"Matrix.h"
#include"VUE.h"
#include"RSU.h"


enum ModulationType {
	QPSK,
	_16QAM,
	_64QAM,
};


class WT_Basic {
public:

	WT_Basic(Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry) :
		m_Config(systemConfig), m_RSUAry(m_RSUAry), m_VeUEAry(systemVeUEAry) {}

	//���ݳ�Ա
	Configure& m_Config;//ϵͳ��������
	RSU* m_RSUAry;//RSU����
	VeUE* m_VeUEAry;//VeUE����

	//�ӿ�
	virtual void initialize() = 0;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	virtual std::tuple<ModulationType, int, double> SINRCalculate(int VeUEId, int subCarrierIdxStart, int subCarrierIdxEnd, MessageType messageType) = 0;
};