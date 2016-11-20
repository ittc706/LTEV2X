#pragma once

#include<vector>
#include<random>
#include<thread>
#include"RRM.h"
#include"RSU.h"
#include"VUE.h"
#include"Enumeration.h"
#include"Exception.h"
#include"WT.h"
#include"GTT.h"

//<RRM_ICC_DRA> :Radio Resource Management Inter-Cluster Concurrency based Distributed Resource Allocation

class RRM_ICC_DRA :public RRM_Basic {
	/*------------------域------------------*/
public:
	/*
	* 地理拓扑单元模块指针
	* 期间会调用地理拓扑单元来计算干扰信道响应
	*/
	GTT_Basic* m_GTTPoint;

	/*
	* 无线传输单元模块指针
	* 期间会调用无线传输单元来计算SINR
	*/
	WT_Basic* m_WTPoint;

	/*
	* 用于存放进行RSU切换的车辆，暂时保存的作用
	*/
	std::list<int> m_SwitchEventIdList;

	/*
	* 用于存放指定车辆指定Pattern的干扰列表(只保留RSU内簇间干扰)
	* 外层下标为VeUEId
	* 内层下标为PatternIdx
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
	RRM_ICC_DRA() = delete;

	/*
	* 构造函数
	* 该构造函数定义了该模块的视图
	* 所有指针成员拷贝系统类中的对应成员指针，共享同一实体
	*/
	RRM_ICC_DRA(int &t_TTI,
		SystemConfig& t_Config,
		RSU* t_RSUAry,
		VeUE* t_VeUEAry,
		std::vector<Event>& t_EventVec,
		std::vector<std::list<int>>& t_EventTTIList,
		std::vector<std::vector<int>>& t_TTIRSUThroughput,
		GTT_Basic* t_GTTPoint,
		WT_Basic* t_WTPoint,
		int t_ThreadNum
	);

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	void initialize() override;

	/*
	* 当发生位置更新时，清除缓存的调度相关信息
	*/
	void cleanWhenLocationUpdate()override;

	/*
	* RRM_ICC_DRA调度总控，覆盖基类的虚函数
	*/
	void schedule() override;
private:
	/*
	* 资源分配信息清空
	*/
	void informationClean();

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
	void writeScheduleInfo(std::ofstream& t_File);

	/*
	* 以TTI为主键记录日志
	*/
	void writeTTILogInfo(std::ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);
	
	/*
	* 写入分簇信息的日志
	*/
	void writeClusterPerformInfo(bool isLocationUpdate, std::ofstream& t_File);


	/*
	* 返回指定Pattern编号对应的子载波编号
	*/
	std::pair<int, int> getOccupiedSubCarrierRange(int t_PatternIdx);
};