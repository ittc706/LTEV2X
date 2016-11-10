#pragma once
#include<vector>
#include<list>
#include<thread>
#include"RRM.h"
#include"GTT.h"
#include"WT.h"
#include"Exception.h"
#include"Global.h"

//<RRM_RR>: Radio Resource Management Round-Robin

class RRM_RR :public RRM_Basic {
public:
	RRM_RR() = delete;
	RRM_RR(int &t_TTI, 
		Configure& t_Config, 
		RSU* t_RSUAry, 
		VeUE* t_VeUEAry, 
		std::vector<Event>& t_EventVec, 
		std::vector<std::list<int>>& t_EventTTIList, 
		std::vector<std::vector<int>>& t_TTIRSUThroughput,
		GTT_Basic* t_GTTPoint,
		WT_Basic* t_WTPoint,
		int t_ThreadNum
		);

	/*------------------数据成员------------------*/
	GTT_Basic* m_GTTPoint;//地理拓扑单元模块指针
	WT_Basic* m_WTPoint;//无线传输单元模块指针

	std::list<int> m_SwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用

	/*
	* 用于存放指定Pattern的干扰列表(同RSU下的簇间干扰或者RSU间干扰)
	* 外层下标为PatternId(绝对量)
	*/
	std::vector<std::list<int>> m_InterferenceVec;


	//多线程有关参数
	int m_ThreadNum;
	std::vector<std::thread> m_Threads;
	std::vector<std::pair<int, int>> m_ThreadsRSUIdRange;

	/*------------------成员函数------------------*/
	void initialize() override;//初始化RSU VeUE内该单元的内部类
	void cleanWhenLocationUpdate()override;//当发生位置更新时，清除缓存的调度相关信息
	void schedule() override;//DRA调度总控，覆盖基类的虚函数

private:
	void informationClean();//资源分配信息清空

	void updateAdmitEventIdList(bool t_ClusterFlag);//更新接入表
	void processEventList();
	void processWaitEventIdListWhenLocationUpdate();
	void processSwitchListWhenLocationUpdate();

	void roundRobin();//轮询调度，分配当前TTI的资源(就是更新ScheduleTable)
	void delaystatistics();//时延统计

	void transimitPreparation();//统计干扰信息
	void transimitStart();//模拟传输开始，更新调度信息，累计吞吐量
	void transimitStartThread(int t_FromRSUId, int t_ToRSUId);//模拟传输开始，更新调度信息
	void transimitEnd();//模拟传输结束，即统计吞吐量(就是更新ScheduleTable)

	void writeScheduleInfo(std::ofstream& t_File);//记录调度信息日志
	void writeTTILogInfo(std::ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);//以时间为单位记录日志
	void writeClusterPerformInfo(bool isLocationUpdate, std::ofstream& t_File);//写入分簇信息的日志

	std::pair<int, int> getOccupiedSubCarrierRange(int t_PatternIdx);
};



