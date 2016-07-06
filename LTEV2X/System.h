#pragma once
#include<vector>
#include"Schedule.h"
#include"Config.h"
#include"VUE.h"
#include"Sector.h"
#include"eNB.h"
#include"Message.h"


class cSystem{
private:
	/*------------------数据成员------------------*/
	sConfig m_Config;//系统配置参数
	int m_TTI;//仿真目前运行到的TTI
	std::vector<Sector> m_VecSector;//扇区容器
	std::vector<ceNB> m_VeceNB;//基站容器
	std::vector<cVeUE> m_VecVUE;//车辆容器
	std::vector<cRSU> m_VecRSU;
	EventList m_EventListCallSetup;//呼叫发起事件链表
	EventList m_EventListHandover;//切换事件链表
	EventList m_EventListCac;//接纳控制事件链表
	EventList m_EnentListCallEnd;//呼叫结束事件链表


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
	void scheduleDistribute();//把部分调度信息写到UE中
	void scheduleInfoClean();//清除当前扇区所有用户的调度信息
	void feedbackInfoReceived();//将反馈信息写入基站端
	void feedbackInfoDelay();
	void feedbackInfoSent();//将反馈信息写入到用户反馈读指针里
	void schedulePF_RP_CSI_UL();//上行PF-RP调度
	void scheduleMCS();//确定所有调度用户的MCS
	void scheduleInfoDelay();

	//线性时间选取算法
public:
	PFInfo selectKthPF(std::vector<PFInfo>& vecF, int k, int p, int r);
	int partition(std::vector<PFInfo>& vecF, int p, int r);
	void exchange(std::vector<PFInfo>& vecF, int i, int j);



	/***************************************************************
	---------------------分布式资源管理-----------------------------
	****************************************************************/
public :
	/*--------------------接口函数--------------------*/
	void distributedSchedule();

private:
	void performCluster();//对RSU内的车辆进行分簇
	void frequencyResourceSelect();//对每个RSU内的小簇（该小簇指：在当前RSU_TII进行资源竞争的小簇）进行资源选择
};



