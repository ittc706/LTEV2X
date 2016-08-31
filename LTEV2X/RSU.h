#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include<tuple>
#include"Schedule.h"
#include"Global.h"
#include"VUE.h"
#include"Traffic.h"
#include "IMTA.h"

class cRSU {
public:
	cRSU() {}
	/*--------------------------------------------------------------
	*                      地理拓扑单元
	* -------------------------------------------------------------*/

	/*------------------数据成员------------------*/
	int m_RSUId;
	std::list<int> m_VeUEIdList;//当前RSU范围内的VeUEId编号容器
	float m_fAbsX;
	float m_fAbsY;
	//cChannelModel *channelModel;
	cIMTA *imta;
	float m_fantennaAngle;

	/*------------------成员函数------------------*/
	void initialize(sRSUConfigure &t_RSUConfigure);

	/*--------------------------------------------------------------
	*                      无线资源管理单元
	* -------------------------------------------------------------*/

	/*----------------------------------------------------
	*                      RR调度
	* ---------------------------------------------------*/
	/*------------------数据成员------------------*/

	/*
	* RSU级别的等待列表
	* 存放的是VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发，但VeUE未在可发送消息的时段，转入等待链表
	*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
	*/
	std::list<int> m_RRWaitEventIdList;

	/*
	* 存放调度调度信息，由于是轮询，因此每个时刻仅有一个用户
	*/
	sRRScheduleInfo* m_RRScheduleInfo;


	/*
	* 当前时刻处于等待接入状态的紧急事件列表
	*/
	std::list<int> m_RREmergencyWaitEventIdList;

	/*------------------成员函数------------------*/
public:
	/*接口函数*/
	void RRInformationClean();//资源分配信息清空

	/*
	* 用于处理System级别的事件链表，将事件转存入RSU级别的等待链表中
	*/
	void RRProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList);


	/*
	* 当发生位置更新后，处理调度表
	* 将发生了RSU切换的的事件推送到System级别的RSU切换链表中，因此要优先于RRProcessSystemLevelSwitchList的调用
	* 将发生了RSU内小簇切换的事件推送到RSU级别的等待链表中，因此要优先于RRProcessRSULevelWaitVeUEIdList的调用
	*/
	void RRProcessScheduleInfoWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList);


	/*
	* 当发生位置更新后，处理等待链表
	*/
	void RRProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList);

	/*
	* 用于处理System级别的RSU切换链表，将事件转存入相应的链表中（RSU级别的接纳链表或者RSU级别的等待链表）
	* 处理完毕后，该链表的大小为0
	*/
	void RRProcessSwitchListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList);

	/*
	* 用于处理RSU级别的等待链表，
	* 将发生了RSU切换的事件推送到System级别的RSU切换链表中
	*/
	void RRProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* 将调度信息写入文件中，测试用！
	*/
	void RRWriteScheduleInfo(std::ofstream& out, int TTI);

	/*
	* 将以TTI为关键字的日志信息写入文件中，测试用！
	*/
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int clusterIdx, int patternIdx);


private:
	/*实现函数*/

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void RRPushToWaitEventIdList(int eventId);
	void RRPushToEmergencyWaitEventIdList(int eventId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList);

	/*----------------------------------------------------
	*                      PF调度
	* ---------------------------------------------------*/
	/*------------------数据成员------------------*/
public:
	double m_AccumulateThroughput;   //累计吞吐量
	bool m_IsScheduledUL;    //UpLink是否在被调度
	bool m_IsScheduledDL;    //DownLink是否在被调度
	double m_FactorPF[gc_RBNum];
	double m_SINR[gc_RBNum];
	sFeedbackInfo m_FeedbackUL;//将要发送给基站端的反馈信息


    /*----------------------------------------------------
    *                   分布式资源管理
    *          DRA:Distributed Resource Allocation
    * ---------------------------------------------------*/

	/*------------------数据成员------------------*/
public:
	/*
	* 一个RSU覆盖范围内的簇的个数，该值可以表明RSU的类型，即处于十字路口或是道路中间
	* 与RSU的类型"m_RSUType"有关
	*/
	int m_DRAClusterNum;

	/*
	* TDR:Time Domain Resource
	* 下标代表簇编号
	* tuple存储的变量的含义依次为：存储每个簇所分配时间数量区间的左端点，右端点以及区间长度
	*/
	std::vector<std::tuple<int,int,int>> m_DRAClusterTDRInfo;

	/*
	* 存放每个簇的VeUE的Id的容器
	* 下标代表簇的编号
	*/
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;  

	/*
	* Pattern块释是否可用的标记
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	* 若"m_DRAPatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
	*/
	std::vector<std::vector<bool>> m_DRAPatternIsAvailable;

	/*
	* 用于存放当前TTI的接纳事件链表
	*/
	std::list<int> m_DRAAdmitEventIdList; 

	/*
	* RSU级别的等待列表
	* 存放的是VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发，但VeUE未在可发送消息的时段，转入等待链表
	*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
	*/
	std::list<int> m_DRAWaitEventIdList;

	/*
	* 存放调度调度信息
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	*/
	std::vector<std::vector<sDRAScheduleInfo*>> m_DRAScheduleInfoTable;
	
	/*
	* 当前时刻当前RSU内处于传输状态的调度信息链表
	* 外层下标代表Pattern编号
	* 内层用list用于处理冲突
	*/
	std::vector<std::list<sDRAScheduleInfo*>> m_DRATransimitScheduleInfoList;


	/*
	* Pattern块释是否可用的标记
	* 下标代表Pattern编号
	* 若"m_DRAPatternIsAvailable[j]==true"代表Pattern块j可用
	*/
	std::vector<bool> m_DRAEmergencyPatternIsAvailable;

	/*
	* 当前时刻等待接入的紧急事件列表
	*/
	std::list<int> m_DRAEmergencyAdmitEventIdList;

	/*
	* 当前时刻处于等待接入状态的紧急事件列表
	*/
	std::list<int> m_DRAEmergencyWaitEventIdList;

	/*
	* 当前时刻处于传输状态的紧急事件调度信息列表
	* 外层下标代表pattern编号
	*/
	std::vector<std::list<sDRAScheduleInfo*>> m_DRAEmergencyTransimitScheduleInfoList;

	/*
	* 当前时刻处于调度状态的紧急事件调度信息列表
	*/
	std::vector<sDRAScheduleInfo*> m_DRAEmergencyScheduleInfoTable;




	/*------------------成员函数------------------*/
public:
	/*接口函数*/

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
	void DRAGroupSizeBasedTDM(cVeUE *systemVeUEVec);

	/*
	* 用于处理System级别的事件链表，将事件转存入相应的链表中（RSU级别的接纳链表或者RSU级别的等待链表）
	*/
	void DRAProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList);

	/*
	* 用于处理等待链表的接入
	*/
	void DRAProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* 用于处理RSU级别的调度表
	* 将发生了RSU切换的的事件推送到System级别的RSU切换链表中，因此要优先于DRAProcessSystemLevelSwitchList的调用
	* 将发生了RSU内小簇切换的事件推送到RSU级别的等待链表中，因此要优先于DRAProcessRSULevelWaitVeUEIdList的调用
	*/
	void DRAProcessScheduleInfoTableWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	
	/*
	* 用于处理RSU级别的等待链表，
	* 将发生了RSU切换的事件推送到System级别的RSU切换链表中
	*/
	void DRAProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* 用于处理System级别的RSU切换链表，将事件转存入相应的链表中（RSU级别的接纳链表或者RSU级别的等待链表）
	* 处理完毕后，该链表的大小为0
	*/
	void DRAProcessSwitchListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* 基于P1和P3的资源分配
	*/
	void DRASelectBasedOnP13(int TTI,cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* 基于P2和P3的资源分配
	*/
	void DRASelectBasedOnP23(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* 基于P1、P2和P3的资源分配
	*/
	void DRASelectBasedOnP123(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* 帧听冲突
	*/
	void DRAConflictListener(int TTI, std::vector<sEvent>& systemEventVec);

	/*
	* 时延统计
	* 统计资源占用的累计TTI
	* 统计等待累计TTI
	* 在DRASelectBasedOnP..之后对其调用
	* 此时所有已触发的事件存在于WaitEventIdList中，或者存在于TransimitScheduleInfoList中，或者ScheduleInfoTable中
	*/
	void DRADelaystatistics(int TTI,std::vector<sEvent>& systemEventVec);

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


private:
	/*实现函数*/

	/*
	* 在DRAGroupSizeBasedTDM(...)内部被调用
	* 用于求出vector容器最大值的索引
	*/
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);

	/*
	* 用于查找指定VeUEId所属的簇编号
	* 不要用这个函数来判断VeUE是否属于该RSU（直接用VeUE对象的RSU编号来判断即可）
	* 这个函数的使用前提是，已知车辆已在簇中
	*/
	int DRAGetClusterIdxOfVeUE(int VeUEId);

	/*
	* 计算当前事件所对应的调度区间
	* 在函数DRASelectBasedOnP123(...)内部被调用
    */
	std::list<std::tuple<int, int>> DRABuildScheduleIntervalList(int TTI,const sEvent& event, std::tuple<int, int, int>ClasterTTI);
	std::list<std::tuple<int, int>> DRABuildEmergencyScheduleInterval(int TTI, const sEvent& event);


	/*
	* 将AdmitEventIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToAdmitEventIdList(int eventId);
	void DRAPushToEmergencyAdmitEventIdList(int eventId);

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToWaitEventIdList(int eventId);
	void DRAPushToEmergencyWaitEventIdList(int eventId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToSwitchEventIdList(int eventId, std::list<int>& systemDRASwitchVeUEIdList);

	/*
	* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToTransmitScheduleInfoList(sDRAScheduleInfo* p,int patternIdx);
	void DRAPushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx);

	/* 
	* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToScheduleInfoTable(int clusterIdx,int patternIdx, sDRAScheduleInfo*p);
	void DRAPushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p);

	/*
	* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPullFromScheduleInfoTable(int TTI);
	void DRAPullFromEmergencyScheduleInfoTable();

};

/*----------------------------------------------------
*                      RR调度
* ---------------------------------------------------*/

inline
void cRSU::RRPushToWaitEventIdList(int eventId) {
	m_RRWaitEventIdList.push_back(eventId);
}

inline
void cRSU::RRPushToEmergencyWaitEventIdList(int eventId) {
	m_RREmergencyWaitEventIdList.push_back(eventId);
}

inline
void cRSU::RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}


/*----------------------------------------------------
*                   分布式资源管理
*          DRA:Distributed Resource Allocation
* ---------------------------------------------------*/
inline
void cRSU::DRAPushToAdmitEventIdList(int eventId) {
	m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToEmergencyAdmitEventIdList(int eventId) {
	m_DRAEmergencyAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToWaitEventIdList(int eventId) {
	m_DRAWaitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToEmergencyWaitEventIdList(int eventId) {
	m_DRAEmergencyWaitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void cRSU::DRAPushToTransmitScheduleInfoList(sDRAScheduleInfo* p,int patternIdx) {
	m_DRATransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void cRSU::DRAPushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx) {
	m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void cRSU::DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, sDRAScheduleInfo*p) {
	m_DRAScheduleInfoTable[clusterIdx][patternIdx] = p;
}

inline
void cRSU::DRAPushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p) {
	m_DRAEmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void cRSU::DRAPullFromScheduleInfoTable(int TTI) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	/*将处于调度表中当前可以传输的信息压入m_DRATransimitEventIdList*/
	for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
		if (m_DRAScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
			m_DRATransimitScheduleInfoList[patternIdx].push_back(m_DRAScheduleInfoTable[clusterIdx][patternIdx]);
			m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
		}
	}
}

inline
void cRSU::DRAPullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0;patternIdx < gc_DRAEmergencyTotalPatternNum;patternIdx++) {
		if (m_DRAEmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(m_DRAEmergencyScheduleInfoTable[patternIdx]);
			m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}