#pragma once
#include<vector>
#include<tuple>
#include<memory>
#include"Enumeration.h"
#include"Matrix.h"

//<WT>: Wireless Transmission
class VeUE;

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

class RSU;

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

class System;
class WT {
	/*------------------��̬------------------*/
public:
	static std::default_random_engine s_Engine;
	/*------------------��------------------*/
private:
	/*
	* ָ��ϵͳ��ָ��
	*/
	System* m_Context;
public:
	/*
	* WT��ͼ�µ�RSU����
	*/
	WT_RSU** m_RSUAry;

	/*
	* WT��ͼ�µ�VeUE����
	*/
	WT_VeUE** m_VeUEAry;

	/*
	* ����SINR��ģʽ
	*/
	WTMode m_SINRMode;

public:
	/*
	* ����������
	*/
	int m_Nt;

	/*
	* ����������
	*/
	int m_Nr;

	/*
	* ���书��
	*/
	double m_Pt;

	/*
	* ����·�����
	*/
	double m_Ploss;

	/*
	* ���ž���·�����
	*/
	std::vector<double> m_PlossInterference;

	/*
	* ��˹�����Ĺ��ʣ���λ��mw
	*/
	double m_Sigma;

	/*
	* ÿ�����ز���һ��Nr*Nt���ŵ�����
	*/
	Matrix m_H;

	/*
	* ÿ�����ز���һ��Nr*Nt�ĸ����ŵ�����
	* �±�Ϊ����Դ���
	*/
	std::vector<Matrix> m_HInterference;

	/*
	* ���³�Ա��Ϊָ�룬��new�����ڴ棬��Ϊ�����ͬWTʵ���Ĺ�����Դ��ֻ����initialize()��ʼ��һ��
	*/
	std::shared_ptr<std::vector<double>> m_QPSK_MI;

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
	WT(System* t_Context);

private:
	/*
	* �������캯��
	* ����Ϊ˽�б�������
	*/
	WT(const WT& t_WT);

public:

	/*
	* ��������
	*/
	~WT();

	/*
	* ��ȡϵͳ���ָ��
	*/
	System* getContext() { return m_Context; }

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	virtual void initialize();

	/*
	* ��ȡ��ģ���һ������
	*/
	virtual WT* getCopy();

	/*
	* �ͷŸ�ģ��Ŀ���
	*/
	virtual void freeCopy();

	/*
	* �����ظɱ�
	*/
	virtual double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);

	/*
	* �����ظɱȣ�MRC
	*/
	double SINRCalculateMRC(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);

	/*
	* �����ظɱȣ�MMSE
	*/
	double SINRCalculateMMSE(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);

private:
	/*
	* ÿ�ε���SINRCalculateǰ��Ҫ���в�������
	*/
	void configuration(int t_VeUEId, int t_PatternIdx, int t_SubCarrierNum);

	/*
	* ��ȡ��Ӧ���ز����ŵ���Ӧ����
	*/
	Matrix readH(int t_VeUEId, int t_SubCarrierIdx);

	/*
	* ��ȡ��Ӧ�����ڶ�Ӧ���ز��ϵĸ��ž�������
	*/
	std::vector<Matrix> readInterferenceH(int t_VeUEId, int t_SubCarrierIdx, int t_PatternIdx);

	/*
	* ���ַ������㷨
	*/
	int closest(std::vector<double> t_Vec, double t_Target);

	/*
	* ���
	*/
	double getMutualInformation(std::vector<double> t_Vec, int t_Index);
};