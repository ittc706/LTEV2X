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

	int m_Nt;//发送天线数
	int m_Nr;//接收天线数
	double m_Pt;//发射功率
	double m_SNR;//信噪比
	double m_Ploss;//距离路径损耗

	std::vector<std::vector<Complex>> m_H;//信道矩阵

	virtual void SINRCalculate()=0;

};