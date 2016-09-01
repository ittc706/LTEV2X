#pragma once

#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <tuple>
#include "Schedule.h"
#include "Global.h"
#include "VUE.h"
#include "Traffic.h"
#include "IMTA.h"

class cRSU {
public:
	cRSU() {}
	/*--------------------------------------------------------------
	*                      地理拓扑单元
	* -------------------------------------------------------------*/

	/*------------------数据成员------------------*/
	int m_RSUId;
	std::list<int> m_VeUEIdList;//当前RSU范围内的VeUEId编号容器
	int m_DRAClusterNum;//一个RSU覆盖范围内的簇的个数
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;//存放每个簇的VeUE的Id的容器,下标代表簇的编号
	float m_fAbsX;
	float m_fAbsY;
	//cChannelModel *channelModel;
	cIMTA *imta;
	float m_fantennaAngle;

	/*------------------成员函数------------------*/
	void initialize(sRSUConfigure &t_RSUConfigure);

	/*--------------------------------------------------------------
	*                      无线资源管理单元
	* -------------------------------------------------------------*/

	/*----------------------------------------------------
	*                      RR调度
	* ---------------------------------------------------*/
	/*------------------数据成员------------------*/
	int m_RRNumRBPerPattern = 5;
	int m_RRPatternNum = gc_DRATotalBandwidth / gc_BandwidthOfRB / m_RRNumRBPerPattern;

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
public:
	/*接口函数*/
	void RRInformationClean();//资源分配信息清空

	/*
	* 用于处理System级别的事件链表，将事件转存入RSU级别的等待链表中
	*/
	void RRProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList);


	/*
	* 当发生位置更新后，处理等待链表
	*/
	void RRProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList);

	/*
	* 用于处理System级别的RSU切换链表，将事件转存入相应的链表中（RSU级别的接纳链表或者RSU级别的等待链表）
	* 处理完毕后，该链表的大小为0
	*/
	void RRProcessSwitchListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList);

	/*
	* 用于处理RSU级别的等待链表，
	* 将发生了RSU切换的事件推送到System级别的RSU切换链表中
	*/
	void RRProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);


	void RRProcessTransimit1(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* 时延统计
	* 统计资源占用的累计TTI
	* 统计等待累计TTI
	* 在DRASelectBasedOnP..之后对其调用
	* 此时所有已触发的事件存在于WaitEventIdList中，或者存在于TransimitScheduleInfoList中，或者ScheduleInfoTable中
	*/
	void RRDelaystatistics(int TTI, std::vector<sEvent>& systemEventVec);

	void RRProcessTransimit2(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* 将调度信息写入文件中，测试用！
	*/
	void RRWriteScheduleInfo(std::ofstream& out, int TTI);

	/*
	* 将以TTI为关键字的日志信息写入文件中，测试用！
	*/
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx);


private:
	/*实现函数*/


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

	/*----------------------------------------------------
	*                      PF调度
	* ---------------------------------------------------*/
	/*------------------数据成员------------------*/
public:
	double m_AccumulateThroughput;   //累计吞吐量
	bool m_IsScheduledUL;    //UpLink是否在被调度
	bool m_IsScheduledDL;    //DownLink是否在被调度
	double m_FactorPF[gc_RBNum];
	double m_SINR[gc_RBNum];
	sFeedbackInfo m_FeedbackUL;//将要发送给基站端的反馈信息

};

/*----------------------------------------------------
*                      RR调度
* ---------------------------------------------------*/

inline
void cRSU::RRPushToAdmitEventIdList(int eventId) {
	m_RRAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::RRPushToWaitEventIdList(int eventId, eMessageType messageType) {
	if (messageType == EMERGENCY) {
		m_RRWaitEventIdList.insert(m_RRWaitEventIdList.begin(), eventId);
	}
	else if(messageType==PERIOD){
		m_RRWaitEventIdList.push_back(eventId);
	}
}


inline
void cRSU::RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}


/*----------------------------------------------------
*                   分布式资源管理
*          DRA:Distributed Resource Allocation
* ---------------------------------------------------*/
