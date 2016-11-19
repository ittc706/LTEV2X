#pragma once
#include<vector>
#include<tuple>
#include"Matrix.h"
#include"VUE.h"
#include"RSU.h"
#include"Enumeration.h"

//<WT>: Wireless Transmission

class WT_Basic {
	/*------------------��------------------*/
public:
	/*
	* ϵͳ���ò���,ָ��ϵͳ�ĸò���
	*/
	SystemConfig& m_Config;

	/*
	* RSU����,ָ��ϵͳ�ĸò���
	*/
	RSU* m_RSUAry;

	/*
	* VeUE����,ָ��ϵͳ�ĸò���
	*/
	VeUE* m_VeUEAry;

	/*
	* ����SINR��ģʽ
	*/
	WTMode m_SINRMode;

	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	WT_Basic() = delete;

	/*
	* ���캯��
	* �ù��캯�������˸�ģ�����ͼ
	* ����ָ���Ա����ϵͳ���еĶ�Ӧ��Աָ�룬����ͬһʵ��
	*/
	WT_Basic(SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, WTMode t_SINRMode) :
		m_Config(t_Config), m_RSUAry(t_RSUAry), m_VeUEAry(t_VeUEAry), m_SINRMode(t_SINRMode) {}

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	virtual void initialize() = 0;

	/*
	* ��ȡ��ģ���һ������
	*/
	virtual WT_Basic* getCopy() = 0;

	/*
	* �����ظɱ�
	*/
	virtual double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) = 0;
};