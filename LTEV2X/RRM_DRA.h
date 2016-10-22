#pragma once
#include<vector>
#include<random>
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
enum eDRAMode {
	//P1:  Collision avoidance based on sensing
	//P2:  Enhanced random resource selection
	//P3:  Location-based resource selection
	P13,               //Combination of P1 and P3
	P23,               //Combination of P2 and P3
	P123               //Combination of P1 and P2 and P3
};



/*===========================================
*                VeUE适配器
* ==========================================*/
class VeUEAdapterDRA {
public:
	static std::default_random_engine s_Engine;

	cVeUE& m_HoldObj;//该适配器持有的原VeUE对象
	VeUEAdapterDRA() = delete;
	VeUEAdapterDRA(cVeUE& _VeUE) :m_HoldObj(_VeUE) {}

	/*------数据成员(新增用于模块数据成员)--------*/

	std::tuple<int, int> m_ScheduleInterval;//该VeUE所在簇的当前一轮调度区间


	/*------------------成员函数------------------*/

	int DRARBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType);
	int DRARBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx);

	std::string toString(int n);//用于打印VeUE信息
};




/*===========================================
*                DRA模块
* ==========================================*/
class RRM_DRA :public RRM_Basic {
public:
	RRM_DRA() = delete;
	RRM_DRA(int &systemTTI, 
		sConfigure& systemConfig, 
		cRSU* systemRSUAry, 
		cVeUE* systemVeUEAry, 
		std::vector<sEvent>& systemEventVec, 
		std::vector<std::list<int>>& systemEventTTIList,
		std::vector<std::vector<int>>& systemTTIRSUThroughput,
		eDRAMode systemDRAMode,
		WT_Basic* systemWTPoint,
		GTAT_Basic* systemGTATPoint
	);

	std::vector<VeUEAdapterDRA> m_VeUEAdapterVec;


	/*------------------数据成员------------------*/

	eDRAMode m_DRAMode;//资源快选择的策略
	GTAT_Basic* m_GTATPoint;//地理拓扑单元模块指针
	WT_Basic* m_WTPoint;//无线传输单元模块指针
	std::list<int> m_DRASwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用

	/*
	* 用于存放指定Pattern的车辆的编号列表，外层代表Pattern编号
	* 外层下标存放Pattern，注意非紧急事件要加上偏移量，因为非紧急事件的Pattern也是从0开始编号，但是这里要存入的是Pattern编号的绝对值
	*/
	std::vector<std::list<int>> m_DRAInterferenceVec;

	int m_NewCount = 0;//记录动态创建的对象的次数

	int m_DeleteCount = 0;//记录删除动态创建对象的次数

	/*------------------成员函数------------------*/

public:
	/*接口函数*/
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
	void DRATransimitEnd();//模拟传输结束，即统计吞吐量

	//日志记录函数
	void DRAWriteScheduleInfo(std::ofstream& out);//记录调度信息日志
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, eEventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx);
	void DRAWriteClusterPerformInfo(std::ofstream &out);//写入分簇信息的日志


	//工具函数
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);
	int DRAGetPatternType(int patternIdx);
	std::pair<int, int> DRAGetOccupiedSubCarrierRange(eMessageType messageType, int patternIdx);
};



inline
int VeUEAdapterDRA::DRARBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType) {
	int size = static_cast<int>(curAvaliablePatternIdx[messageType].size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return curAvaliablePatternIdx[messageType][u(s_Engine)];
}

inline
int VeUEAdapterDRA::DRARBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx) {
	int size = static_cast<int>(curAvaliableEmergencyPatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return curAvaliableEmergencyPatternIdx[u(s_Engine)];
}


