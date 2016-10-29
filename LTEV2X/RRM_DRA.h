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

//RRM_DRA:Radio Resource Management Distributed Resource Allocation


/*===========================================
*        用于该模块的枚举类型定义
* ==========================================*/
enum DRAMode {
	//P1:  Collision avoidance based on sensing
	//P2:  Enhanced random resource selection
	//P3:  Location-based resource selection
	P13,               //Combination of P1 and P3
	P23,               //Combination of P2 and P3
	P123               //Combination of P1 and P2 and P3
};


/*===========================================
*                DRA模块
* ==========================================*/
class RRM_DRA :public RRM_Basic {
public:
	RRM_DRA() = delete;
	RRM_DRA(int &systemTTI, 
		Configure& systemConfig, 
		RSU* systemRSUAry, 
		VeUE* systemVeUEAry, 
		std::vector<Event>& systemEventVec, 
		std::vector<std::list<int>>& systemEventTTIList,
		std::vector<std::vector<int>>& systemTTIRSUThroughput,
		DRAMode systemDRAMode,
		WT_Basic* systemWTPoint,
		GTAT_Basic* systemGTATPoint,
		int threadNum
	);

	/*------------------数据成员------------------*/

	DRAMode m_DRAMode;//资源快选择的策略
	GTAT_Basic* m_GTATPoint;//地理拓扑单元模块指针
	WT_Basic* m_WTPoint;//无线传输单元模块指针
	std::list<int> m_DRASwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用

	/*
	* 用于存放指定Pattern的[事件类型信息,该Pattern的干扰列表]
	* 外层下标为PatternId(绝对量)
	*/
	std::vector<std::list<int>> m_DRAInterferenceVec;
	
	int m_NewCount = 0;//记录动态创建的对象的次数

	int m_DeleteCount = 0;//记录删除动态创建对象的次数

	//多线程有关参数
	int m_ThreadNum;
	//std::vector<std::thread> m_Threads;
	std::vector<std::pair<int, int>> m_ThreadsRSUIdRange;

	/*------------------成员函数------------------*/

public:
	/*接口函数*/
	void initialize() override;//初始化RSU VeUE内该单元的内部类
	void cleanWhenLocationUpdate()override;//当发生位置更新时，清除缓存的调度相关信息
	void schedule() override;//DRA调度总控，覆盖基类的虚函数
private:
	/*实现函数*/
	void DRAInformationClean();//资源分配信息清空
	void DRAGroupSizeBasedTDM(bool clusterFlag);//基于簇大小的时分复用

	void DRAUpdateAdmitEventIdList(bool clusterFlag);//更新接纳链表
	void DRAProcessEventList();
	void DRAProcessScheduleInfoTableWhenLocationUpdate();
	void DRAProcessWaitEventIdListWhenLocationUpdate();
	void DRAProcessSwitchListWhenLocationUpdate();
	void DRAProcessWaitEventIdList();


	void DRASelectBasedOnP13();//基于P1和P3的资源分配
	void DRASelectBasedOnP23();//基于P2和P3的资源分配
	void DRASelectBasedOnP123();//基于P1、P2和P3的资源分配


	void DRADelaystatistics();//时延统计
	void DRAConflictListener();//帧听冲突

	void DRATransimitPreparation();//统计干扰信息
	void DRATransimitStart();//模拟传输开始，更新调度信息
	void DRATransimitStartThread(int fromRSUId,int toRSUId);//模拟传输开始，更新调度信息
	void DRATransimitEnd();//模拟传输结束，即统计吞吐量

	//日志记录函数
	void DRAWriteScheduleInfo(std::ofstream& out);//记录调度信息日志
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx);
	void DRAWriteClusterPerformInfo(std::ofstream &out);//写入分簇信息的日志


	//工具函数
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);
	int DRAGetPatternType(int patternIdx);
	std::pair<int, int> DRAGetOccupiedSubCarrierRange(MessageType messageType, int patternIdx);


	void f() {
		;
	}
};

