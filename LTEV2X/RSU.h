#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include"Schedule.h"
#include"Global.h"

class cRSU {
	//-----------------------TEST-----------------------
public:
	static int count;
	cRSU();
	void print();
	void Cluster();
	std::string path = "E:\\LTEV2X\\output\\DRAScheduleInfo.txt";
	//-----------------------TEST-----------------------
public:

	int m_RSUId;
	std::vector<int> m_VecVUE;//当前RSU范围内的UEid编号容器

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
	eRSUType m_RSUType;   //RSU的类型
	const int m_ClusterNum;  //一个RSU覆盖范围内的簇的个数（与RSU的类型有关）
	std::vector<std::tuple<int,int,int>> m_DRAClusterTTI;//存储每个簇所分配时间数量区间的左端点，右端点以及区间长度
	std::vector<std::vector<int>> m_DRA_RBIsAvailable;  //若"m_DRA_CNTI<=m_DRA_RBIsAvailable[i][j]"代表簇i的资源块j可用;内层vector存储的是对应资源块解除占用的时刻
	std::vector<std::vector<int>> m_Cluster;   //存放簇的容器，每个簇包含一个vector<int>存储车辆的ID
	std::vector<std::vector<int>> m_CallList;   //外层vector代表一个簇，内层vector<int>代表要传输数据的车辆ID
	std::vector<std::vector<sDRAScheduleInfo>> m_DRAScheduleList;  //当前调度信息，[i][j]代表第i个簇的第j个RB块


	/*--------------------接口函数--------------------*/
	int getDRAClusterIdx();//根据此刻的m_DRA_CNTI返回当前可以进行资源分配的簇的编号
	void DRAPerformCluster();//进行分簇，并给每个簇分配对应的时域资源
	void writeDRAScheduleInfo();
	/*--------------------辅助函数--------------------*/
private:
	int getMaxIndex(const std::vector<double>&v);
};