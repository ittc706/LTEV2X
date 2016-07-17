#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include<tuple>
#include"Schedule.h"
#include"Global.h"
#include"VUE.h"
#include"Event.h"

class cRSU {
	//-----------------------TEST-----------------------
public:
	static int s_RSUCount;
	cRSU();
	
	//-----------------------TEST-----------------------
public:
	/*
	* 当前RSU的ID
	*/
	const int m_RSUId=s_RSUCount++;

	/*
	* 当前RSU范围内的VeUEId编号容器
	*/
	std::list<int> m_VeUEIdList;

	/***************************************************************
	------------------------上行调度--------------------------------
	****************************************************************/
	double m_AccumulateThroughput;   //累计吞吐量
	bool m_IsScheduledUL;    //UpLink是否在被调度
	bool m_IsScheduledDL;    //DownLink是否在被调度
	double m_FactorPF[gc_RBNum];
	double m_SINR[gc_RBNum];
	sFeedbackInfo m_FeedbackUL;//将要发送给基站端的反馈信息

	std::vector<double> m_CQIPredictIdeal;
	std::vector<double> m_CQIPredictRealistic;


	/***************************************************************
	---------------------分布式资源管理-----------------------------
	-------------DRA:Distributed Resource Allocation----------------
	****************************************************************/

	//-----------------------static-------------------------------
	/*
	* Pattern的类型种类
	* 每个Pattern所占的FB块连续
	* 不同Pattern的区别是所占的FB块的个数
	* 目前暂定两个，第一个是周期性事件，一个是数据业务事件
	*/
	static const int s_DRAPatternTypeNum=2;

	/*
	* 在全频段每个Pattern种类对应的Pattern数量
	* 下标对应着eMessageType中定义的事件类型的数值
	* 比如，下标0对应着PERIOD；1对应着DATA
	*/
	static const int s_DRAPatternNumPerPatternType[s_DRAPatternTypeNum];

	/*
	* 每个Pattern种类所占的FB数量
	*/
	static const int s_DRA_FBNumPerPatternType[s_DRAPatternTypeNum];

	/*
	* 每个Pattern种类对应的Pattern Idx的列表
	*/
	static const std::list<int> s_DRAPatternIdxList[s_DRAPatternTypeNum];


	/*
	* 所有Pattern类型的Pattern数量总和
	*/
	static const int s_DRATotalPatternNum;

	//-----------------------static-------------------------------

	/*
	* RSU的类型：
	* 1、处于十字路口的RSU
	* 2、分布于道路中间的RSU
	*/
	eRSUType m_RSUType; 

	/*
	* 一个RSU覆盖范围内的簇的个数
	* 与RSU的类型"m_RSUType"有关
	*/
	const int m_DRAClusterNum;

	/*
	* TDR:Time Domain Resource
	* 下标代表簇编号
	* tuple存储的变量的含义依次为：存储每个簇所分配时间数量区间的左端点，右端点以及区间长度
	*/
	std::vector<std::tuple<int,int,int>> m_DRAClusterTDRInfo;

	/*
	* Pattern块释放该资源的TTI时刻（该TTI结束时解除）
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	* 若"m_DRAPatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
	*/
	std::vector<std::vector<bool>> m_DRAPatternIsAvailable;  

	/*
	* 存放每个簇的VeUE的ID的容器
	* 下标代表簇的编号
	*/
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;  

	/*
	* 用于存放当前TTI的接纳事件链表
	*/
	std::vector<int> m_DRAAdmitEventIdList; 

	/*
	* 存放调度调度信息
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	*/
	std::vector<std::vector<sDRAScheduleInfo*>> m_DRAScheduleInfoTable;
	
	/*
	* 当前时刻当前RSU内处于传输状态的事件链表
	* 外层下标代表Pattern编号
	* 内层用list用于处理冲突
	*/
	std::vector<std::list<sDRAScheduleInfo*>> m_DRATransimitEventIdList;

	/*
	* RSU级别的等待列表
	* 存放的是VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发，但VeUE未在可发送消息的时段，转入等待链表
	*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
	*/
	std::list<int> m_DRAWaitEventIdList;

	/*--------------------接口函数--------------------*/
	/*
	* 根据此刻的TTI返回当前进行资源分配的簇的编号
	*/
	int DRAGetClusterIdx(int TTI);

	/*
	* 资源分配信息清空
	*/
	void DRAInformationClean();

	/*
	* 基于簇大小的时分复用
	* 每个簇至少分配一个时隙
	* 剩余时隙按比例进行分配
	*/
	void DRAGroupSizeBasedTDM();

	/*
	* 在System级别的函数内部被调用
	* 用于处理System级别的事件链表，将事件转存入相应的链表中（RSU级别的接纳链表或者RSU级别的等待链表）
	*/
	void DRAProcessSystemLevelEventList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList);

	/*
	* 在System级别的函数内部被调用
	* 用于处理RSU级别的调度表
	* 将发生了RSU切换的的事件推送到System级别的RSU切换链表中，因此要优先于DRAProcessSystemLevelSwitchList的调用
	* 将发生了RSU内小簇切换的事件推送到RSU级别的等待链表中，因此要优先于DRAProcessRSULevelWaitVeUEIdList的调用
	*/
	void DRAProcessRSULevelScheduleInfoTable(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* 在System级别的函数内部被调用
	* 用于处理RSU级别的等待链表，
	* 将发生了RSU切换的事件推送到System级别的RSU切换链表中
	*/
	void DRAProcessRSULevelWaitEventIdList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* 在System级别的函数内部被调用
	* 用于处理System级别的RSU切换链表，将事件转存入相应的链表中（RSU级别的接纳链表或者RSU级别的等待链表）
	* 处理完毕后，该链表的大小为0
	*/
	void DRAProcessSystemLevelSwitchList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* 基于P1和P3的资源分配
	*/
	void DRASelectBasedOnP13(int TTI,std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec);

	/*
	* 基于P2和P3的资源分配
	*/
	void DRASelectBasedOnP23(int TTI,std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec);

	/*
	* 基于P1、P2和P3的资源分配
	*/
	void DRASelectBasedOnP123(int TTI,std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec);

	/*
	* 帧听冲突
	*/
	void DRAConflictListener(int TTI, std::vector<sEvent>& systemEventVec);

	/*
	* 将调度信息写入文件中，测试用！
	*/
	void DRAWriteScheduleInfo(std::ofstream& out,int TTI);

	/*
	* 将以TTI为关键字的日志信息写入文件中，测试用！
	*/
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int clusterIdx, int patternIdx);

	/*
	* 生成包含RSU信息的string
	*/
	std::string toString(int n);

	/*--------------------辅助函数--------------------*/
private:
	/*
	* 在DRAGroupSizeBasedTDM(...)内部被调用
	* 用于求出vector容器最大值的索引
	*/
	int getMaxIndex(const std::vector<double>&clusterSize);

	/*
	* 用于查找指定VeUEId所属的簇编号
	* 不要用这个函数来判断VeUE是否属于该RSU（直接用VeUE对象的RSU编号来判断即可）
	* 这个函数的使用前提是，已知车辆已在簇中
	*/
	int getClusterIdxOfVeUE(int VeUEId);

	/*
	* 计算当前事件所对应的调度区间
	* 在函数DRASelectBasedOnP123(...)内部被调用
    */
	std::list<std::tuple<int, int>> buildScheduleIntervalList(int TTI,sEvent event, std::tuple<int, int, int>ClasterTTI);

	/*
	* 将RSU级别的CallVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToRSULevelAdmitEventIdList(int eventId);

	/*
	* 将RSU级别的WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToRSULevelWaitEventIdList(int eventId);

	/*
	* 将System级别的SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToSystemLevelSwitchEventIdList(int eventId, std::list<int>& systemDRASwitchVeUEIdList);

	/* 
	* 将RSU级别的ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToScheduleInfoTable(int clusterIdx,int patternIdx, sDRAScheduleInfo*p);

	/*
	* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
	*/
	void pullFromScheduleInfoTable(int TTI);
};


inline
void cRSU::pushToRSULevelAdmitEventIdList(int eventId) {
	m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::pushToRSULevelWaitEventIdList(int eventId) {
	m_DRAWaitEventIdList.push_back(eventId);
}

inline
void cRSU::pushToSystemLevelSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void cRSU::pushToScheduleInfoTable(int clusterIdx, int patternIdx, sDRAScheduleInfo*p) {
	m_DRAScheduleInfoTable[clusterIdx][patternIdx] = p;
}

inline
void cRSU::pullFromScheduleInfoTable(int TTI) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	/*将处于调度表中当前可以传输的信息压入m_DRATransimitEventIdList*/
	for (int patternIdx = 0; patternIdx < s_DRATotalPatternNum; patternIdx++) {
		if (m_DRAScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
			m_DRATransimitEventIdList[patternIdx].push_back(m_DRAScheduleInfoTable[clusterIdx][patternIdx]);
			m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
		}
	}
}