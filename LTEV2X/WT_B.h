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
	int  m_Mol;//��һ�εĵ��Ʒ�ʽ


	Matrix m_H;//ÿ��RB��һ��Nr*Nt���ŵ�����
	std::vector<Matrix> m_HInterference;//�±�Ϊ����Դ���

    //���³�Ա��Ϊָ�룬��new�����ڴ棬��Ϊ�����ͬWT_Bʵ���Ĺ�����Դ��ֻ����initialize()��ʼ��һ��
	std::shared_ptr<std::vector<int>> m_MCSLevelTable;//������ЧSINR_EFF��Ŀ�������(Ĭ��Ϊ0.1)����MCS�ȼ�
	std::shared_ptr<std::vector<double>> m_QPSK_MI;
	std::shared_ptr<std::vector<double>> m_QAM_MI16;
	std::shared_ptr<std::vector<double>> m_QAM_MI64;

	WT_B(Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry);
	WT_B(const WT_B& t_WT_B);

	void initialize() override;//ģ���ʼ�����õĳ�ʼ������,��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	WT_Basic* getCopy()override;//��ȡ��ģ���һ������
	double SINRCalculateMRC(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) override;
	std::tuple<ModulationType, int, double> SINRCalculateMMSE(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) override;
	void testCloest();
	void testSINR();

private:
	void configuration(int t_VeUEId, int t_PatternIdx);//ÿ�ε���SINRCalculateǰ��Ҫ���в�������
	Matrix readH(int t_VeUEIdx, int t_SubCarrierIdx);//��ȡ��Ӧ���ز����ŵ���Ӧ����
	std::vector<Matrix> readInterferenceH(int t_VeUEIdx, int t_SubCarrierIdx, int t_PatternIdx);//��ȡ��Ӧ�����ڶ�Ӧ���ز��ϵĸ��ž�������

	int searchMCSLevelTable(double t_SINR);
	int closest(std::vector<double> t_Vec, double t_Target);//���ַ������㷨
	int closest2(std::vector<double> t_Vec, double t_Target);//���Բ���

	double getMutualInformation(std::vector<double> t_Vec, int t_Index);
	std::tuple<ModulationType, int, double> MCS2ModulationAndRate(int t_MCSLevel);//��MCS�ȼ�ӳ��Ϊ[���Ʒ�ʽ���õ��Ʒ�ʽ��һ�����Ŷ�Ӧ��bit����������]
};