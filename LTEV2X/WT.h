#pragma once
#include<vector>
#include<tuple>
#include"Matrix.h"
#include"VUE.h"
#include"RSU.h"
#include"Enumeration.h"

//<WT>: Wireless Transmission
enum SINRMode {
	SINR_MRC,
	SINR_MMSE
};

class WT_Basic {
public:

	WT_Basic(SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, SINRMode t_SINRMode) :
		m_Config(t_Config), m_RSUAry(t_RSUAry), m_VeUEAry(t_VeUEAry), m_SINRMode(t_SINRMode) {}

	//数据成员
	SystemConfig& m_Config;//系统参数配置
	RSU* m_RSUAry;//RSU容器
	VeUE* m_VeUEAry;//VeUE容器
	SINRMode m_SINRMode;//计算SINR的模式

	//接口
	virtual void initialize() = 0;//初始化RSU VeUE内该单元的内部类
	virtual WT_Basic* getCopy() = 0;//获取该模块的一个拷贝
	virtual double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) = 0;
};