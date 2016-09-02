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
#include"RRM_RR.h"

using namespace std;

std::string sRRScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ eventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


std::string sRRScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ ";
	ss << "[ eventId = " << left << setw(3) << eventId << " , VeUEId = " << left << setw(3) << VeUEId << " ]";
	ss << " : occupy Number Of TTI = [";
	ss << occupiedNumTTI;
	ss << "] }";
	return ss.str();
}

RSUAdapterRR::RSUAdapterRR(cRSU& _RSU):m_HoldObj(_RSU){
	m_RRScheduleInfoTable = vector<sRRScheduleInfo*>(gc_RRPatternNum);
}


RRM_RR::RRM_RR(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList) {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAdapterVec.push_back(VeUEAdapterRR(m_VeUEAry[VeUEId]));
	}
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAdapterVec.push_back(RSUAdapterRR(m_RSUAry[RSUId]));
	}
}



void RRM_RR::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//调度前清理工作
	RRInformationClean();

	//建立接纳链表
	RRUpdateAdmitEventIdList(clusterFlag);

	//开始本次调度
	RRTransimitBegin();
	RRWriteScheduleInfo(g_OutRRScheduleInfo);

	RRDelaystatistics();
	RRTransimitEnd();
}



void RRM_RR::RRInformationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		_RSUAdapterRR.m_RRAdmitEventIdList.clear();
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
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		for (int eventId : m_EventTTIList[m_TTI]) {
			sEvent event = m_EventVec[eventId];
			int VeUEId = event.VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterRR.m_HoldObj.m_RSUId) {//当前事件对应的VeUE不在当前RSU中，跳过即可
				continue;
			}
			else {//当前事件对应的VeUE在当前RSU中
				//将事件压入等待链表
				_RSUAdapterRR.RRPushToWaitEventIdList(eventId, m_EventVec[eventId].message.messageType);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
			}
		}
	}
}


void RRM_RR::RRProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		//开始处理 m_RRWaitEventIdList
		list<int>::iterator it = _RSUAdapterRR.m_RRWaitEventIdList.begin();
		while (it != _RSUAdapterRR.m_RRWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			eMessageType messageType = m_EventVec[eventId].message.messageType;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterRR.m_HoldObj.m_RSUId) {//该VeUE已经不在该RSU范围内
				//将剩余待传bit重置
				m_EventVec[eventId].message.resetRemainBitNum();

				//将其添加到System级别的RSU切换链表中
				_RSUAdapterRR.RRPushToSwitchEventIdList(eventId, m_RRSwitchEventIdList);
				
				//将其从等待链表中删除
				it = _RSUAdapterRR.m_RRWaitEventIdList.erase(it);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);	
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
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		list<int>::iterator it = m_RRSwitchEventIdList.begin();
		while (it != m_RRSwitchEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterRR.m_HoldObj.m_RSUId) {//该切换链表中的事件对应的VeUE，不属于当前簇，跳过即可
				it++;
				continue;
			}
			else {//该切换链表中的VeUE，属于当RSU
				//压入等待链表
				_RSUAdapterRR.RRPushToWaitEventIdList(eventId, m_EventVec[eventId].message.messageType);

				//从Switch链表中删除
				it = m_RRSwitchEventIdList.erase(it);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);
			}
		}
	}
}


void RRM_RR::RRProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		int count = 0;
		list<int>::iterator it = _RSUAdapterRR.m_RRWaitEventIdList.begin();
		while (it != _RSUAdapterRR.m_RRWaitEventIdList.end() && count++ < gc_RRPatternNum) {
			int eventId = *it;
			eMessageType messageType = m_EventVec[eventId].message.messageType;
			//压入接入链表
			_RSUAdapterRR.RRPushToAdmitEventIdList(eventId);
			
			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, -1);

			//记录TTI日志
			RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, -1);

			it = _RSUAdapterRR.m_RRWaitEventIdList.erase(it);
		}
		if (_RSUAdapterRR.m_RRAdmitEventIdList.size() > gc_RRPatternNum)
			throw Exp("RRProcessWaitEventIdList()");
	}
}


void RRM_RR::RRTransimitBegin() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		for (int patternIdx = 0; patternIdx < static_cast<int>(_RSUAdapterRR.m_RRAdmitEventIdList.size()); patternIdx++) {
			int eventId = _RSUAdapterRR.m_RRAdmitEventIdList[patternIdx];
			int VeUEId = m_EventVec[eventId].VeUEId;
			int occupiedNumTTI = (int)ceil((double)m_EventVec[eventId].message.remainBitNum / (double)gc_BitNumPerRB / (double)gc_RRNumRBPerPattern);
			eMessageType messageType = m_EventVec[eventId].message.messageType;
			_RSUAdapterRR.m_RRScheduleInfoTable[patternIdx] = new sRRScheduleInfo(eventId, messageType, VeUEId, _RSUAdapterRR.m_HoldObj.m_RSUId, patternIdx, occupiedNumTTI);
			m_NewCount++;
		}
	}
}


void RRM_RR::RRWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];
		
		out << "    RSU[" << _RSUAdapterRR.m_HoldObj.m_RSUId << "] :" << endl;
		out << "    {" << endl;
		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
			sRRScheduleInfo* info = _RSUAdapterRR.m_RRScheduleInfoTable[patternIdx];
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



void RRM_RR::RRDelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		//处理等待链表
		for (int eventId : _RSUAdapterRR.m_RRWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//处理此刻正在将要传输的调度表
		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			if (_RSUAdapterRR.m_RRScheduleInfoTable[patternIdx] == nullptr)continue;
			m_EventVec[_RSUAdapterRR.m_RRScheduleInfoTable[patternIdx]->eventId].sendDelay++;
		}
	}
}


void RRM_RR::RRTransimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterRR &_RSUAdapterRR = m_RSUAdapterVec[RSUId];

		for (int patternIdx = 0; patternIdx < gc_RRPatternNum; patternIdx++) {
			sRRScheduleInfo* &info = _RSUAdapterRR.m_RRScheduleInfoTable[patternIdx];
			if (info == nullptr) continue;
			//更新该事件的日志
			m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, patternIdx);

			//更新剩余待传输bit数量
			m_EventVec[info->eventId].message.remainBitNum -= gc_RRBitNumPerPattern;

			if (m_EventVec[info->eventId].message.remainBitNum <= 0) {//说明已经传输完毕
				//设置传输成功标记
				m_EventVec[info->eventId].isSuccessded = true;

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, patternIdx);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, 0, info->eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, patternIdx);

				//释放调度信息对象的内存资源
				delete info;
				info = nullptr;
				m_DeleteCount++;
			}
			else {//没有传输完毕，转到Wait链表，等待下一次调度
				_RSUAdapterRR.RRPushToWaitEventIdList(info->eventId, m_EventVec[info->eventId].message.messageType);

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSUAdapterRR.m_HoldObj.m_RSUId, -1, patternIdx);

				//记录TTI日志
				RRWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, info->eventId, _RSUAdapterRR.m_HoldObj.m_RSUId, patternIdx);

				//释放调度信息对象的内存资源
				delete info;
				info = nullptr;
				m_DeleteCount++;
			}
		}
	}
}