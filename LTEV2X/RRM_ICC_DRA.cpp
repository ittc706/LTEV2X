/*
* =====================================================================================
*
*       Filename:  RRM_ICC_DRA.cpp
*
*    Description:  RRM_ICC_DRA模块
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/


#include"RRM_ICC_DRA.h"

using namespace std;

RRM_ICC_DRA::RRM_ICC_DRA(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {

	m_InterferenceVec = vector<list<int>>(ns_RRM_ICC_DRA::gc_TotalPatternNum);
	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = m_Config.RSUNum - 1;//修正最后一个边界
}


void RRM_ICC_DRA::initialize() {
	//初始化VeUE的该模块参数部分
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeRRM_ICC_DRA();
	}

	//初始化RSU的该模块参数部分
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeRRM_ICC_DRA();
	}
}


void RRM_ICC_DRA::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();

		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(ns_RRM_ICC_DRA::gc_TotalPatternNum, false);
	}
}


void RRM_ICC_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//资源分配信息清空:包括每个RSU内的接入链表等
	informationClean();

	//建立接纳链表
	updateWaitEventIdList(clusterFlag);

	//资源选择
	selectBasedOnP123();

	//统计时延信息
	delaystatistics();

	//帧听冲突
	conflictListener();

	//请求地理拓扑单元计算干扰响应矩阵
	transimitPreparation();

	//模拟传输开始，更新调度信息
	transimitStart();

	//写入调度信息
	writeScheduleInfo(g_FileDRAScheduleInfo);

	//模拟传输结束，统计吞吐量
	transimitEnd();
}


void RRM_ICC_DRA::informationClean() {

}


void RRM_ICC_DRA::updateWaitEventIdList(bool t_ClusterFlag) {
	//首先，处理System级别的事件触发链表
	processEventList();
	//其次，如果当前TTI进行了位置更新，需要处理调度表
	if (t_ClusterFlag) {
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
		//处理RSU级别的调度链表
		processScheduleInfoTableWhenLocationUpdate();

		//处理RSU级别的等待链表
		processWaitEventIdListWhenLocationUpdate();

		//处理System级别的切换链表
		processSwitchListWhenLocationUpdate();
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
	}
}


void RRM_ICC_DRA::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event &event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		
		//将该事件压入等待链表
		_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx,eventId);

		//更新该事件的日志
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Trigger");

		//记录TTI日志
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1);
	}
}


void RRM_ICC_DRA::processScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx] == nullptr) continue;

				int eventId = _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
					//压入Switch链表
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//将剩余待传bit重置
					m_EventVec[eventId].message.reset();

					//并释放该调度信息的资源
					delete _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx];
					_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

					//释放Pattern资源
					_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "LocationUpdate");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx);
				}
				else {
					if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//RSU内部发生了簇切换，将其从调度表中取出，压入等待链表
						//压入该RSU的等待链表
						_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId);

						//并释放该调度信息的资源
						delete _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx];
						_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

						//释放Pattern资源
						_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

						//更新该事件的日志
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "LocationUpdate");

						//记录TTI日志
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx);
					}
				}
			}
		}
	}
}


void RRM_ICC_DRA::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//开始处理 m_WaitEventIdList
			list<int>::iterator it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE已经不在该RSU范围内
					//将其添加到System级别的RSU切换链表中
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//将其从等待链表中删除
					it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].erase(it);

					//将剩余待传bit重置
					m_EventVec[eventId].message.reset();

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "LocationUpdate");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
				}
				else {//仍然处于当前RSU范围内
					it++;
					continue; //继续留在当前RSU的等待链表
				}
			}
		}
	}
}


void RRM_ICC_DRA::processSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_SwitchEventIdList.begin();
	while (it != m_SwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		
		//转入等待链表
		_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx,eventId);

		//从Switch表中将其删除
		it = m_SwitchEventIdList.erase(it);

		//更新该事件的日志
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "LocationUpdate");

		//记录TTI日志
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1);
	}
}



void RRM_ICC_DRA::selectBasedOnP123() {

}


void RRM_ICC_DRA::delaystatistics() {

}


void RRM_ICC_DRA::conflictListener() {

}


void RRM_ICC_DRA::transimitPreparation() {

}


void RRM_ICC_DRA::transimitStart() {

}


void RRM_ICC_DRA::transimitStartThread(int t_FromRSUId, int t_ToRSUId) {

}


void RRM_ICC_DRA::transimitEnd() {

}


void RRM_ICC_DRA::writeScheduleInfo(ofstream& t_File) {

}


void RRM_ICC_DRA::writeTTILogInfo(std::ofstream& t_File, int TTI, EventLogType t_EventLogType, int t_EventId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx) {

}


void RRM_ICC_DRA::writeClusterPerformInfo(ofstream& t_File) {

}


pair<int, int> RRM_ICC_DRA::getOccupiedSubCarrierRange(MessageType t_MessageType, int t_PatternIdx) {
	return pair<int, int>(-1, -1);
}
