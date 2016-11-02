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


RRM_RR::RRM_RR(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput, GTAT_Basic* systemGTATPoint, WT_Basic* systemWTPoint, int threadNum) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList, systemTTIRSUThroughput), m_GTATPoint(systemGTATPoint), m_WTPoint(systemWTPoint), m_ThreadNum(threadNum) {
	
	m_RRInterferenceVec = std::vector<std::list<int>>(gc_RRTotalPatternNum);

	m_ThreadsRSUIdRange = vector<pair<int, int>>(threadNum);

	int num = m_Config.RSUNum / threadNum;
	for (int threadIdx = 0; threadIdx < threadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[threadNum - 1].second = m_Config.RSUNum - 1;//修正最后一个边界
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
		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(gc_DRATotalPatternNum, false);
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
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++)
			_RSU.m_RRM_RR->m_RRAdmitEventIdList[clusterIdx].clear();
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
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTAT->m_RSUId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx;
		RSU &_RSU = m_RSUAry[RSUId];
		//将事件压入等待链表
		_RSU.m_RRM_RR->RRPushToWaitEventIdList(clusterIdx,eventId, m_EventVec[eventId].message.messageType);

		//更新该事件的日志
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, RSUId, -1, -1, "Trigger");

		//记录TTI日志
		RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, RSUId, -1);
	}
}


void RRM_RR::RRProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {

			list<int>::iterator it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].end()) {
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//该VeUE已经不在该RSU范围内
					//将剩余待传bit重置
					m_EventVec[eventId].message.reset();

					//将其添加到System级别的RSU切换链表中
					_RSU.m_RRM_RR->RRPushToSwitchEventIdList(eventId, m_RRSwitchEventIdList);

					//将其从等待链表中删除
					it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].erase(it);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

					//记录TTI日志
					RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1);
				}else if(m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx!= clusterIdx){//仍然处于当前RSU范围内，但是位于不同的簇
					//将其转移到当前RSU的其他簇内
					_RSU.m_RRM_RR->RRPushToWaitEventIdList(m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx, eventId, messageType);

					//将其从等待链表中删除
					it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].erase(it);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

					//记录TTI日志
					RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1);
				}else {
					it++;
					continue; //继续留在当前RSU当前簇的等待链表
				}
			}
		}
	}
}


void RRM_RR::RRProcessSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_RRSwitchEventIdList.begin();
	while (it != m_RRSwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int clusterIdx = m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx;
		int RSUId = m_VeUEAry[VeUEId].m_GTAT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];

		_RSU.m_RRM_RR->RRPushToWaitEventIdList(clusterIdx, eventId, m_EventVec[eventId].message.messageType);

		//从Switch链表中删除
		it = m_RRSwitchEventIdList.erase(it);

		//更新该事件的日志
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, RSUId, -1, -1, "LocationUpdate");

		//记录TTI日志
		RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, RSUId, -1);

	}
}


void RRM_RR::RRProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			int count = 0;
			list<int>::iterator it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].end() && count++ < gc_RRTotalPatternNum) {
				int eventId = *it;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				//压入接入链表
				_RSU.m_RRM_RR->RRPushToAdmitEventIdList(clusterIdx, eventId);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, RSUId, -1, -1, "Accept");

				//记录TTI日志
				RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, RSUId, -1);

				it = _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx].erase(it);
			}
			if (_RSU.m_RRM_RR->m_RRAdmitEventIdList[clusterIdx].size() > gc_RRTotalPatternNum)
				throw Exp("RRProcessWaitEventIdList()");
		}
	}
}


void RRM_RR::RRRoundRobin() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < _RSU.m_RRM_RR->m_RRAdmitEventIdList[clusterIdx].size(); patternIdx++) {
				int eventId = _RSU.m_RRM_RR->m_RRAdmitEventIdList[clusterIdx][patternIdx];
				int VeUEId = m_EventVec[eventId].VeUEId;
				MessageType messageType = m_EventVec[eventId].message.messageType;
				_RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx] = new RSU::RRM_RR::RRScheduleInfo(eventId, messageType, VeUEId, _RSU.m_GTAT->m_RSUId, patternIdx);
				m_NewCount++;
			}
		}
	}
}


void RRM_RR::RRDelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			//处理等待链表
			for (int eventId : _RSU.m_RRM_RR->m_RRWaitEventIdList[clusterIdx])
				m_EventVec[eventId].queuingDelay++;

			//处理此刻正在将要传输的调度表
			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx] == nullptr)continue;
				m_EventVec[_RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx]->eventId].sendDelay++;
			}
		}
	}
}


void RRM_RR::RRTransimitPreparation() {
	for (list<int>&lst : m_RRInterferenceVec) {
		lst.clear();
	}

	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
				RSU::RRM_RR::RRScheduleInfo *&info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				m_RRInterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
	}


	//更新每辆车的干扰车辆列表	
	for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
		list<int>& lst = m_RRInterferenceVec[patternIdx];

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
	m_GTATPoint->calculateInterference(m_RRInterferenceVec);
	long double end = clock();
	m_GTATTimeConsume += end - start;
}


void RRM_RR::RRTransimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_RR::RRTransimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}


void RRM_RR::RRTransimitStartThread(int fromRSUId, int toRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//由于每个线程的该模块会有不同的状态且无法共享，因此这里拷贝该模块用于本次计算
	for (int RSUId = fromRSUId; RSUId <= toRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
				RSU::RRM_RR::RRScheduleInfo *&info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx];

				if (info == nullptr) continue;
				int VeUEId = info->VeUEId;

				//计算SINR，获取调制编码方式
				pair<int, int> &subCarrierIdxRange = RRGetOccupiedSubCarrierRange(patternIdx);
				g_FileTemp << "PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
				int maxEquivalentBitNum = (int)((double)(gc_RRNumRBPerPattern * gc_BitNumPerRB)* factor);

				//记录调度信息
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
				info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

				//该编码方式下，该Pattern在一个TTI传输的实际的有效信息bit数量，并更新信息状态
				int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);

				//累计吞吐率
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTAT->m_RSUId] += realEquivalentBitNum;

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Transimit");
			}
		}
	}
}


void RRM_RR::RRWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {

		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			out << "    RSU[" << _RSU.m_GTAT->m_RSUId << "] :" << endl;
			out << "    {" << endl;
			out << "        Cluster[" << clusterIdx << "] :" << endl;
			out << "        {" << endl;
			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {
				out << "            Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
				RSU::RRM_RR::RRScheduleInfo* &info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx];
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


void RRM_RR::RRWriteTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int patternIdx) {
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


void RRM_RR::RRTransimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {

			for (int patternIdx = 0; patternIdx < gc_RRTotalPatternNum; patternIdx++) {

				RSU::RRM_RR::RRScheduleInfo* &info = _RSU.m_RRM_RR->m_RRScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;

				if (m_EventVec[info->eventId].message.isFinished()) {//说明已经传输完毕
					//设置传输成功标记
					m_EventVec[info->eventId].isSuccessded = true;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Succeed");

					//记录TTI日志
					RRWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTAT->m_RSUId, patternIdx);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
					m_DeleteCount++;
				}
				else {//没有传输完毕，转到Wait链表，等待下一次调度
					_RSU.m_RRM_RR->RRPushToWaitEventIdList(clusterIdx, info->eventId, m_EventVec[info->eventId].message.messageType);

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
}


std::pair<int, int> RRM_RR::RRGetOccupiedSubCarrierRange(int patternIdx) {
	pair<int, int> res;

	res.first = gc_RRNumRBPerPattern * patternIdx;
	res.second = gc_RRNumRBPerPattern * (patternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
