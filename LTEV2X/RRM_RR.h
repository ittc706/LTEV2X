#pragma once
#include<vector>
#include<list>
#include"RRM.h"
#include"Exception.h"
#include"Global.h"

/*===========================================
*              模块常量定义
* ==========================================*/
const int gc_RRNumRBPerPattern = 5;//每个Pattern的RB数量
const int gc_RRPatternNum = gc_TotalBandwidth / gc_BandwidthOfRB / gc_RRNumRBPerPattern;//总的Pattern数量
const int gc_RRBitNumPerPattern = gc_RRNumRBPerPattern*gc_BitNumPerRB;//每个Pattern单位TTI传输的bit数量

/*===========================================
*            调度信息数据结构
* ==========================================*/
struct sRRScheduleInfo {
	int eventId;//事件编号
	eMessageType messageType;//事件类型
	int VeUEId;//车辆编号
	int RSUId;//RSU编号
	int patternIdx;//频域块编号
	int occupiedNumTTI;//当前VeUE进行传输的实际TTI区间（闭区间）

	sRRScheduleInfo() {}
	sRRScheduleInfo(int eventId, eMessageType messageType, int VeUEId, int RSUId, int patternIdx, const int &occupiedNumTTI) {
		this->eventId = eventId;
		this->messageType = messageType;
		this->VeUEId = VeUEId;
		this->RSUId = RSUId;
		this->patternIdx = patternIdx;
		this->occupiedNumTTI = occupiedNumTTI;
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
class RSUAdapterRR {
public:
	cRSU& m_HoldObj;//该适配器持有的原RSU对象
	RSUAdapterRR() = delete;
	RSUAdapterRR(cRSU& _RSU);

	/*------------------数据成员------------------*/
	

	std::vector<int> m_RRAdmitEventIdList;//当前TTI接入列表，最大长度不超过Pattern数量

	/*
	* RSU级别的等待列表
	* 存放的是VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发，转入等待链表
	*/
	std::list<int> m_RRWaitEventIdList;

	/*
	* 存放调度调度信息
	* 外层代表Pattern号
	*/
	std::vector<sRRScheduleInfo*> m_RRScheduleInfoTable;


	/*------------------成员函数------------------*/

	/*
	* 将AdmitEventIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void RRPushToAdmitEventIdList(int eventId);

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void RRPushToWaitEventIdList(int eventId, eMessageType messageType);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList);
};


/*===========================================
*                VeUE适配器
* ==========================================*/
class VeUEAdapterRR {
public:
	cVeUE& m_HoldObj;//该适配器持有的原VeUE对象
	VeUEAdapterRR() = delete;
	VeUEAdapterRR(cVeUE& _VeUE) :m_HoldObj(_VeUE) {}
};



class RRM_RR :public RRM_Basic {
public:
	RRM_RR() = delete;
	RRM_RR(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList);

	std::vector<RSUAdapterRR> m_RSUAdapterVec;
	std::vector<VeUEAdapterRR> m_VeUEAdapterVec;

	/*------------------数据成员------------------*/
	std::list<int> m_RRSwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用

	int m_NewCount = 0;//记录动态创建的对象的次数

	int m_DeleteCount = 0;//记录删除动态创建对象的次数

	/*------------------成员函数------------------*/
	void schedule() override;//DRA调度总控，覆盖基类的虚函数

	void RRInformationClean();//资源分配信息清空

	void RRUpdateAdmitEventIdList(bool clusterFlag);//更新接入表
	void RRProcessEventList();
	void RRProcessWaitEventIdListWhenLocationUpdate();
	void RRProcessSwitchListWhenLocationUpdate();
	void RRProcessWaitEventIdList();

	void RRTransimitBegin();//开始传输(就是更新ScheduleTable)

	void RRWriteScheduleInfo(std::ofstream& out);//记录调度信息日志
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx);//以时间为单位记录日志
	void RRDelaystatistics();//时延统计

	void RRTransimitEnd();//传输结束(就是更新ScheduleTable)
};



inline
void RSUAdapterRR::RRPushToAdmitEventIdList(int eventId) {
	m_RRAdmitEventIdList.push_back(eventId);
}

inline
void RSUAdapterRR::RRPushToWaitEventIdList(int eventId, eMessageType messageType) {
	if (messageType == EMERGENCY) {
		m_RRWaitEventIdList.insert(m_RRWaitEventIdList.begin(), eventId);
	}
	else if (messageType == PERIOD) {
		m_RRWaitEventIdList.push_back(eventId);
	}
}

inline
void RSUAdapterRR::RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}