#pragma once
#include<vector>
#include<list>
#include<random>
#include<tuple>
#include"Enumeration.h"

class VeUE;
class RRM_TDM_DRA_VeUE;
class RRM_ICC_DRA_VeUE;
class RRM_RR_VeUE;

class RRM_VeUE {
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元VeUE数据交互的系统级VeUE对象
	*/
	VeUE* m_This;

public:
	/*
	* 下标对应的Pattern下，同频干扰数量
	*/
	std::vector<int> m_InterferenceVeUENum;

	/*
	* 下标对应的Pattern下，同频干扰车辆ID
	* 不包含当前车辆
	*/
	std::vector<std::vector<int>> m_InterferenceVeUEIdVec;

	/*
	* 含义同上，上一次的干扰车辆
	* 用于判断是否相同，从而决定是否需要再算一次信道以及载干比
	*/
	std::vector<std::vector<int>> m_PreInterferenceVeUEIdVec;

	/*
	* 调制方式
	*/
	const ModulationType m_ModulationType;

	/*
	* 信道编码码率
	*/
	const double m_CodeRate;

	/*
	* 上次计算的载干比
	* 若为最小值((std::numeric_limits<double>::min)())则说明之前没有计算过
	*/
	std::vector<double> m_PreSINR;

	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	RRM_VeUE() = delete;
	RRM_VeUE(int t_TotalPatternNum);

	/*
	* 析构函数
	* 没有需要清理的资源
	*/
	~RRM_VeUE() {}

	/*
	* 判断是否需要重新计算SINR
	* 取决于干扰列表是否相同
	*/
	bool isNeedRecalculateSINR(int t_PatternIdx);

	/*
	* 判断之前是否已经算过SINR
	*/
	bool isAlreadyCalculateSINR(int t_PatternIdx) { return m_PreSINR[t_PatternIdx] != (std::numeric_limits<double>::min)(); }

	/*
	* 用于取得指向实际类型的指针
	* 由于静态类型为RRM_VeUE
	*/
	virtual RRM_TDM_DRA_VeUE *const getTDM_DRAPoint() = 0;
	virtual RRM_ICC_DRA_VeUE *const getICC_DRAPoint() = 0;
	virtual RRM_RR_VeUE *const getRRPoint() = 0;

	/*
	* 取得系统级System的VeUE的指针
	*/
	VeUE* getSystemPoint();

	/*
	* 设置系统级System的VeUE的指针
	*/
	void setSystemPoint(VeUE* t_Point);
};


class RRM_ICC_DRA_VeUE :public RRM_VeUE {
	/*------------------静态------------------*/
public:
	static std::default_random_engine s_Engine;

	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	RRM_ICC_DRA_VeUE();

	/*
	* 随机选择资源块
	*/
	int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 用于取得指向实际类型的指针
	*/
	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { return this; }
	RRM_RR_VeUE *const getRRPoint()override { throw std::logic_error("RuntimeException"); }
};


class RRM_RR_VeUE :public RRM_VeUE {
	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	RRM_RR_VeUE();

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 用于取得指向实际类型的指针
	*/
	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_RR_VeUE *const getRRPoint()override { return this; }
};


class RRM_TDM_DRA_VeUE :public RRM_VeUE {
	/*------------------静态------------------*/
public:
	static std::default_random_engine s_Engine;

	/*------------------域------------------*/
public:
	/*
	* 该VeUE所在簇的当前地理位置的相对调度区间
	*/
	std::tuple<int, int> m_ScheduleInterval;

	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	RRM_TDM_DRA_VeUE();

	/*
	* 随机选取可用资源快
	*/
	int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 用于取得指向实际类型的指针
	*/
	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { return this; }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_RR_VeUE *const getRRPoint()override { throw std::logic_error("RuntimeException"); }
};
