#pragma once
#include<vector>
#include<random>
#include"RRM.h"
#include"RSU.h"
#include"VUE.h"
#include"Enumeration.h"
#include"Exception.h"
#include"WT.h"

//RRM_DRA:Radio Resource Management Distributed Resource Allocation

/*===========================================
*              模块常量定义
* ==========================================*/
const int gc_DRA_NTTI = 100; //所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)

const int gc_DRAEmergencyTotalPatternNum = 3;//传输紧急事件的Pattern数量
const int gc_DRAEmergencyRBNumPerPattern = 2;//每个紧急事件的Pattern占用的RB数量

const int gc_DRAPatternTypeNum = 2;//非紧急事件的Pattern的类型种类
const int gc_DRA_RBNumPerPatternType[gc_DRAPatternTypeNum] = { 5,10 };//每个Pattern种类所占的RB数量
const int gc_DRAPatternNumPerPatternType[gc_DRAPatternTypeNum] = { 6,2 };//在全频段每个Pattern种类对应的Pattern数量
const int gc_DRAPatternTypePatternIdxInterval[gc_DRAPatternTypeNum][2] = { {0,5},{6,7} };

const int gc_DRATotalPatternNum = [&]() {
	int res = 0;
	for (int num : gc_DRAPatternNumPerPatternType)
		res += num;
	return res;
}();//所有非EmergencyPattern类型的Pattern数量总和



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
*            调度信息数据结构
* ==========================================*/
struct sDRAScheduleInfo {
	int eventId;//事件编号
	int VeUEId;//车辆编号
	int RSUId;//RSU编号
	int patternIdx;//频域块编号
	int remainBitNum=-1;//剩余待传bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
	int transimitBitNum=-1;//当前传输的bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
	int occupiedTTINum = -1;//传输完还需占用的TTI数量

	sDRAScheduleInfo() {}
	sDRAScheduleInfo(int eventId, int VeUEId, int RSUId, int patternIdx) {
		this->eventId = eventId;
		this->VeUEId = VeUEId;
		this->RSUId = RSUId;
		this->patternIdx = patternIdx;
	}

	std::string toLogString(int n);

	/*
	* 生成表示调度信息的string对象
	* 包括事件的Id，车辆的Id，以及要传输该事件所占用的TTI区间
	*/
	std::string toScheduleString(int n);
};


/*===========================================
*                RSU适配器
* ==========================================*/
class RSUAdapterDRA {
public:
	cRSU& m_HoldObj;//该适配器持有的原RSU对象
	RSUAdapterDRA() = delete;
	RSUAdapterDRA(cRSU& _RSU);

	/*------数据成员(新增用于模块数据成员)--------*/

	/*
	* TDR:Time Domain Resource
	* 下标代表簇编号
	* tuple存储的变量的含义依次为：存储每个簇所分配时间数量区间的左端点，右端点以及区间长度
	*/
	std::vector<std::tuple<int, int, int>> m_DRAClusterTDRInfo;


	/*
	* Pattern块释是否可用的标记
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	* 若"m_DRAPatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
	*/
	std::vector<std::vector<bool>> m_DRAPatternIsAvailable;

	/*
	* 用于存放当前TTI的接纳事件链表
	*/
	std::list<int> m_DRAAdmitEventIdList;

	/*
	* RSU级别的等待列表
	* 存放的是VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发，但VeUE未在可发送消息的时段，转入等待链表
	*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
	*/
	std::list<int> m_DRAWaitEventIdList;

	/*
	* 存放调度调度信息
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	*/
	std::vector<std::vector<sDRAScheduleInfo*>> m_DRAScheduleInfoTable;

	/*
	* 当前时刻当前RSU内处于传输状态的调度信息链表
	* 外层下标代表Pattern编号
	* 内层用list用于处理冲突
	*/
	std::vector<std::list<sDRAScheduleInfo*>> m_DRATransimitScheduleInfoList;


	/*
	* Pattern块释是否可用的标记
	* 下标代表Pattern编号
	* 若"m_DRAPatternIsAvailable[j]==true"代表Pattern块j可用
	*/
	std::vector<bool> m_DRAEmergencyPatternIsAvailable;

	/*
	* 当前时刻等待接入的紧急事件列表
	*/
	std::list<int> m_DRAEmergencyAdmitEventIdList;

	/*
	* 当前时刻处于等待接入状态的紧急事件列表
	*/
	std::list<int> m_DRAEmergencyWaitEventIdList;

	/*
	* 当前时刻处于传输状态的紧急事件调度信息列表
	* 外层下标代表pattern编号
	*/
	std::vector<std::list<sDRAScheduleInfo*>> m_DRAEmergencyTransimitScheduleInfoList;

	/*
	* 当前时刻处于调度状态的紧急事件调度信息列表
	*/
	std::vector<sDRAScheduleInfo*> m_DRAEmergencyScheduleInfoTable;


	/*------------------成员函数------------------*/

	std::string toString(int n);

	int DRAGetClusterIdx(int TTI);

	/*
	* 用于查找指定VeUEId所属的簇编号
	* 不要用这个函数来判断VeUE是否属于该RSU（直接用VeUE对象的RSU编号来判断即可）
	* 这个函数的使用前提是，已知车辆已在簇中
	*/
	int DRAGetClusterIdxOfVeUE(int VeUEId);

	/*
	* 将AdmitEventIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToAdmitEventIdList(int eventId);
	void DRAPushToEmergencyAdmitEventIdList(int eventId);

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToWaitEventIdList(int eventId);
	void DRAPushToEmergencyWaitEventIdList(int eventId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToSwitchEventIdList(int eventId, std::list<int>& systemDRASwitchVeUEIdList);

	/*
	* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx);
	void DRAPushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx);

	/*
	* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, sDRAScheduleInfo*p);
	void DRAPushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p);

	/*
	* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPullFromScheduleInfoTable(int TTI);
	void DRAPullFromEmergencyScheduleInfoTable();
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
		WT_Basic* systemWTPoint);

	std::vector<RSUAdapterDRA> m_RSUAdapterVec;
	std::vector<VeUEAdapterDRA> m_VeUEAdapterVec;


	/*------------------数据成员------------------*/

	eDRAMode m_DRAMode;//资源快选择的策略
	WT_Basic* m_WTPoint;
	std::list<int> m_DRASwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用

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

	void DRATransimitStart();//模拟传输开始，更新调度信息
	void DRATransimitEnd();//模拟传输结束，即统计吞吐量

	//日志记录函数
	void DRAWriteScheduleInfo(std::ofstream& out);//记录调度信息日志
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, eEventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx);
	void DRAWriteClusterPerformInfo(std::ofstream &out);//写入分簇信息的日志


	//工具函数
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);
	int getPatternType(int patternIdx);
	std::pair<int, int> DRAGetOccupiedRBRange(eMessageType messageType, int patternIdx);
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


inline
int RSUAdapterDRA::DRAGetClusterIdxOfVeUE(int VeUEId) {
	int dex = -1;
	for (int clusterIdx = 0; clusterIdx < m_HoldObj.m_DRAClusterNum; clusterIdx++) {
		for (int Id : m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx])
			if (Id == VeUEId) return clusterIdx;
	}
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)：该车不在当前RSU中");
}


inline
void RSUAdapterDRA::DRAPushToAdmitEventIdList(int eventId) {
	m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void RSUAdapterDRA::DRAPushToEmergencyAdmitEventIdList(int eventId) {
	m_DRAEmergencyAdmitEventIdList.push_back(eventId);
}

inline
void RSUAdapterDRA::DRAPushToWaitEventIdList(int eventId) {
	m_DRAWaitEventIdList.push_back(eventId);
}

inline
void RSUAdapterDRA::DRAPushToEmergencyWaitEventIdList(int eventId) {
	m_DRAEmergencyWaitEventIdList.push_back(eventId);
}

inline
void RSUAdapterDRA::DRAPushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void RSUAdapterDRA::DRAPushToTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx) {
	m_DRATransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void RSUAdapterDRA::DRAPushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx) {
	m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void RSUAdapterDRA::DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, sDRAScheduleInfo*p) {
	m_DRAScheduleInfoTable[clusterIdx][patternIdx] = p;
}

inline
void RSUAdapterDRA::DRAPushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p) {
	m_DRAEmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void RSUAdapterDRA::DRAPullFromScheduleInfoTable(int TTI) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	/*将处于调度表中当前可以传输的信息压入m_DRATransimitEventIdList*/
	for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
		if (m_DRAScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
			m_DRATransimitScheduleInfoList[patternIdx].push_back(m_DRAScheduleInfoTable[clusterIdx][patternIdx]);
			m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
		}
	}
}

inline
void RSUAdapterDRA::DRAPullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
		if (m_DRAEmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(m_DRAEmergencyScheduleInfoTable[patternIdx]);
			m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}