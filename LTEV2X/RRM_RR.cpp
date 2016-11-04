/*
* =====================================================================================
*
*       Filename:  RRM_RR.cpp
*
*    Description:  RR模块
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
#include<set>
#include"RRM_RR.h"

using namespace std;


RRM_RR::RRM_RR(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {
	
	m_InterferenceVec = vector<list<int>>(ns_RRM_RR::gc_RRTotalPatternNum);

	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = m_Config.RSUNum - 1;//修正最后一个边界
}


void RRM_RR::initialize() {
	//初始化VeUE的该模块参数部分
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeRRM_RR();
	}

	//初始化RSU的该模块参数部分
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeRRM_RR();
	}
}


void RRM_RR::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();
		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(ns_RRM_TDM_DRA::gc_TotalPatternNum, false);
	}
}


void RRM_RR::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//调度前清理工作
	informationClean();

	//建立接纳链表
	updateAdmitEventIdList(clusterFlag);

	//开始本次调度
	roundRobin();

    //统计时延信息
	delaystatistics();

	//统计干扰信息
	transimitPreparation();

	//模拟传输开始，更新调度信息，累计吞吐量
	transimitStart();

	//写调度日志
	writeScheduleInfo(g_FileRRScheduleInfo);

	//传输结束
	transimitEnd();
}



void RRM_RR::informationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++)
			_RSU.m_RRM_RR->m_AdmitEventIdList[clusterIdx].clear();
	}
}


void RRM_RR::updateAdmitEventIdList(bool clusterFlag) {
	//首先，处理System级别的事件触发链表
	processEventList();

	//其次，如果当前TTI进行了分簇，需要处理调度表
	if (clusterFlag) {
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");

		//处理RSU级别的等待链表
		processWaitEventIdListWhenLocationUpdate();

		//处理System级别的切换链表
		processSwitchListWhenLocationUpdate();

		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");
	}

	//然后，处理RSU级别的等待链表
	processWaitEventIdList();
}

void RRM_RR::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		RSU &_RSU = m_RSUAry[RSUId];
		//将事件压入等待链表
		_RSU.m_RRM_RR->pushToWaitEventIdList(clusterIdx,eventId, m_EventVec[eventId].message.messageType);

		//更新该事件的日志
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, RSUId, -1, -1, "Trigger");

		//记录TTI日志
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, RSUId, -1);
	}
}


void RRM_RR::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {

			list<int>::iterator it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE已经不在该RSU范围内
					//将剩余待传bit重置
					m_EventVec[eventId].message.reset();

					//将其添加到System级别的RSU切换链表中
					_RSU.m_RRM_RR->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//将其从等待链表中删除
					it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, -1);
				}else if(m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx!= clusterIdx){//仍然处于当前RSU范围内，但是位于不同的簇
					//将其转移到当前RSU的其他簇内
					_RSU.m_RRM_RR->pushToWaitEventIdList(m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId, messageType);

					//将其从等待链表中删除
					it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1);
				}else {
					it++;
					continue; //继续留在当前RSU当前簇的等待链表
				}
			}
		}
	}
}


void RRM_RR::processSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_SwitchEventIdList.begin();
	while (it != m_SwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];

		_RSU.m_RRM_RR->pushToWaitEventIdList(clusterIdx, eventId, m_EventVec[eventId].message.messageType);

		//从Switch链表中删除
		it = m_SwitchEventIdList.erase(it);

		//更新该事件的日志
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, RSUId, -1, -1, "LocationUpdate");

		//记录TTI日志
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, RSUId, -1);

	}
}


void RRM_RR::processWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			int count = 0;
			list<int>::iterator it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].end() && count++ < ns_RRM_RR::gc_RRTotalPatternNum) {
				int eventId = *it;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				//压入接入链表
				_RSU.m_RRM_RR->pushToAdmitEventIdList(clusterIdx, eventId);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, RSUId, -1, -1, "Accept");

				//记录TTI日志
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, RSUId, -1);

				it = _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx].erase(it);
			}
			if (_RSU.m_RRM_RR->m_AdmitEventIdList[clusterIdx].size() > ns_RRM_RR::gc_RRTotalPatternNum)
				throw Exp("RRProcessWaitEventIdList()");
		}
	}
}


void RRM_RR::roundRobin() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < _RSU.m_RRM_RR->m_AdmitEventIdList[clusterIdx].size(); patternIdx++) {
				int eventId = _RSU.m_RRM_RR->m_AdmitEventIdList[clusterIdx][patternIdx];
				int VeUEId = m_EventVec[eventId].VeUEId;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				_RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx] = new RSU::RRM_RR::ScheduleInfo(eventId, messageType, VeUEId, _RSU.m_GTT->m_RSUId, patternIdx);
			}
		}
	}
}


void RRM_RR::delaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//处理等待链表
			for (int eventId : _RSU.m_RRM_RR->m_WaitEventIdList[clusterIdx])
				m_EventVec[eventId].queuingDelay++;

			//处理此刻正在将要传输的调度表
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx] == nullptr)continue;
				m_EventVec[_RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx]->eventId].sendDelay++;
			}
		}
	}
}


void RRM_RR::transimitPreparation() {
	for (list<int>&lst : m_InterferenceVec) {
		lst.clear();
	}

	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
				RSU::RRM_RR::ScheduleInfo *&info = _RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				m_InterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
	}


	//更新每辆车的干扰车辆列表	
	for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
		list<int>& lst = m_InterferenceVec[patternIdx];

		for (int VeUEId : lst) {

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUENum[patternIdx] = (int)lst.size() - 1;//写入干扰数目

			set<int> s(lst.begin(), lst.end());//用于更新干扰列表的
			s.erase(VeUEId);

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx].assign(s.begin(), s.end());//写入干扰车辆ID

			g_FileTemp << "VeUEId: " << VeUEId << " [";
			for (auto c : m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx])
				g_FileTemp << c << ", ";
			g_FileTemp << " ]" << endl;
		}
	}

	//请求地理拓扑单元计算干扰响应矩阵
	long double start = clock();
	m_GTTPoint->calculateInterference(m_InterferenceVec);
	long double end = clock();
	m_GTTTimeConsume += end - start;
}


void RRM_RR::transimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_RR::transimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}


void RRM_RR::transimitStartThread(int fromRSUId, int toRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//由于每个线程的该模块会有不同的状态且无法共享，因此这里拷贝该模块用于本次计算
	for (int RSUId = fromRSUId; RSUId <= toRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
				RSU::RRM_RR::ScheduleInfo *&info = _RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx];

				if (info == nullptr) continue;
				int VeUEId = info->VeUEId;

				//计算SINR，获取调制编码方式
				pair<int, int> &subCarrierIdxRange = getOccupiedSubCarrierRange(patternIdx);
				g_FileTemp << "PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
				int maxEquivalentBitNum = (int)((double)(ns_RRM_RR::gc_RRNumRBPerPattern * gc_BitNumPerRB)* factor);

				//记录调度信息
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
				info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

				//该编码方式下，该Pattern在一个TTI传输的实际的有效信息bit数量，并更新信息状态
				int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);

				//累计吞吐率
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTT->m_RSUId] += realEquivalentBitNum;

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTT->m_RSUId, -1, patternIdx, "Transimit");
			}
		}
	}
}


void RRM_RR::writeScheduleInfo(ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {

		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			out << "    RSU[" << _RSU.m_GTT->m_RSUId << "] :" << endl;
			out << "    {" << endl;
			out << "        Cluster[" << clusterIdx << "] :" << endl;
			out << "        {" << endl;
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {
				out << "            Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
				RSU::RRM_RR::ScheduleInfo* &info = _RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				out << info->toScheduleString(3) << endl;
			}
			out << "        }" << endl;
			out << "    }" << endl;
		}
		
	}
	out << "}" << endl;
	out << "\n\n" << endl;
}


void RRM_RR::writeTTILogInfo(ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int patternIdx) {
	stringstream ss;
	switch (type) {
	case SUCCEED:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]    PatternIdx[" << patternIdx << "] }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[0]Succeed";
		out << "    " << ss.str() << endl;
		break;
	case EVENT_TO_WAIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[1]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_ADMIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[2]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << patternIdx << "] ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[3]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_SWITCH:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[4]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[5]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SWITCH_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[5]Switch";
		out << "    " << ss.str() << endl;
		break;
	}
}


void RRM_RR::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {

			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_RRTotalPatternNum; patternIdx++) {

				RSU::RRM_RR::ScheduleInfo* &info = _RSU.m_RRM_RR->m_ScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;

				if (m_EventVec[info->eventId].message.isFinished()) {//说明已经传输完毕
					//设置传输成功标记
					m_EventVec[info->eventId].isSuccessded = true;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, -1, patternIdx, "Succeed");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, patternIdx);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
				}
				else {//没有传输完毕，转到Wait链表，等待下一次调度
					_RSU.m_RRM_RR->pushToWaitEventIdList(clusterIdx, info->eventId, m_EventVec[info->eventId].message.messageType);

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, patternIdx, "WaitNextTurn");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, patternIdx);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
				}
			}
		}
	}
}


pair<int, int> RRM_RR::getOccupiedSubCarrierRange(int patternIdx) {
	pair<int, int> res;

	res.first = ns_RRM_RR::gc_RRNumRBPerPattern * patternIdx;
	res.second = ns_RRM_RR::gc_RRNumRBPerPattern * (patternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
