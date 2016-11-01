/*
* =====================================================================================
*
*       Filename:  RRM_RR.cpp
*
*    Description:  DRA模块
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


#include<iomanip>
#include<sstream>
#include<iostream>
#include"RRM_RR.h"

using namespace std;


RRM_RR::RRM_RR(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList, systemTTIRSUThroughput) {
}


void RRM_RR::initialize() {
	//初始化VeUE的该模块参数部分
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeRR();
	}

	//初始化RSU的该模块参数部分
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeRR();
	}
}


void RRM_RR::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();
	}
}


void RRM_RR::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//调度前清理工作
	RRInformationClean();

	//建立接纳链表
	RRUpdateAdmitEventIdList(clusterFlag);

	//开始本次调度
	RRRoundRobin();

    //统计时延信息
	RRDelaystatistics();

	//统计干扰信息
	RRTransimitPreparation();

	//模拟传输开始，更新调度信息，累计吞吐量
	RRTransimitStart();

	//写调度日志
	RRWriteScheduleInfo(g_FileRRScheduleInfo);

	//传输结束
	RRTransimitEnd();
}



void RRM_RR::RRInformationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_RRM_RR->m_RRAdmitEventIdList.clear();
	}
}


void RRM_RR::RRUpdateAdmitEventIdList(bool clusterFlag) {
	//首先，处理System级别的事件触发链表
	RRProcessEventList();

	//其次，如果当前TTI进行了分簇，需要处理调度表
	if (clusterFlag) {
		if (m_RRSwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");

		//处理RSU级别的等待链表
		RRProcessWaitEventIdListWhenLocationUpdate();

		//处理System级别的切换链表
		RRProcessSwitchListWhenLocationUpdate();

		if (m_RRSwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");
	}

	//然后，处理RSU级别的等待链表
	RRProcessWaitEventIdList();
}

void RRM_RR::RRProcessEventList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int eventId : m_EventTTIList[m_TTI]) {
			Event event = m_EventVec[eventId];
			int VeUEId = event.VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//当前事件对应的VeUE不在当前RSU中，跳过即可
				continue;
			}
			else {//当前事件对应的VeUE在当前RSU中
				//将事件压入等待链表
				_RSU.m_RRM_RR->RRPushToWaitEventIdList(eventId, m_EventVec[eventId].message.messageType);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Trigger");

				//记录TTI日志
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1);
			}
		}
	}
}


void RRM_RR::RRProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		//开始处理 m_RRWaitEventIdList
		list<int>::iterator it = _RSU.m_RRM_RR->m_RRWaitEventIdList.begin();
		while (it != _RSU.m_RRM_RR->m_RRWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			MessageType messageType = m_EventVec[eventId].message.messageType;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//该VeUE已经不在该RSU范围内
				//将剩余待传bit重置
				m_EventVec[eventId].message.reset();

				//将其添加到System级别的RSU切换链表中
				_RSU.m_RRM_RR->RRPushToSwitchEventIdList(eventId, m_RRSwitchEventIdList);
				
				//将其从等待链表中删除
				it = _RSU.m_RRM_RR->m_RRWaitEventIdList.erase(it);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

				//记录TTI日志
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1);
			}
			else {//仍然处于当前RSU范围内
				it++;
				continue; //继续留在当前RSU的等待链表
			}
		}
	}
}


void RRM_RR::RRProcessSwitchListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		list<int>::iterator it = m_RRSwitchEventIdList.begin();
		while (it != m_RRSwitchEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//该切换链表中的事件对应的VeUE，不属于当前簇，跳过即可
				it++;
				continue;
			}
			else {//该切换链表中的VeUE，属于当RSU
				//压入等待链表
				_RSU.m_RRM_RR->RRPushToWaitEventIdList(eventId, m_EventVec[eventId].message.messageType);

				//从Switch链表中删除
				it = m_RRSwitchEventIdList.erase(it);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

				//记录TTI日志
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1);
			}
		}
	}
}


void RRM_RR::RRProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		int count = 0;
		list<int>::iterator it = _RSU.m_RRM_RR->m_RRWaitEventIdList.begin();
		while (it != _RSU.m_RRM_RR->m_RRWaitEventIdList.end() && count++ < gc_RRPatternNum) {
			int eventId = *it;
			MessageType messageType = m_EventVec[eventId].message.messageType;
			//压入接入链表
			_RSU.m_RRM_RR->RRPushToAdmitEventIdList(eventId);
			
			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Accept");

			//记录TTI日志
			RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTAT->m_RSUId, -1);

			it = _RSU.m_RRM_RR->m_RRWaitEventIdList.erase(it);
		}
		if (_RSU.m_RRM_RR->m_RRAdmitEventIdList.size() > gc_RRPatternNum)
			throw Exp("RRProcessWaitEventIdList()");
	}
}


void RRM_RR::RRRoundRobin() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int patternIdx = 0; patternIdx < static_cast<int>(_RSU.m_RRM_RR->m_RRAdmitEventIdList.size()); patternIdx++) {
			int eventId = _RSU.m_RRM_RR->m_RRAdmitEventIdList[patternIdx];
			int VeUEId = m_EventVec[eventId].VeUEId;
			MessageType messageType = m_EventVec[eventId].message.messageType;
			_RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx] = new RSU::RRM_RR::RRScheduleInfo(eventId, messageType, VeUEId, _RSU.m_GTAT->m_RSUId, patternIdx);
			m_NewCount++;
		}
	}
}


void RRM_RR::RRDelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		//处理等待链表
		for (int eventId : _RSU.m_RRM_RR->m_RRWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//处理此刻正在将要传输的调度表
		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			if (_RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx] == nullptr)continue;
			m_EventVec[_RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx]->eventId].sendDelay++;
		}
	}
}


void RRM_RR::RRTransimitPreparation() {

}


void RRM_RR::RRTransimitStart() {

}



void RRM_RR::RRWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		
		out << "    RSU[" << _RSU.m_GTAT->m_RSUId << "] :" << endl;
		out << "    {" << endl;
		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
			RSU::RRM_RR::RRScheduleInfo* info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx];
			if (info == nullptr) continue;
			out << info->toScheduleString(3) << endl;
		}
		out << "    }" << endl;
	}
	out << "}" << endl;
	out << "\n\n" << endl;
}


void RRM_RR::RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx) {
	stringstream ss;
	switch (type) {
	case 0:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]    PatternIdx[" << patternIdx << "] }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[0]Succeed";
		out << "    " << ss.str() << endl;
		break;
	case 1:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[1]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 2:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[2]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 3:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << patternIdx << "] ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[3]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 4:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[4]Switch";
		out << "    " << ss.str() << endl;
		break;
	case 5:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[5]Switch";
		out << "    " << ss.str() << endl;
		break;
	}
}


void RRM_RR::RRTransimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			RSU::RRM_RR::RRScheduleInfo* &info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[patternIdx];
			if (info == nullptr) continue;

			//UNDONE
			//这里需要增加SINR计算
			//这里需要增加message.transimit
			//UNDONE
			////累计吞吐率
			//m_TTIRSUThroughput[m_TTI][_RSU.m_GTAT->m_RSUId] += m_EventVec[info->eventId].message.remainBitNum>gc_RRNumRBPerPattern*gc_BitNumPerRB? gc_RRNumRBPerPattern*gc_BitNumPerRB: m_EventVec[info->eventId].message.remainBitNum;

			//更新该事件的日志
			m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Transimit");

			

			if (m_EventVec[info->eventId].message.isFinished()) {//说明已经传输完毕
				//设置传输成功标记
				m_EventVec[info->eventId].isSuccessded = true;

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Succeed");

				//记录TTI日志
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, 0, info->eventId, _RSU.m_GTAT->m_RSUId, patternIdx);

				//释放调度信息对象的内存资源
				delete info;
				info = nullptr;
				m_DeleteCount++;
			}
			else {//没有传输完毕，转到Wait链表，等待下一次调度
				_RSU.m_RRM_RR->RRPushToWaitEventIdList(info->eventId, m_EventVec[info->eventId].message.messageType);

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "WaitNextTurn");

				//记录TTI日志
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, info->eventId, _RSU.m_GTAT->m_RSUId, patternIdx);

				//释放调度信息对象的内存资源
				delete info;
				info = nullptr;
				m_DeleteCount++;
			}
		}
	}
}