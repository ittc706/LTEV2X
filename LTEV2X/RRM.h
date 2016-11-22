#pragma once
#include"VUE.h"
#include"RSU.h"

//<RRM>: Radio Resource Management

class RRM_TDM_DRA_VeUE;
class RRM_ICC_DRA_VeUE;
class RRM_RR_VeUE;

class RRM_VeUE {
	/*------------------域------------------*/
public:
	VeUE* m_This;

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
	const ModulationType m_ModulationType = gc_ModulationType;

	/*
	* 信道编码码率
	*/
	const double m_CodeRate = gc_CodeRate;

	/*
	* 上次计算的载干比
	* 若为最小值((std::numeric_limits<double>::min)())则说明之前没有计算过
	*/
	std::vector<double> m_PreSINR;

	/*------------------方法------------------*/
public:
	virtual void initialize() = 0;

	virtual RRM_TDM_DRA_VeUE *const getTDM_DRAPoint() = 0;
	virtual RRM_ICC_DRA_VeUE *const getICC_DRAPoint() = 0;
	virtual RRM_RR_VeUE *const getRRPoint() = 0;

	/*
	* 判断是否需要重新计算SINR
	* 取决于干扰列表是否相同
	*/
	bool isNeedRecalculateSINR(int t_PatternIdx);

	/*
	* 判断之前是否已经算过SINR
	*/
	bool isAlreadyCalculateSINR(int t_PatternIdx) { return m_PreSINR[t_PatternIdx] != (std::numeric_limits<double>::min)(); }

};

class RRM_Basic {
	/*------------------域------------------*/
public:
	/*
	* 当前的TTI时刻,指向系统的该参数
	*/
	int& m_TTI;

	/*
	* 系统配置参数,指向系统的该参数
	*/
	SystemConfig& m_Config;

	/*
	* RSU容器,指向系统的该参数
	*/
	RSU* m_RSUAry;

	/*
	* VeUE容器,指向系统的该参数
	*/
	RRM_VeUE* m_VeUEAry;

	/*
	* 事件容器,指向系统的该参数
	*/
	std::vector<Event>& m_EventVec;

	/*
	* 事件触发链表,指向系统的该参数
	*/
	std::vector<std::list<int>>& m_EventTTIList;

	/*
	* 吞吐率，外层下标为TTI，内层下标为RSUId,指向系统的该参数
	*/
	std::vector<std::vector<int>>& m_TTIRSUThroughput;

													  
	/*
	* 其他模块调用时间记录
	*/
	long double m_GTTTimeConsume = 0;
	long double m_WTTimeConsume = 0;

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	RRM_Basic() = delete;

	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
	*/
	RRM_Basic(int &t_TTI, SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
		m_TTI(t_TTI),
		m_Config(t_Config),
		m_RSUAry(t_RSUAry),
		m_EventVec(t_EventVec),
		m_EventTTIList(t_EventTTIList),
		m_TTIRSUThroughput(t_TTIRSUThroughput) {}


	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	virtual void initialize() = 0;

	/*
	* 当发生位置更新时，清除缓存的调度相关信息
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* 流程总控函数
	*/
	virtual void schedule() = 0;
};


