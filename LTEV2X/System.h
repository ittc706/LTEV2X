#pragma once
#include <vector>
#include <fstream>
#include "Schedule.h"
#include "Config.h"
#include "eNB.h"
#include "RSU.h"
#include "VUE.h"
#include "Traffic.h"
#include "Road.h"
#include "RRMBasic.h"

class cSystem{
public:

	/*--------------------------------------------------------------
	*                      全局控制单元
	* -------------------------------------------------------------*/

	/*------------------数据成员------------------*/
	int m_TTI;//当前的TTI时刻
	int m_NTTI;//仿真总共的TTI
	sConfigure m_Config;//系统参数配置
	ceNB* m_eNBAry;//基站容器
	cRoad *m_RoadAry;//道路容器
	cRSU* m_RSUAry;//RSU容器
	cVeUE* m_VeUEAry;//VeUE容器


	RRM_Basic* RRMPoint;

	/*------------------成员函数------------------*/
public:
	/*接口函数*/
	void configure();//系统仿真参数配置
	void initialization();//系统参数配置，完成系统初始化
	void RRMInitialization();
	void process();//系统仿真流程
	void dispose();//内存清理

	/*--------------------------------------------------------------
	*                      业务模型与控制单元
	* -------------------------------------------------------------*/
public:
	/*------------------数据成员------------------*/
	std::vector<sEvent> m_EventVec;//事件容器
	/*
	* 外层下标为时间槽（代表TTI）
	* 与事件容器不同，事件触发链表将相同时刻触发的事件的Id置于相同的时间槽中
	*/
	std::vector<std::list<int>> m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	
	std::vector<int> m_VeUEEmergencyNum;//每辆车紧急事件触发的次数，用于验证泊松分布，仿真中并无用处
	/*------------------成员函数------------------*/
	void buildEventList();
	void processStatistics();
	void writeEventListInfo(std::ofstream &out);//写入事件列表的信息
	void writeEventLogInfo(std::ofstream &out);//写入以事件的日志信息
	void writeVeUELocationUpdateLogInfo(std::ofstream &out); //写入地理位置更新日志



	/*--------------------------------------------------------------
	*                      地理拓扑单元
	* -------------------------------------------------------------*/
public:
	/*------------------数据成员------------------*/
	int m_AllUsers;
	int m_FreshNum;

	/*------------------成员函数------------------*/

	void channelGeneration();//信道产生与刷新
	void freshLoc(void);
	

	/*--------------------------------------------------------------
	*                      无线资源管理单元
	* -------------------------------------------------------------*/
	eScheduleMode m_ScheduleMode;

	/*----------------------------------------------------
	*                      RR调度
	* ---------------------------------------------------*/
	/*------------------数据成员------------------*/
	std::list<int> m_RRSwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用

	/*------------------成员函数------------------*/
public:
	/*接口函数*/
	void RRSchedule();//RR调度总控
private:
	/*实现函数*/
	void RRInformationClean();//资源分配信息清空
	void RRUpdateAdmitEventIdList(bool clusterFlag);//更新接入表

	void RRProcessTransimit1();
	void RRWriteScheduleInfo();//记录调度信息日志
	void RRDelaystatistics();//时延统计
	void RRProcessTransimit2();




	/*----------------------------------------------------
	*                      PF调度
	* ---------------------------------------------------*/

	/*------------------成员函数------------------*/
public:
	/*接口函数*/
	void PFSchedule();//调度总控制

private:
	/*实现函数*/
	void PFInformationClean();//清除当前扇区所有用户的调度信息
	void schedulePF_RP_CSI_UL();//上行PF-RP调度

	//线性时间选取算法
	sPFInfo selectKthPF(std::vector<sPFInfo>& vecF, int k, int p, int r);
	int partition(std::vector<sPFInfo>& vecF, int p, int r);
	void exchange(std::vector<sPFInfo>& vecF, int i, int j);

};



