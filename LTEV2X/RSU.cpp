#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Traffic.h"
#include"Global.h"


using namespace std;

int newCount = 0;//记录动态创建的对象的次数

int deleteCount = 0;//记录删除动态创建对象的次数

void cRSU::initialize(sRSUConfigure &t_RSUConfigure){
	m_RSUId = t_RSUConfigure.wRSUID;
	m_fAbsX = c_RSUTopoRatio[m_RSUId * 2 + 0] * c_wide;
	m_fAbsY = c_RSUTopoRatio[m_RSUId * 2 + 1] * c_length;
	RandomUniform(&m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU：");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_RSUId, m_fAbsX, m_fAbsY);

	m_DRAClusterNum = c_RSUClusterNum[m_RSUId];
	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);


	//RR参量初始化
	m_RRScheduleInfoTable = vector<sRRScheduleInfo*>(m_RRPatternNum);
}


//<DONE>
void cRSU::RRInformationClean() {
	m_RRAdmitEventIdList.clear();
}

//<UNDONE>
void cRSU::RRProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList) {
	for (int eventId : systemEventTTIList[TTI]) {
		sEvent event = systemEventVec[eventId];
		int VeUEId = event.VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//当前事件对应的VeUE不在当前RSU中，跳过即可
			continue;
		}
		else {//当前事件对应的VeUE在当前RSU中
			if (systemEventVec[eventId].message.messageType == EMERGENCY) {//若是紧急事件
				/*  EMERGENCY  */
				RRPushToWaitEventIdList(eventId, EMERGENCY);

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 2, m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 1, eventId, m_RSUId , -1);
				/*  EMERGENCY  */
			}
			else {//一般性事件，包括周期事件，或者数据业务事件
				//将该事件压入等待链表
				RRPushToWaitEventIdList(eventId,PERIOD);

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 2, m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 1, eventId, m_RSUId, -1);
			}
		}
	}
}



//<UNDONE>
void cRSU::RRProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList) {
	//开始处理 m_RRWaitEventIdList
	list<int>::iterator it = m_RRWaitEventIdList.begin();
	while (it != m_RRWaitEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		eMessageType messageType = systemEventVec[eventId].message.messageType;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该VeUE已经不在该RSU范围内
			RRPushToSwitchEventIdList(eventId, systemRRSwitchEventIdList);//将其添加到System级别的RSU切换链表中
			it = m_RRWaitEventIdList.erase(it);//将其从等待链表中删除

			if (messageType == EMERGENCY) {
				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 5, m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 4, eventId, m_RSUId, -1);
			}
			else if (messageType == PERIOD) {
				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 5, m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 4, eventId, m_RSUId, -1);
			}
		}
		else {//仍然处于当前RSU范围内
			it++;
			continue; //继续留在当前RSU的等待链表
		}
	}
}

//<UNDONE>
void cRSU::RRProcessSwitchListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList) {
	list<int>::iterator it = systemRRSwitchEventIdList.begin();
	while (it != systemRRSwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该切换链表中的事件对应的VeUE，不属于当前簇，跳过即可
			it++;
			continue;
		}
		else {//该切换链表中的VeUE，属于当RSU
			  /*  EMERGENCY  */
			if (systemEventVec[eventId].message.messageType == EMERGENCY) {//属于紧急事件
				RRPushToWaitEventIdList(eventId, EMERGENCY);
				it = systemRRSwitchEventIdList.erase(it);

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 8, m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 5, eventId, m_RSUId, -1);
			}
			/*  EMERGENCY  */

			else {//非紧急事件
				RRPushToWaitEventIdList(eventId,PERIOD);
				it = systemRRSwitchEventIdList.erase(it);

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 8, m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 5, eventId, m_RSUId, -1);		
			}
		}
	}
}

//<UNDONE>
void cRSU::RRProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList) {
	int count = 0;
	list<int>::iterator it = m_RRWaitEventIdList.begin();
	while (it != m_RRWaitEventIdList.end() && count < m_RRPatternNum) {
		int eventId = *it;
		eMessageType messageType = systemEventVec[eventId].message.messageType;
		RRPushToAdmitEventIdList(eventId);
		if (messageType == EMERGENCY) {
			//更新该事件的日志
			systemEventVec[eventId].addEventLog(TTI, 6, m_RSUId, -1, -1);

			//记录TTI日志
			RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 2, eventId, m_RSUId, -1);
		}
		else if (messageType == PERIOD) {
			//更新该事件的日志
			systemEventVec[eventId].addEventLog(TTI, 6, m_RSUId, -1, -1);

			//记录TTI日志
			RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 2, eventId, m_RSUId, -1);
		}
		it = m_RRWaitEventIdList.erase(it);
	}
}

//<UNDONE>
void cRSU::RRProcessTransimit1(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec){
	for (int patternIdx = 0; patternIdx < static_cast<int>(m_RRAdmitEventIdList.size()); patternIdx++) {
		int eventId = m_RRAdmitEventIdList[patternIdx];
		int VeUEId = systemEventVec[eventId].VeUEId;
		int occupiedTTI = ceil((double)systemEventVec[eventId].message.bitNum / (double)gc_BitNumPerRB / (double)m_RRNumRBPerPattern);
		eMessageType messageType = systemEventVec[eventId].message.messageType;
		tuple<int, int> scheduleInterval(TTI, TTI + occupiedTTI - 1);
		m_RRScheduleInfoTable[patternIdx] = new sRRScheduleInfo(eventId, messageType, VeUEId, m_RSUId, patternIdx,scheduleInterval);
	}
}

//<UNDONE>
void cRSU::RRDelaystatistics(int TTI, std::vector<sEvent>& systemEventVec) {
	//处理等待链表
	for (int eventId : m_RRWaitEventIdList)
		systemEventVec[eventId].queuingDelay++;

	//处理此刻正在将要传输的调度表
	for (int patternIdx = 0; patternIdx < m_RRPatternNum; patternIdx++) {
		if (m_RRScheduleInfoTable[patternIdx] == nullptr)continue;
		systemEventVec[m_RRScheduleInfoTable[patternIdx]->eventId].sendDelay++;
	}
}

//<UNDONE>
void cRSU::RRProcessTransimit2(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec) {
	for (int patternIdx = 0; patternIdx < m_RRPatternNum; patternIdx++) {
		sRRScheduleInfo* &info = m_RRScheduleInfoTable[patternIdx];
		if (info == nullptr) continue;
		//更新该事件的日志
		systemEventVec[info->eventId].addEventLog(TTI, 10, m_RSUId, -1, patternIdx);

		//WRONG
		if (TTI == get<1>(info->occupiedInterval)) {
			//更新该事件的日志
			systemEventVec[info->eventId].addEventLog(TTI, 0, m_RSUId, -1, patternIdx);

			//记录TTI日志
			RRWriteTTILogInfo(g_OutTTILogInfo, TTI, 0, info->eventId, m_RSUId, patternIdx);

			//释放调度信息对象的内存资源
			delete info;
			info = nullptr;
			deleteCount++;
		}
	}
}










