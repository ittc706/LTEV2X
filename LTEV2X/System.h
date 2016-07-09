#pragma once
#include<vector>
#include"Schedule.h"
#include"Config.h"
#include"VUE.h"
#include"eNB.h"
#include"Message.h"
#include"Event.h"


class cSystem{
	//-----------------------TEST-----------------------
public:
	void print();
	//-----------------------TEST-----------------------
private:
	/*------------------数据成员------------------*/
	sConfig m_Config;//系统配置参数
	std::vector<ceNB> m_VeceNB;//基站容器
	std::vector<cVeUE> m_VecVUE;//车辆容器
	std::vector<cRSU> m_VecRSU;//RSU容器
	
public:
	/*------------------系统流程控制------------------*/
	void configure();//系统仿真参数配置
	void initialization();//系统参数配置，完成系统初始化
	void destroy();//释放资源
	void process();//系统仿真流程




	/***************************************************************
	------------------------调度模块--------------------------------
	****************************************************************/
public:
	/*--------------------接口函数--------------------*/
	void centralizedSchedule();//调度总控制

private:
	/*--------------------私有实现函数--------------------*/
	void scheduleInfoClean();//清除当前扇区所有用户的调度信息
	void schedulePF_RP_CSI_UL();//上行PF-RP调度

	//线性时间选取算法
public:
	sPFInfo selectKthPF(std::vector<sPFInfo>& vecF, int k, int p, int r);
	int partition(std::vector<sPFInfo>& vecF, int p, int r);
	void exchange(std::vector<sPFInfo>& vecF, int i, int j);



	/***************************************************************
	---------------------分布式资源管理-----------------------------
	-------------DRA:Distributed Resource Allocation----------------
	****************************************************************/
public :
	eDRAMode m_DRAMode;
	/*--------------------接口函数--------------------*/
	void DRASchedule();

private:
	/*--------------------实现函数--------------------*/
	void DRAPerformCluster();//对RSU内的车辆进行分簇
	void DRAGroupSizeBasedTDM();//基于簇大小的时分复用
	void DRAInformationClean();//资源分配信息清空
	void DRAbuildCallList();//建立呼叫链表


	void DRABasedOnP13();
	void DRABasedOnP23();
	void DRABasedOnP123();

	/*--------------------辅助函数--------------------*/
};



