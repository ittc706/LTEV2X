#pragma once
#include<fstream>
#include<random>
#include<memory>
#include "WT.h"

//<WT_B>: Wireless Transmission Base

class WT_B :public WT_Basic {
public:
	static std::default_random_engine s_Engine;


	int m_Nt;//����������
	int m_Nr;//����������
	double m_Pt;//���书��
	double m_Ploss;//����·�����
	std::vector<double> m_PlossInterference;//���ž���·�����
	double m_Sigma;//��˹�����Ĺ��ʣ���λ��mw
	Matrix m_H;//ÿ��RB��һ��Nr*Nt���ŵ�����
	std::vector<Matrix> m_HInterference;//�±�Ϊ����Դ���

	//���³�Ա��Ϊָ�룬��new�����ڴ棬��Ϊ�����ͬWT_Bʵ���Ĺ�����Դ��ֻ����initialize()��ʼ��һ��
	std::shared_ptr<std::vector<double>> m_QPSK_MI;


	WT_B(SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, WTMode t_SINRMode);
	WT_B(const WT_B& t_WT_B);

	void initialize() override;//ģ���ʼ�����õĳ�ʼ������,��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	WT_Basic* getCopy()override;//��ȡ��ģ���һ������
	double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) override;
	double SINRCalculateMRC(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);
	double SINRCalculateMMSE(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);

private:
	void configuration(int t_VeUEId, int t_PatternIdx, int t_SubCarrierNum);//ÿ�ε���SINRCalculateǰ��Ҫ���в�������
	Matrix readH(int t_VeUEIdx, int t_SubCarrierIdx);//��ȡ��Ӧ���ز����ŵ���Ӧ����
	std::vector<Matrix> readInterferenceH(int t_VeUEIdx, int t_SubCarrierIdx, int t_PatternIdx);//��ȡ��Ӧ�����ڶ�Ӧ���ز��ϵĸ��ž�������

	
	int closest(std::vector<double> t_Vec, double t_Target);//���ַ������㷨
	double getMutualInformation(std::vector<double> t_Vec, int t_Index);
};