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
	int  m_Mol;//上一次的调制方式


	Matrix m_H;//每个RB有一个Nr*Nt的信道矩阵
	std::vector<Matrix> m_HInterference;//下标为干扰源编号

    //以下成员设为指针，用new分配内存，作为多个不同WT_B实例的共享资源，只会在initialize()初始化一次
	std::shared_ptr<std::vector<int>> m_MCSLevelTable;//根据有效SINR_EFF和目标误块率(默认为0.1)查找MCS等级
	std::shared_ptr<std::vector<double>> m_QPSK_MI;
	std::shared_ptr<std::vector<double>> m_QAM_MI16;
	std::shared_ptr<std::vector<double>> m_QAM_MI64;

	WT_B(Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry);
	WT_B(const WT_B& t_WT_B);

	void initialize() override;//模块初始化调用的初始化函数,初始化RSU VeUE内该单元的内部类
	WT_Basic* getCopy()override;//获取该模块的一个拷贝
	std::tuple<ModulationType, int, double> SINRCalculate(int VeUEId, int subCarrierIdxStart, int subCarrierIdxEnd, int patternIdx) override;
	void testCloest();
	void testSINR();

private:
	void configuration(int VeUEId, int patternIdx);//每次调用SINRCalculate前需要进行参数配置
	Matrix readH(int VeUEIdx, int subCarrierIdx);//读取对应子载波的信道响应矩阵
	std::vector<Matrix> readInterferenceH(int VeUEIdx, int subCarrierIdx, int patternIdx);//读取对应车辆在对应子载波上的干扰矩阵数组

	int searchMCSLevelTable(double SINR);
	int closest(std::vector<double> v, double target);//二分法查找算法
	int closest2(std::vector<double> v, double target);//线性查找

	double getMutualInformation(std::vector<double> v, int dex);
	std::tuple<ModulationType, int, double> MCS2ModulationAndRate(int MCSLevel);//将MCS等级映射为[调制方式，该调制方式下一个符号对应的bit数量，码率]
};