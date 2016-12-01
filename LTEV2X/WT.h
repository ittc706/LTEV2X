#pragma once
#include<vector>
#include<tuple>
#include<memory>
#include"Enumeration.h"
#include"Matrix.h"

//<WT>: Wireless Transmission
class VeUE;

class WT_VeUE {
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元VeUE数据交互的系统级VeUE对象
	*/
	VeUE* m_This;

	/*------------------方法------------------*/
public:
	/*
	* 取得系统级System的VeUE的指针
	*/
	VeUE* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的VeUE的指针
	*/
	void setSystemPoint(VeUE* t_Point) { m_This = t_Point; }
};

class RSU;

class WT_RSU {
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元RSU数据交互的系统级RSU对象
	*/
	RSU* m_This;

	/*------------------方法------------------*/
public:
	/*
	* 取得系统级System的RSU的指针
	*/
	RSU* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的RSU的指针
	*/
	void setSystemPoint(RSU* t_Point) { m_This = t_Point; }
};

class System;
class WT {
	/*------------------静态------------------*/
public:
	static std::default_random_engine s_Engine;
	/*------------------域------------------*/
private:
	/*
	* 指向系统的指针
	*/
	System* m_Context;
public:
	/*
	* WT视图下的RSU容器
	*/
	WT_RSU** m_RSUAry;

	/*
	* WT视图下的VeUE容器
	*/
	WT_VeUE** m_VeUEAry;

	/*
	* 计算SINR的模式
	*/
	WTMode m_SINRMode;

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
	* 以下成员设为指针，用new分配内存，作为多个不同WT实例的共享资源，只会在initialize()初始化一次
	*/
	std::shared_ptr<std::vector<double>> m_QPSK_MI;

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	WT() = delete;

	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
	*/
	WT(System* t_Context);

private:
	/*
	* 拷贝构造函数
	* 定义为私有避免误用
	*/
	WT(const WT& t_WT);

public:

	/*
	* 析构函数
	*/
	~WT();

	/*
	* 获取系统类的指针
	*/
	System* getContext() { return m_Context; }

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	virtual void initialize();

	/*
	* 获取该模块的一个拷贝
	*/
	virtual WT* getCopy();

	/*
	* 释放该模块的拷贝
	*/
	virtual void freeCopy();

	/*
	* 计算载干比
	*/
	virtual double SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx);

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
	Matrix readH(int t_VeUEId, int t_SubCarrierIdx);

	/*
	* 读取对应车辆在对应子载波上的干扰矩阵数组
	*/
	std::vector<Matrix> readInterferenceH(int t_VeUEId, int t_SubCarrierIdx, int t_PatternIdx);

	/*
	* 二分法查找算法
	*/
	int closest(std::vector<double> t_Vec, double t_Target);

	/*
	* 查表
	*/
	double getMutualInformation(std::vector<double> t_Vec, int t_Index);
};