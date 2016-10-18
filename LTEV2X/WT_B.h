#pragma once
#include<fstream>
#include<random>
#include "WT.h"

class WT_B :public WT_Basic {
public:
	static std::default_random_engine s_Engine;


	int m_Nt;//����������
	int m_Nr;//����������
	double m_Pt;//���书��
	double m_Ploss;//����·�����
	std::vector<double> m_PlossInter;//���ž���·�����
	double m_Sigma;//��˹�����Ĺ��ʣ���λ��mw
	int  m_Mol;//��һ�εĵ��Ʒ�ʽ


	Matrix m_H;//ÿ��RB��һ��Nr*Nt���ŵ�����
	std::vector<Matrix> m_HInter;//�±�Ϊ����Դ���
	std::vector<int> m_MCSLevelTable;//������ЧSINR_EFF��Ŀ�������(Ĭ��Ϊ0.1)����MCS�ȼ�
	std::vector<double> m_QPSK_MI;
	std::vector<double> m_QAM_MI16;
	std::vector<double> m_QAM_MI64;

	WT_B(cVeUE* systemVeUEAry);
	void SINRCalculate(int VeUEId, int subCarrierIdxStart, int subCarrierIdxEnd) override;
	void testCloest();

private:
	void initialize();//ģ���ʼ�����õĳ�ʼ������
	void configuration(int VeUEId);//ÿ�ε���SINRCalculateǰ��Ҫ���в�������
	Matrix readH(int VeUEIdx, int subCarrierIdx);//��ȡ��Ӧ���ز����ŵ���Ӧ����

	int searchMCSLevelTable(double SINR);
	int closest(std::vector<double> v, int target);
	int closest2(std::vector<double> v, int target);
};