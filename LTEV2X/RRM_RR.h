#pragma once
#include<vector>
#include<list>
#include"RRM.h"
#include"Exception.h"
#include"Global.h"
//RRM_DRA:Radio Resource Management Round-Robin

class RRM_RR :public RRM_Basic {
public:
	RRM_RR() = delete;
	RRM_RR(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput);

	/*------------------数据成员------------------*/
	std::list<int> m_RRSwitchEventIdList;//用于存放进行RSU切换的车辆，暂时保存的作用

	int m_NewCount = 0;//记录动态创建的对象的次数

	int m_DeleteCount = 0;//记录删除动态创建对象的次数

	/*------------------成员函数------------------*/
	void initialize() override;//初始化RSU VeUE内该单元的内部类
	void cleanWhenLocationUpdate()override;//当发生位置更新时，清除缓存的调度相关信息
	void schedule() override;//DRA调度总控，覆盖基类的虚函数

	void RRInformationClean();//资源分配信息清空

	void RRUpdateAdmitEventIdList(bool clusterFlag);//更新接入表
	void RRProcessEventList();
	void RRProcessWaitEventIdListWhenLocationUpdate();
	void RRProcessSwitchListWhenLocationUpdate();
	void RRProcessWaitEventIdList();

	void RRRoundRobin();//轮询调度，分配当前TTI的资源(就是更新ScheduleTable)
	void RRTransimitPreparation();//统计干扰信息
	void RRTransimitStart();//模拟传输开始，更新调度信息，累计吞吐量
	void RRWriteScheduleInfo(std::ofstream& out);//记录调度信息日志
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx);//以时间为单位记录日志
	void RRDelaystatistics();//时延统计

	void RRTransimitEnd();//模拟传输结束，即统计吞吐量(就是更新ScheduleTable)
};



