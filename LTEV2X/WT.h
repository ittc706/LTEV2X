#pragma once
#include<vector>
#include<tuple>
#include"Matrix.h"
#include"VUE.h"
#include"RSU.h"
#include"Enumeration.h"
#include"GTT.h"
#include"RRM.h"
//<WT>: Wireless Transmission

class WT_VeUE {
	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪVeUE���ݽ�����ϵͳ��VeUE����
	*/
	VeUE* m_This;

	/*------------------����------------------*/
public:
	/*
	* ȡ��ϵͳ��System��VeUE��ָ��
	*/
	VeUE* getSystemPoint() { return m_This; }

	/*
	* ����ϵͳ��System��VeUE��ָ��
	*/
	void setSystemPoint(VeUE* t_Point) { m_This = t_Point; }
};


class WT_RSU {
	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪRSU���ݽ�����ϵͳ��RSU����
	*/
	RSU* m_This;

	/*------------------����------------------*/
public:
	/*
	* ȡ��ϵͳ��System��RSU��ָ��
	*/
	RSU* getSystemPoint() { return m_This; }

	/*
	* ����ϵͳ��System��RSU��ָ��
	*/
	void setSystemPoint(RSU* t_Point) { m_This = t_Point; }
};

class WT {
	/*------------------��------------------*/
public:
	/*
	* ϵͳ���ò���,ָ��ϵͳ�ĸò���
	*/
	SystemConfig& m_Config;

	/*
	* RSU����
	* ��һά�ȵ�ָ��ָ�����飬��������ָ��WT_RSUʵ���ָ��
	* Ϊʲô��������ָ�룬��Ϊ��Ҫʵ�ֶ�̬(��Ȼ��ʱWT��Ԫ��û�ж�̬)
	*/
	WT_RSU** m_RSUAry;

	/*
	* VeUE����
	* ��һά�ȵ�ָ��ָ�����飬��������ָ��WT_VeUEʵ���ָ��
	* Ϊʲô��������ָ�룬��Ϊ��Ҫʵ�ֶ�̬(��Ȼ��ʱWT��Ԫ��û�ж�̬)
	*/
	WT_VeUE** m_VeUEAry;

	/*
	* ����SINR��ģʽ
	*/
	WTMode m_SINRMode;

	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	WT() = delete;

	/*
	* ���캯��
	* �ù��캯�������˸�ģ�����ͼ
	* ����ָ���Ա����ϵͳ���еĶ�Ӧ��Աָ�룬����ͬһʵ��
	*/
	WT(SystemConfig& t_Config, WTMode t_SINRMode) :
		m_Config(t_Config), m_SINRMode(t_SINRMode) {}

	/*
	* ��������
	*/
	~WT();

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	virtual void initialize() = 0;

	/*
	* ��ȡ��ģ���һ������
	*/
	virtual WT* getCopy() = 0;

	/*
	* �ͷŸ�ģ��Ŀ���
	*/
	virtual void freeCopy() = 0;

	/*
	* �����ظɱ�
	*/
	virtual double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) = 0;
};