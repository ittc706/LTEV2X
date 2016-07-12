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
	int m_RSUId;

	/*
	* 当前RSU范围内的VeUEId编号容器
	*/
	std::list<int> m_VeUEList;

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
	std::vector<std::vector<int>> m_DRA_RBIsAvailable;  

	/*
	* 存放VeUE的ID的容器
	* 下标代表簇的编号
	*/
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;  

	/*
	* 用于存放当前TTI呼叫VeUE的ID
	*/
	std::vector<int> m_DRACallVeUEIdList; 

	/*
	* 存放调度调度信息
	* 外层下标代表簇编号
	* 内层下标代表FB编号
	*/
	std::vector<std::vector<std::list<sDRAScheduleInfo>>> m_DRAScheduleList;
	
	/*
	* 冲突列表
	* 列表元素是tuple，分别代表VeUEId,ClusterIdx以及FBIdx，其中ClusterIdx以及FBIdx是用于冲突处理释放对应的频域资源
	*/
	std::list<std::tuple<int,int,int>> m_DRAConflictInfoList;


	/*
	* RSU级别的等待列表
	* 存放的是VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发，但VeUE未在可发送消息的时段，转入等待链表
	*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
	*/
	std::list<int> m_DRAWaitingVeUEIdList;

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
	* 每个簇至少分配一个FB
	* 剩余FB按比例进行分配
	*/
	void DRAGroupSizeBasedTDM();

	/*
	* 在System级别的函数内部被调用
	* 用于处理System级别的事件链表，将事件转存入相应的链表中（RSU级别的呼叫链表或者RSU级别的等待链表）
	*/
	void DRAProcessSystemLevelEventList(int TTI, int STTI, const std::vector<cVeUE>& systemVeUEVec, const std::vector<std::list<sEvent>>& systemEventList);//从System级别的RSU切换链表转入当前时刻的呼叫链表

	/*
	* 在System级别的函数内部被调用
	* 用于处理RSU级别的等待链表，将事件转存入相应的链表中（RSU级别的呼叫链表或者System级别的RSU切换链表）
	*/
	void DRAProcessRSULevelWaitingVeUEIdList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::list<int> &systemDRA_RSUSwitchVeUEIdList);//将上一个TTI冲突的用户()重新添加到呼叫链表中

	/*
	* 在System级别的函数内部被调用
	* 用于处理System级别的RSU切换链表，将事件转存入相应的链表中（RSU级别的呼叫链表或者RSU级别的等待链表）
	* 处理完毕后，该链表的大小为0
	*/
	void DRAProcessSystemLevelRSUSwitchList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::list<int> &systemDRA_RSUSwitchVeUEIdList);


	/*
	* 基于P1和P3的资源分配
	*/
	void DRASelectBasedOnP13(int TTI,std::vector<cVeUE>&systemVeUEVec);

	/*
	* 基于P2和P3的资源分配
	*/
	void DRASelectBasedOnP23(int TTI,std::vector<cVeUE>&systemVeUEVec);

	/*
	* 基于P1、P2和P3的资源分配
	*/
	void DRASelectBasedOnP123(int TTI,std::vector<cVeUE>&systemVeUEVec);

	/*
	* 帧听冲突
	*/
	void DRAConflictListener(int TTI);

	/*
	* 维护m_DRAScheduleList以及m_DRA_RBIsAvailable
	*/
	void DRAConflictSolve(int TTI);


	/*
	* 将调度信息写入文件中，测试用！
	*/
	void DRAWriteScheduleInfo(std::ofstream& out);

	/*
	* 将处理流程中的调息写入文件中，测试用！
	*/
	void DRAWriteProcessInfo(std::ofstream& out,int type);

	/*
	* 生成包含RSU信息的string
	*/
	std::string toString();

	/*--------------------辅助函数--------------------*/
private:
	/*
	* 在DRAGroupSizeBasedTDM(...)内部被调用
	* 用于求出vector容器最大值的索引
	*/
	int getMaxIndex(const std::vector<double>&v);

	/*
	* 将CallVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToRSULevelCallVeUEIdList(int VeUEId);

	/*
	* 将WaitingVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToRSULevelWaitingVeUEIdList(int VeUEId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToSystemLevelRSUSwitchVeUEIdList(int VeUEId, std::list<int>& systemDRA_RSUSwitchVeUEIdList);
};