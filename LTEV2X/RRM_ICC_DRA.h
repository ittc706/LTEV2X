#pragma once

#include<random>
#include<thread>
#include"RRM.h"
#include"WT.h"
#include"GTT.h"
#include"Enumeration.h"
#include"Exception.h"
#include"Global.h"

//<RRM_ICC_DRA> :Radio Resource Management Inter-Cluster Concurrency based Distributed Resource Allocation

class RRM_ICC_DRA_VeUE :public RRM_VeUE{
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
	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { throw Exp("RuntimeException"); }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { return this; }
	RRM_RR_VeUE *const getRRPoint()override { throw Exp("RuntimeException"); }
};


class RRM_ICC_DRA_RSU :public RRM_RSU {
	/*------------------域------------------*/
public:
	/*
	* RSU级别的待接入列表
	* 外层下标为簇编号
	* 内层list存放的是对应簇的VeUEId
	*/
	std::vector<std::list<int>> m_AccessEventIdList;

	/*
	* RSU级别的等待列表
	* 外层下标为簇编号
	* 内层list存放的是对应簇的VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发
	*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
	*/
	std::vector<std::list<int>> m_WaitEventIdList;

	/*
	* Pattern块释是否可用的标记
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	* 若"m_PatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
	*/
	std::vector<std::vector<bool>> m_PatternIsAvailable;

	/*
	* 存放调度调度信息
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	*/
	std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;

	/*
	* 当前时刻当前RSU内处于传输状态的调度信息链表
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	* 最内层用list用于处理冲突
	*/
	std::vector<std::vector<std::list<ScheduleInfo*>>> m_TransimitScheduleInfoList;

	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	RRM_ICC_DRA_RSU();

	/*
	* 初始化
	* 部分成员需要等到GTT模块初始化完毕后，有了簇的数量才能进行本单元RSU的初始化
	*/
	void initialize() override;

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 将AccessVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToAccessEventIdList(int t_ClusterIdx, int t_EventId);

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToWaitEventIdList(int t_ClusterIdx, int t_EventId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

	/*
	* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToTransmitScheduleInfoList(ScheduleInfo* t_Info);

	/*
	* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToScheduleInfoTable(ScheduleInfo* t_Info);

	/*
	* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
	*/
	void pullFromScheduleInfoTable(int t_TTI);

	/*
	* 用于取得指向实际类型的指针
	*/
	RRM_TDM_DRA_RSU *const getTDM_DRAPoint()override { throw Exp("RuntimeException"); }
	RRM_ICC_DRA_RSU *const getICC_DRAPoint() override { return this; }
	RRM_RR_RSU *const getRRPoint() override { throw Exp("RuntimeException"); }
};


class RRM_ICC_DRA :public RRM {
	/*------------------域------------------*/
public:
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
	RRM_ICC_DRA(System* t_Context, int t_ThreadNum);

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
	* 传输开始
	*/
	void transimitStart();
	void transimitStartThread(int t_FromRSUId, int t_ToRSUId);

	/*
	* 传输结束
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


inline
int RRM_ICC_DRA_VeUE::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}


inline
void RRM_ICC_DRA_RSU::pushToAccessEventIdList(int t_ClusterIdx, int t_EventId) {
	m_AccessEventIdList[t_ClusterIdx].push_back(t_EventId);
}

inline
void RRM_ICC_DRA_RSU::pushToWaitEventIdList(int t_ClusterIdx, int t_EventId) {
	m_WaitEventIdList[t_ClusterIdx].push_back(t_EventId);
}

inline
void RRM_ICC_DRA_RSU::pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}

inline
void RRM_ICC_DRA_RSU::pushToTransmitScheduleInfoList(ScheduleInfo* t_Info) {
	m_TransimitScheduleInfoList[t_Info->clusterIdx][t_Info->patternIdx].push_back(t_Info);
}

inline
void RRM_ICC_DRA_RSU::pushToScheduleInfoTable(ScheduleInfo* t_Info) {
	m_ScheduleInfoTable[t_Info->clusterIdx][t_Info->patternIdx] = t_Info;
}

inline
void RRM_ICC_DRA_RSU::pullFromScheduleInfoTable(int t_TTI) {
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
			if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
				m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
				m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
			}
		}
	}
}