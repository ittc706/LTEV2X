#pragma once
#include<fstream>
#include<random>
#include<memory>
#include "WT.h"

//<WT_B>: Wireless Transmission Base

class WT_B :public WT_Basic {
	/*------------------静态------------------*/
public:
	static std::default_random_engine s_Engine;

	/*------------------域------------------*/
public:
	/*
	* 发送天线数
	*/
	int m_Nt;

	/*
	* 接收天线数
	*/
	int m_Nr;

	/*
	* 发射功率
	*/
	double m_Pt;

	/*
	* 距离路径损耗
	*/
	double m_Ploss;

	/*
	* 干扰距离路径损耗
	*/
	std::vector<double> m_PlossInterference;

	/*
	* 高斯噪声的功率，单位是mw
	*/
	double m_Sigma;

	/*
	* 每个子载波有一个Nr*Nt的信道矩阵
	*/
	Matrix m_H;

	/*
	* 每个子载波有一组Nr*Nt的干扰信道矩阵
	* 下标为干扰源编号
	*/
	std::vector<Matrix> m_HInterference;

	/*
	* 以下成员设为指针，用new分配内存，作为多个不同WT_B实例的共享资源，只会在initialize()初始化一次
	*/
	std::shared_ptr<std::vector<double>> m_QPSK_MI;

	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
	*/
	WT_B(SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, WTMode t_SINRMode);

	/*
	* 拷贝构造函数
	*/
	WT_B(const WT_B& t_WT_B);

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	void initialize() override;

	/*
	* 获取该模块的一个拷贝
	*/
	WT_Basic* getCopy()override;

	/*
	* 计算载干比
	*/
	double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) override;

	/*
	* 计算载干比：MRC
	*/
	double SINRCalculateMRC(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);

	/*
	* 计算载干比：MMSE
	*/
	double SINRCalculateMMSE(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);

private:
	/*
	* 每次调用SINRCalculate前需要进行参数配置
	*/
	void configuration(int t_VeUEId, int t_PatternIdx, int t_SubCarrierNum);

	/*
	* 读取对应子载波的信道响应矩阵
	*/
	Matrix readH(int t_VeUEIdx, int t_SubCarrierIdx);

	/*
	* 读取对应车辆在对应子载波上的干扰矩阵数组
	*/
	std::vector<Matrix> readInterferenceH(int t_VeUEIdx, int t_SubCarrierIdx, int t_PatternIdx);

	/*
	* 二分法查找算法
	*/
	int closest(std::vector<double> t_Vec, double t_Target);

	/*
	* 查表
	*/
	double getMutualInformation(std::vector<double> t_Vec, int t_Index);
};