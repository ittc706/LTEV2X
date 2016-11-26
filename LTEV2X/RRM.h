#pragma once
#include<vector>
#include<list>
#include"Enumeration.h"

//<RRM>: Radio Resource Management

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
	~RRM_VeUE(){}

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
	VeUE* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的VeUE的指针
	*/
	void setSystemPoint(VeUE* t_Point) { m_This = t_Point; }
};

class RSU;
class RRM_TDM_DRA_RSU;
class RRM_ICC_DRA_RSU;
class RRM_RR_RSU;

class RRM_RSU {
public:
	/*------------------内部类------------------*/
	class ScheduleInfo {
		/*--------域--------*/
	public:
		/*
		* 事件编号
		*/
		int eventId;

		/*
		* 车辆编号
		*/
		int VeUEId;

		/*
		* RSU编号
		*/
		int RSUId;

		/*
		* 簇编号
		*/
		int clusterIdx;

		/*
		* 频域块编号
		*/
		int patternIdx;

		/*
		* 当前传输的数据包的编号
		* 直接从消息类读取
		*/
		int currentPackageIdx = -1;

		/*
		* 剩余待传bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
		* 直接从消息类读取
		*/
		int remainBitNum = -1;

		/*
		* 当前传输的bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
		* 直接从消息类读取
		*/
		int transimitBitNum = -1;

		/*-------方法-------*/
		/*
		* 构造函数
		*/
		ScheduleInfo() {}

		/*
		* 构造函数
		*/
		ScheduleInfo(int t_EventId, int t_VeUEId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx) {
			this->eventId = t_EventId;
			this->VeUEId = t_VeUEId;
			this->RSUId = t_RSUId;
			this->clusterIdx = t_ClusterIdx;
			this->patternIdx = t_PatternIdx;
		}

		/*
		* 生成格式化字符串
		*/
		std::string toLogString();

		/*
		* 生成表示调度信息的string对象
		* 包括事件的Id，车辆的Id，以及要传输该事件所占用的TTI区间
		*/
		std::string toScheduleString(int t_NumTab);
	};

	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元RSU数据交互的系统级RSU对象
	*/
	RSU* m_This;

	/*------------------方法------------------*/
public:
	/*
	* 初始化
	* 部分成员需要等到GTT模块初始化完毕后，有了簇的数量才能进行本单元RSU的初始化
	*/
	virtual void initialize() = 0;

	/*
	* 用于取得指向实际类型的指针
	* 由于静态类型为RRM_RSU
	*/
	virtual RRM_TDM_DRA_RSU *const getTDM_DRAPoint() = 0;
	virtual RRM_ICC_DRA_RSU *const getICC_DRAPoint() = 0;
	virtual RRM_RR_RSU *const getRRPoint() = 0;

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
class RRM {
	/*------------------静态------------------*/
public:
	/*
	* 10MHz，总带宽(Hz)
	*/
	static const int s_TOTAL_BANDWIDTH = 10 * 1000 * 1000;

	/*
	* 每个RB的带宽(Hz)
	*/
	static const int s_BANDWIDTH_OF_RB = 12 * 1000 * 15;

	/*
	* 单位(个),由于RB带宽为180kHz，TTI为1ms，因此单位TTI单位RB传输的比特数为180k*1ms=180
	*/
	static const int s_BIT_NUM_PER_RB = 180;

	/*
	* 调制方式
	*/
	static const ModulationType s_MODULATION_TYPE = QPSK;

	/*
	* 信道编码码率
	*/
	static const double s_CODE_RATE;

	/*
	* 不丢包传输的最小载干比
	* 用于判断是否丢包之用
	*/
	static const double s_DROP_SINR_BOUNDARY;
	/*------------------域------------------*/
private:
	/*
	* 指向系统的指针
	*/
	System* m_Context;
public:
	/*
	* RRM视图下的RSU容器
	*/
	RRM_RSU** m_RSUAry;

	/*
	* RRM视图下的VeUE容器
	*/
	RRM_VeUE** m_VeUEAry;
													  
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
	RRM() = delete;

	/*
	* 构造函数
	*/
	RRM(System* t_Context) : m_Context(t_Context) {}

	/*
	* 析构函数
	*/
	~RRM();

	/*
	* 获取系统类的指针
	*/
	System* getContext() { return m_Context; }

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


