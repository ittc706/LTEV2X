#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include"Schedule.h"
#include"Global.h"
#include"VUE.h"
#include"Event.h"

class cRSU {
	//-----------------------TEST-----------------------
public:
	static int count;
	cRSU();
	
	//-----------------------TEST-----------------------
public:
	/*
	* 当前RSU的ID
	*/
	const int m_RSUId=count++;

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
	* FB块释放该资源的TTI时刻（ATTI）（该TTI结束时解除）
	* 外层下标代表簇编号
	* 内层下标代表FB块编号
	* 若"TTI>m_DRA_RBIsAvailable[i][j]"代表簇i的资源块j可用
	*/
	std::vector<std::vector<bool>> m_DRA_RBIsAvailable;  

	/*
	* 存放VeUE的ID的容器
	* 下标代表簇的编号
	*/
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;  

	/*
	* 用于存放当前TTI的接纳事件链表
	*/
	std::vector<int> m_DRAAdmissionEventIdList; 

	/*
	* 存放调度调度信息
	* 外层下标代表簇编号
	* 内层下标代表FB编号
	*/
	std::vector<std::vector<sDRAScheduleInfo*>> m_DRAScheduleInfoTable;
	
	/*
	* 当前时刻当前RSU内处于传输状态的事件链表
	* 外层下标代表FB编号
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
	std::list<int> m_DRAWaitingEventIdList;

	/*--------------------接口函数--------------------*/
	/*
	* 根据此刻的TTI返回当前进行资源分配的簇的编号
	*/
	int DRAGetClusterIdx(int ATTI);

	/*
	* 资源分配信息清空
	*/
	void DRAInformationClean();

	/*
	* 基于簇大小的时分复用
	* 每个簇至少分配一个FB
	* 剩余FB按比例进行分配
	*/
	void DRAGroupSizeBasedTDM();

	/*
	* 在System级别的函数内部被调用
	* 用于处理System级别的事件链表，将事件转存入相应的链表中（RSU级别的接纳链表或者RSU级别的等待链表）
	*/
	void DRAProcessSystemLevelEventList(int ATTI, int STTI, const std::vector<cVeUE>& systemVeUEVec, const std::vector<sEvent>& systemEventList, const std::vector<std::list<int>>& systemEventTTIList);

	/*
	* 在System级别的函数内部被调用
	* 用于处理RSU级别的调度表
	* 将发生了RSU切换的的事件推送到System级别的RSU切换链表中，因此要优先于DRAProcessSystemLevelRSUSwitchList的调用
	* 将发生了RSU内小簇切换的事件推送到RSU级别的等待链表中，因此要优先于DRAProcessRSULevelWaitingVeUEIdList的调用
	*/
	void DRAProcessRSULevelScheduleInfoTable(int ATTI, const std::vector<cVeUE>& systemVeUEVec, const std::vector<sEvent>& systemEventVec, std::list<int> &systemDRA_RSUSwitchEventIdList);

	/*
	* 在System级别的函数内部被调用
	* 用于处理RSU级别的等待链表，
	* 将发生了RSU切换的事件推送到System级别的RSU切换链表中
	*/
	void DRAProcessRSULevelWaitingVeUEIdList(int ATTI, const std::vector<cVeUE>& systemVeUEVec, const std::vector<sEvent>& systemEventVec, std::list<int> &systemDRA_RSUSwitchEventIdList);


	/*
	* 在System级别的函数内部被调用
	* 用于处理System级别的RSU切换链表，将事件转存入相应的链表中（RSU级别的接纳链表或者RSU级别的等待链表）
	* 处理完毕后，该链表的大小为0
	*/
	void DRAProcessSystemLevelRSUSwitchList(int ATTI, const std::vector<cVeUE>& systemVeUEVec, const std::vector<sEvent>& systemEventVec, std::list<int> &systemDRA_RSUSwitchEventIdList);


	/*
	* 基于P1和P3的资源分配
	*/
	void DRASelectBasedOnP13(int ATTI,std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec);

	/*
	* 基于P2和P3的资源分配
	*/
	void DRASelectBasedOnP23(int ATTI,std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec);

	/*
	* 基于P1、P2和P3的资源分配
	*/
	void DRASelectBasedOnP123(int ATTI,std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec);


	/*
	* 帧听冲突
	*/
	void DRAConflictListener(int ATTI);

	/*
	* 将调度信息写入文件中，测试用！
	*/
	void DRAWriteScheduleInfo(std::ofstream& out,int ATTI);

	/*
	* 将处理流程中的调息写入文件中，测试用！
	*/
	void DRAWriteProcessInfo(std::ofstream& out,int type, const std::vector<sEvent>& systemEventVec);

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
	* 将RSU级别的CallVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToRSULevelAdmissionEventIdList(int eventId);

	/*
	* 将RSU级别的WaitingVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToRSULevelWaitingEventIdList(int eventId);

	/*
	* 将System级别的SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToSystemLevelRSUSwitchEventIdList(int eventId, std::list<int>& systemDRA_RSUSwitchVeUEIdList);

	/* 
	* 将System级别的
	*/

	void pushToScheduleInfoTable(int clusterIdx,int FBIdx, sDRAScheduleInfo*p);

	void pullFromScheduleInfoTable(int ATTI);
};