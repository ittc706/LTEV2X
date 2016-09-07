#pragma once
//WT:Wireless transmission
#include<vector>


class Complex {
public:
	double real;
	double imag;
};



class WT_Basic {
public:
	WT_Basic(int t_Nt, int t_Nr, double t_Pt, double t_SNR, double t_Ploss):
		m_Nt(t_Nt), m_Nr(t_Nr), m_Pt(t_Pt), m_SNR(t_SNR), m_Ploss(t_Ploss){}

	int m_Nt;//����������
	int m_Nr;//����������
	double m_Pt;//���书��
	double m_SNR;//�����
	double m_Ploss;//����·�����

	std::vector<std::vector<Complex>> m_H;//�ŵ�����

	virtual void SINRCalculate()=0;

};