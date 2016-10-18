#pragma once
#include<fstream>
#include<random>
#include "WT.h"

class WT_B :public WT_Basic {
public:
	static std::default_random_engine s_Engine;
	static void buildTestFile(int t_Nt, int t_Nr, int subNum);
	static double s_RBBandwidth;


	int m_Nt;//����������
	int m_Nr;//����������
	int m_SubCarrierNum;//���ز�����
	double m_Pt;//���书��
	double m_Ploss;//����·�����
	std::vector<double> m_PlossInter;//���ž���·�����
	double m_Sigma;//��˹�����Ĺ��ʣ���λ��mw
	int  m_Mol;//��һ�εĵ��Ʒ�ʽ


	Matrix m_H;//ÿ��RB��һ��Nr*N1���ŵ�����
	std::vector<Matrix> m_HInter;//�±�Ϊ����Դ���
	std::vector<int> m_MCSLevelTable;//������ЧSINR_EFF��Ŀ�������(Ĭ��Ϊ0.1)����MCS�ȼ�
	std::vector<double> m_QPSK_MI;
	std::vector<double> m_QAM_MI16;
	std::vector<double> m_QAM_MI64;

	WT_B(cVeUE* systemVeUEAry);
	void SINRCalculate(int VeUEId) override;
	void test();

private:
	void initialize();//ģ���ʼ�����õĳ�ʼ������
	void configuration();//ÿ�ε���SINRCalculateǰ��Ҫ���в�������
	void loadH();//�����ŵ���Ӧ����

	int searchMCSLevelTable(double SINR);
	int closest(std::vector<double> v, int target);
	int closest2(std::vector<double> v, int target);
};