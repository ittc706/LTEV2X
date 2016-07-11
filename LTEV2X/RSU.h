#pragma once

#include<vector>
#include<set>
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
	void print();
	void testCluster();
	//-----------------------TEST-----------------------
public:

	int m_RSUId;
	std::set<int> m_VUESet;//当前RSU范围内的VEId编号容器

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
	* RSU的类型
	* 处于十字路口的RSU
	* 分布于道路中间的RSU
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
	* 存放车辆的容器
	* 下标代表簇的编号
	* 簇内车辆用set来维护，用于利用内置红黑树数据结构判断事件链表中对应事件的车辆ID是否存在于该set中，提高效率
	*/
	std::vector<std::set<int>> m_DRAClusterVUESet;  

	/*
	* 用于存放当前TTI呼叫车辆的编号
	*/
	std::vector<int> m_DRACallList; 

	/*
	* 存放调度调度信息
	* 外层下标代表簇编号
	* 内层下标代表FB编号
	*/
	std::vector<std::vector<std::list<sDRAScheduleInfo>>> m_DRAScheduleList;
	
	/*
	* 冲突列表
	* 列表元素是tuple，分别代表VEId,ClusterIdx以及FBIdx
	* 其中ClusterIdx以及FBIdx是用于冲突处理释放对应的频域资源
	*/
	std::set<std::tuple<int,int,int>> m_DRAConflictSet;

	/*--------------------接口函数--------------------*/
	int DRAGetClusterIdx(int TTI);//根据此刻的TTI返回当前可以进行资源分配的簇的编号
	void DRAInformationClean();//资源分配信息清空
	void DRAPerformCluster();//进行分簇
	void DRAGroupSizeBasedTDM();//基于簇大小的时分复用
	void DRABuildCallList(int TTI,const std::vector<std::list<sEvent>>& eventList);//建立呼叫链表
	
	void DRASelectBasedOnP13(int TTI,std::vector<cVeUE>&v);//基于P1和P3的资源分配
	void DRASelectBasedOnP23(int TTI,std::vector<cVeUE>&v);//基于P2和P3的资源分配
	void DRASelectBasedOnP123(int TTI,std::vector<cVeUE>&v);//基于P1、P2和P3的资源分配

	void DRAReaddConflictListToCallList(int TTI);//将上一个TTI冲突的用户()重新添加到呼叫链表中
	void DRAWriteScheduleInfo(std::ofstream& out);//写调度信息
	void DRAConflictListener(int TTI);//帧听冲突
	void DRAConflictSolve(int TTI);//维护m_DRAScheduleList以及m_DRA_RBIsAvailable
	/*--------------------辅助函数--------------------*/
private:
	int getMaxIndex(const std::vector<double>&v);
};