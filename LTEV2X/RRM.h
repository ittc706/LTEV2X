#pragma once
#include<vector>
#include<list>
#include"VUE.h"
#include"RSU.h"
#include"Event.h"
#include"Config.h"

//<RRM>: Radio Resource Management

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

class RRM {
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
	RRM_RSU** m_RSUAry;

	/*
	* VeUE容器
	* 第一维度的指针指向数组，该数组存放指向RRM_VeUE实体的指针
	* 为什么数组存的是指针，因为需要实现多态
	*/
	RRM_VeUE** m_VeUEAry;

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
	RRM() = delete;

	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
	*/
	RRM(int &t_TTI, SystemConfig& t_Config, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
		m_TTI(t_TTI),
		m_Config(t_Config),
		m_EventVec(t_EventVec),
		m_EventTTIList(t_EventTTIList),
		m_TTIRSUThroughput(t_TTIRSUThroughput) {}

	/*
	* 析构函数
	*/
	~RRM();

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


