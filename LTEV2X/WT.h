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

	//���ݳ�Ա
	SystemConfig& m_Config;//ϵͳ��������
	RSU* m_RSUAry;//RSU����
	VeUE* m_VeUEAry;//VeUE����
	SINRMode m_SINRMode;//����SINR��ģʽ

	//�ӿ�
	virtual void initialize() = 0;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	virtual WT_Basic* getCopy() = 0;//��ȡ��ģ���һ������
	virtual double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) = 0;
};