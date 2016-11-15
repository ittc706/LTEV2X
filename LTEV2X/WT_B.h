#pragma once
#include<fstream>
#include<random>
#include<memory>
#include "WT.h"

//<WT_B>: Wireless Transmission Base

class WT_B :public WT_Basic {
public:
	static std::default_random_engine s_Engine;


	int m_Nt;//发送天线数
	int m_Nr;//接收天线数
	double m_Pt;//发射功率
	double m_Ploss;//距离路径损耗
	std::vector<double> m_PlossInterference;//干扰距离路径损耗
	double m_Sigma;//高斯噪声的功率，单位是mw
	Matrix m_H;//每个RB有一个Nr*Nt的信道矩阵
	std::vector<Matrix> m_HInterference;//下标为干扰源编号

	//以下成员设为指针，用new分配内存，作为多个不同WT_B实例的共享资源，只会在initialize()初始化一次
	std::shared_ptr<std::vector<double>> m_QPSK_MI;


	WT_B(SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, WTMode t_SINRMode);
	WT_B(const WT_B& t_WT_B);

	void initialize() override;//模块初始化调用的初始化函数,初始化RSU VeUE内该单元的内部类
	WT_Basic* getCopy()override;//获取该模块的一个拷贝
	double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) override;
	double SINRCalculateMRC(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);
	double SINRCalculateMMSE(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);

private:
	void configuration(int t_VeUEId, int t_PatternIdx, int t_SubCarrierNum);//每次调用SINRCalculate前需要进行参数配置
	Matrix readH(int t_VeUEIdx, int t_SubCarrierIdx);//读取对应子载波的信道响应矩阵
	std::vector<Matrix> readInterferenceH(int t_VeUEIdx, int t_SubCarrierIdx, int t_PatternIdx);//读取对应车辆在对应子载波上的干扰矩阵数组

	
	int closest(std::vector<double> t_Vec, double t_Target);//二分法查找算法
	double getMutualInformation(std::vector<double> t_Vec, int t_Index);
};