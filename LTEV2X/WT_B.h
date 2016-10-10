#pragma once
#include<fstream>
#include<random>
#include "WT.h"

class WT_B :public WT_Basic {
public:
	static std::default_random_engine s_Engine;
	static void buildTestFile(int t_Nt, int t_Nr, int subNum);
	static double s_RBBandwidth;


	int m_Nt;//发送天线数
	int m_Nr;//接收天线数
	int m_SubCarrierNum;//子载波数量
	double m_Pt;//发射功率
	double m_Ploss;//距离路径损耗
	std::vector<double> m_PlossInter;//干扰距离路径损耗
	double m_Sigma;//高斯噪声的功率，单位是mw
	int  m_Mol;//上一次的调制方式


	Matrix m_H;//每个RB有一个Nr*N1的信道矩阵
	std::vector<Matrix> m_HInter;//下标为干扰源编号
	std::vector<int> m_MCSLevelTable;//根据有效SINR_EFF和目标误块率(默认为0.1)查找MCS等级
	std::vector<double> m_QPSK_MI;
	std::vector<double> m_QAM_MI16;
	std::vector<double> m_QAM_MI64;

	WT_B(int t_Nt, int t_Nr, int t_SubCarrierNum, double t_Pt, double t_Ploss, double t_Sigma,int t_Mol);
	void loadMCSLevelTable();
	void config();

	void SINRCalculate() override;

	
	void test();

private:
	int searchMCSLevelTable(double SINR);
	int closest(std::vector<double> v, int target);
	int closest2(std::vector<double> v, int target);

};