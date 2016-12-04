#pragma once
#include<vector>
#include<list>
#include"RRM_VeUE.h"
#include"RRM_RSU.h"
#include"Enumeration.h"

//<RRM>: Radio Resource Management

class System;

class RRM {
	/*------------------��̬------------------*/
public:
	/*
	* 10MHz���ܴ���(Hz)
	*/
	static const int s_TOTAL_BANDWIDTH = 10 * 1000 * 1000;

	/*
	* ÿ��RB�Ĵ���(Hz)
	*/
	static const int s_BANDWIDTH_OF_RB = 12 * 1000 * 15;

	/*
	* ��λ(��),����RB����Ϊ180kHz��TTIΪ1ms����˵�λTTI��λRB����ı�����Ϊ180k*1ms=180
	*/
	static const int s_BIT_NUM_PER_RB = 180;

	/*
	* ���Ʒ�ʽ
	*/
	static const ModulationType s_MODULATION_TYPE = QPSK;

	/*
	* �ŵ���������
	*/
	static const double s_CODE_RATE;

	/*
	* �������������С�ظɱ�
	* �����ж��Ƿ񶪰�֮��
	*/
	static const double s_DROP_SINR_BOUNDARY;
	/*------------------��------------------*/
private:
	/*
	* ָ��ϵͳ��ָ��
	*/
	System* m_Context;
public:
	/*
	* RRM��ͼ�µ�RSU����
	*/
	RRM_RSU** m_RSUAry;

	/*
	* RRM��ͼ�µ�VeUE����
	*/
	RRM_VeUE** m_VeUEAry;
													  
	/*
	* ����ģ�����ʱ���¼
	*/
	long double m_GTTTimeConsume = 0;
	long double m_WTTimeConsume = 0;

	/*
	* ��־�ļ�
	*/
	std::ofstream m_FileScheduleInfo;
	std::ofstream m_FileClasterPerformInfo;
	std::ofstream m_FileTTILogInfo;
	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	RRM() = delete;

	/*
	* ���캯��
	*/
	RRM(System* t_Context);

	/*
	* ��������
	*/
	~RRM();

	/*
	* ��ȡϵͳ���ָ��
	*/
	System* getContext() { return m_Context; }

	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	virtual void initialize() = 0;

	/*
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* �����ܿغ���
	*/
	virtual void schedule() = 0;
};


