#pragma once
#include<vector>
#include<random>
#include<thread>
#include"RRM.h"
#include"RSU.h"
#include"VUE.h"
#include"Enumeration.h"
#include"Exception.h"
#include"WT.h"
#include"GTAT.h"

//RRM_TDM_DRA:Radio Resource Management Distributed Resource Allocation



/*===========================================
*                RRM_TDM_DRA模块
* ==========================================*/
class RRM_TDM_DRA :public RRM_Basic {
public:
	RRM_TDM_DRA() = delete;
	RRM_TDM_DRA(int &systemTTI,
		Configure& systemConfig,
		RSU* systemRSUAry,
		VeUE* systemVeUEAry,
		std::vector<Event>& systemEventVec,
		std::vector<std::list<int>>& systemEventTTIList,
		std::vector<std::vector<int>>& systemTTIRSUThroughput,
		GTAT_Basic* systemGTATPoint,
		WT_Basic* systemWTPoint,
		int threadNum
		);

	/*------------------数据成员------------------*/

	GTAT_Basic* m_GTATPoint;//地理拓扑单元模块指针
	WT_Basic* m_WTPoint;//无线传输单元模块指针
	std::list<int> m_SwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用

	/*
	* 用于存放指定Pattern的干扰列表
	* 外层下标为PatternId(绝对量)
	*/
	std::vector<std::list<int>> m_InterferenceVec;

	//多线程有关参数
	int m_ThreadNum;
	std::vector<std::thread> m_Threads;
	std::vector<std::pair<int, int>> m_ThreadsRSUIdRange;

	/*------------------成员函数------------------*/

public:
	/*接口函数*/
	void initialize() override;//初始化RSU VeUE内该单元的内部类
	void cleanWhenLocationUpdate()override;//当发生位置更新时，清除缓存的调度相关信息
	void schedule() override;//RRM_TDM_DRA调度总控，覆盖基类的虚函数
private:
	/*实现函数*/
	void informationClean();//资源分配信息清空
	void groupSizeBasedTDM(bool clusterFlag);//基于簇大小的时分复用
	void uniformTDM(bool clusterFlag);//基于簇大小的时分复用

	void updateAdmitEventIdList(bool clusterFlag);//更新接纳链表
	void processEventList();
	void processScheduleInfoTableWhenLocationUpdate();
	void processWaitEventIdListWhenLocationUpdate();
	void processSwitchListWhenLocationUpdate();
	void processWaitEventIdList();

	void selectBasedOnP123();//基于P1、P2和P3的资源分配


	void delaystatistics();//时延统计
	void conflictListener();//帧听冲突

	void transimitPreparation();//统计干扰信息
	void transimitStart();//模拟传输开始，更新调度信息，累计吞吐量
	void transimitStartThread(int fromRSUId,int toRSUId);//模拟传输开始，更新调度信息
	void transimitEnd();//模拟传输结束

	//日志记录函数
	void writeScheduleInfo(std::ofstream& out);//记录调度信息日志
	void writeTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx);
	void writeClusterPerformInfo(std::ofstream &out);//写入分簇信息的日志


	//工具函数
	int getMaxIndex(const std::vector<double>&clusterSize);
	int getPatternType(int patternIdx);
	std::pair<int, int> getOccupiedSubCarrierRange(MessageType messageType, int patternIdx);

};

