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

	//数据成员
	Configure& m_Config;//系统参数配置
	RSU* m_RSUAry;//RSU容器
	VeUE* m_VeUEAry;//VeUE容器

	//接口
	virtual void initialize() = 0;//初始化RSU VeUE内该单元的内部类
	virtual std::tuple<ModulationType, int, double> SINRCalculate(int VeUEId, int subCarrierIdxStart, int subCarrierIdxEnd, MessageType messageType) = 0;
};