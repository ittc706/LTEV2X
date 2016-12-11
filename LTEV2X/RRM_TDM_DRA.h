#pragma once
#include<vector>
#include<utility>
#include<random>
#include<thread>
#include<stdexcept>
#include"RRM.h"

//<RRM_TDM_DRA> :Radio Resource Management Time Division Dultiplexing based Distributed Resource Allocation

class RRM_TDM_DRA :public RRM {
	/*------------------静态------------------*/
public:
	/*
	* 所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)
	*/
	static int s_NTTI;

	/*
	* 事件的Pattern的类型种类
	* 即紧急事件，周期事件，数据业务事件
	*/
	static int s_PATTERN_TYPE_NUM;

	/*
	* 每个Pattern种类所占的RB数量
	*/
	static std::vector<int> s_RB_NUM_PER_PATTERN_TYPE;

	/*
	* 每个Pattern种类对应的Pattern数量
	*/
	static std::vector<int> s_PATTERN_NUM_PER_PATTERN_TYPE;

	/*
	* 每个种类的事件，其各自的Pattern的开始与结束编号，即[startIdx,endIdx]，闭区间
	*/
	static std::vector<std::pair<int, int>> s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL;

	/*
	* 所有Pattern数量，包括三个事件
	*/
	static int s_TOTAL_PATTERN_NUM;

	/*
	* 加载城镇场景配置参数
	*/
	static void loadConfig(Platform t_Platform);
	/*------------------域------------------*/
public:
	/*
	* 用于存放进行RSU切换的车辆，暂时保存的作用
	*/
	std::list<int> m_SwitchEventIdList;

	/*
	* 用于存放指定车辆指定Pattern的干扰列表(只保留RSU内簇间干扰)
	* 外层下标为VeUEId
	* 内层下标为PatternIdx(绝对量)
	*/
	std::vector<std::vector<std::list<int>>> m_InterferenceVec;

	/*
	* 多线程总数
	*/
	int m_ThreadNum;

	/*
	* 多线程容器
	*/
	std::vector<std::thread> m_Threads;

	/*
	* 分配给每个线程的RSUId范围
	*/
	std::vector<std::pair<int, int>> m_ThreadsRSUIdRange;

	/*------------------方法------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	RRM_TDM_DRA() = delete;

	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
	*/
	RRM_TDM_DRA(System* t_Context);

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	void initialize() override;

	/*
	* 当发生位置更新时，清除缓存的调度相关信息
	*/
	void cleanWhenLocationUpdate()override;

	/*
	* RRM_TDM_DRA调度总控，覆盖基类的虚函数
	*/
	void schedule() override;


private:
	/*
	* 资源分配信息清空
	*/
	void informationClean();

	/*
	* 基于簇大小的时分复用
	*/
	void groupSizeBasedTDM(bool t_ClusterFlag);

	/*
	* 均匀分配给每个簇时域资源
	*/
	void uniformTDM(bool t_ClusterFlag);

	/*
	* 更新等待链表
	*/
	void updateAccessEventIdList(bool t_ClusterFlag);

	/*
	* 处理事件链表
	* 将该时刻触发的事件放入RSU的等待链表中
	*/
	void processEventList();

	/*
	* 地理位置更新时，处理调度表
	*/
	void processScheduleInfoTableWhenLocationUpdate();

	/*
	* 地理位置更新时，处理等待链表
	*/
	void processWaitEventIdListWhenLocationUpdate();

	/*
	* 地理位置更新时，处理转接表
	*/
	void processSwitchListWhenLocationUpdate();

	/*
	* 处理等待链表，生成接入链表
	*/
	void processWaitEventIdList();

	/*
	* 基于P1/P2/P3的资源选择
	*/
	void selectRBBasedOnP123();

	/*
	* 时延统计
	*/
	void delaystatistics();

	/*
	* 帧听冲突，冲突后避让
	*/
	void conflictListener();

	/*
	* 计算干扰信道响应
	*/
	void transimitPreparation();

	/*
	* 模拟传输开始，更新调度信息，累计吞吐量
	*/
	void transimitStart();
	void transimitStartThread(int t_FromRSUId, int t_ToRSUId);

	/*
	* 模拟传输结束，更新状态
	*/
	void transimitEnd();

	/*
	* 记录调度信息日志
	*/
	void writeScheduleInfo();

	/*
	* 以TTI为主键记录日志
	*/
	void writeTTILogInfo(int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);

	/*
	* 写入分簇信息的日志
	*/
	void writeClusterPerformInfo();


	/*
	* 返回给定数组中最大值的下标
	*/
	int getMaxIndex(const std::vector<double>&t_ClusterSize);

	/*
	* 返回指定Pattern编号对应的Pattern类型编号，即事件类型编号
	*/
	int getPatternType(int t_PatternIdx);

	/*
	* 返回指定Pattern编号对应的子载波编号
	*/
	std::pair<int, int> getOccupiedSubCarrierRange(MessageType t_MessageType, int t_PatternIdx);

};



