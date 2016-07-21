#pragma once
#include<vector>
#include<fstream>
#include"Schedule.h"
#include"Config.h"
#include"eNB.h"
#include"RSU.h"
#include"VUE.h"
#include"Event.h"


class cSystem{
private:
	/*------------------数据成员------------------*/
	sConfig m_Config;//系统配置参数
	int m_TTI;//当前的TTI时刻
	int m_NTTI;//仿真总共的TTI
	std::vector<ceNB> m_eNBVec;//基站容器
	std::vector<cRSU> m_RSUVec;//RSU容器
	std::vector<cVeUE> m_VeUEVec;//VeUE容器
	std::vector<sEvent> m_EventVec;//事件容器
	
	/*
	* 外层下标为时间槽（代表TTI）
	* 与事件容器不同，事件触发链表将相同时刻触发的事件的Id置于相同的时间槽中
	*/
	std::vector<std::list<int>> m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	
public:
	/*------------------系统流程控制------------------*/
	void configure();//系统仿真参数配置
	void initialization();//系统参数配置，完成系统初始化
	void destroy();//释放资源
	void process();//系统仿真流程

	

private:
	void buildEventList();


	/*--------------------------------------------------------------
	*                      上行调度
	* -------------------------------------------------------------*/
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



	/*--------------------------------------------------------------
	*                      分布式资源管理
	*            DRA:Distributed Resource Allocation
	* -------------------------------------------------------------*/
public :
	eDRAMode m_DRAMode;

	std::list<int> m_DRASwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用


	/*--------------------接口函数--------------------*/
	void DRASchedule();//DRA调度总控

private:
	/*--------------------实现函数--------------------*/
	void DRAInformationClean();//资源分配信息清空
	void DRAPerformCluster(bool clusterFlag);//对RSU内的VeUE进行分簇
	void DRAGroupSizeBasedTDM(bool clusterFlag);//基于簇大小的时分复用
	void DRAUpdateAdmitEventIdList(bool clusterFlag);//建立接纳链表


	void DRASelectBasedOnP13();//基于P1和P3的资源分配
	void DRASelectBasedOnP23();//基于P2和P3的资源分配
	void DRASelectBasedOnP123();//基于P1、P2和P3的资源分配

	void DRAWriteScheduleInfo();//记录调度信息日志
	void DRADelaystatistics();//时延统计
	void DRAConflictListener();//帧听冲突

	/*--------------------辅助函数--------------------*/
	void writeClusterPerformInfo(std::ofstream &out);//写入分簇信息的日志
	void writeEventListInfo(std::ofstream &out);//写入时间列表的信息
	void writeEventLogInfo(std::ofstream &out);//写入以事件的日志信息
	void writeVeUELocationUpdateLogInfo(std::ofstream &out); //写入地理位置更新日志
};



