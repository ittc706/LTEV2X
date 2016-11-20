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

#include<iomanip>
#include"RRM_ICC_DRA.h"
#include"Function.h"

using namespace std;

RRM_ICC_DRA::RRM_ICC_DRA(int &t_TTI, SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {

	m_InterferenceVec = vector<vector<list<int>>>(m_Config.VeUENum, vector<list<int>>(ns_RRM_ICC_DRA::gc_TotalPatternNum));
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

		m_VeUEAry[VeUEId].m_RRM->m_PreSINR.assign(ns_RRM_ICC_DRA::gc_TotalPatternNum, (numeric_limits<double>::min)());
	}
}


void RRM_ICC_DRA::schedule() {
	bool isLocationUpdate = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//写入地理位置信息
	writeClusterPerformInfo(isLocationUpdate, g_FileClasterPerformInfo);

	//资源分配信息清空:包括每个RSU内的接入链表等
	informationClean();

	//更新等待链表
	updateAccessEventIdList(isLocationUpdate);

	//资源选择
	selectRBBasedOnP123();

	//统计时延信息
	sendDelaystatistics();

	//帧听冲突
	conflictListener();

	//请求地理拓扑单元计算干扰响应矩阵
	transimitPreparation();

	//模拟传输开始，更新调度信息
	transimitStart();

	//写入调度信息
	writeScheduleInfo(g_FileScheduleInfo);

	//模拟传输结束，统计吞吐量
	transimitEnd();
}


void RRM_ICC_DRA::informationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			_RSU.m_RRM_ICC_DRA->m_AccessEventIdList[clusterIdx].clear();
		}
	}
}


void RRM_ICC_DRA::updateAccessEventIdList(bool t_ClusterFlag) {
	//首先，处理System级别的事件触发链表
	processEventList();
	//其次，如果当前TTI进行了位置更新，需要处理调度表
	if (t_ClusterFlag) {
		//处理RSU级别的调度链表
		processScheduleInfoTableWhenLocationUpdate();

		//处理RSU级别的等待链表
		processWaitEventIdListWhenLocationUpdate();

		//处理System级别的切换链表
		processSwitchListWhenLocationUpdate();
	}
	//最后，由等待表生成接入表
	processWaitEventIdList();
}


void RRM_ICC_DRA::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event &event = m_EventVec[eventId];
		int VeUEId = event.getVeUEId();
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		
		//将该事件压入等待链表
		_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx,eventId);

		//更新日志
		m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, -1, -1, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Trigger");
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, -1, -1, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Trigger");
	}
}


void RRM_ICC_DRA::processScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx] == nullptr) continue;

				int eventId = _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].getVeUEId();
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
					//压入Switch链表
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(m_SwitchEventIdList,eventId);

					//将剩余待传bit重置
					m_EventVec[eventId].reset();

					//并释放该调度信息的资源
					Delete::safeDelete(_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx]);

					//释放Pattern资源
					_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					//更新日志
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "LocationUpdate");
				}
				else {
					if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//RSU内部发生了簇切换，将其从调度表中取出，压入等待链表
						//压入该RSU的等待链表
						_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId);

						//并释放该调度信息的资源
						Delete::safeDelete(_RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx]);

						//释放Pattern资源
						_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

						//更新日志
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "LocationUpdate");
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "LocationUpdate");
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
				int VeUEId = m_EventVec[eventId].getVeUEId();
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE已经不在该RSU范围内
					//将其添加到System级别的RSU切换链表中
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//将其从等待链表中删除
					it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].erase(it);

					//将剩余待传bit重置
					m_EventVec[eventId].reset();

					//更新日志
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				else if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//仍然处于当前RSU范围内，但位于不同的簇

					//将其添加到所在簇的等待链表
					_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, eventId);

					//将其从等待链表中的当前簇删除
					it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].erase(it);

					//更新日志
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx, -1, "LocationUpdate");
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
		int VeUEId = m_EventVec[eventId].getVeUEId();
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
		
		//转入等待链表
		_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx,eventId);

		//从Switch表中将其删除
		it = m_SwitchEventIdList.erase(it);

		//更新日志
		m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, -1, -1, -1, RSUId, clusterIdx, -1, "LocationUpdate");
		writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, -1, -1, -1, RSUId, clusterIdx, -1, "LocationUpdate");
	}
}


void RRM_ICC_DRA::processWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			list<int>::iterator it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].end()) {
				int eventId = *it;
				//如果该事件不需要退避，则转入接入表
				if (m_EventVec[eventId].tryAcccess()) {
					_RSU.m_RRM_ICC_DRA->m_AccessEventIdList[clusterIdx].push_back(eventId);
					//更新日志
					m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ACCESS, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "CanAccess");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ACCESS, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "CanAccess");

					//将该事件从等代表中删除
					it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].erase(it);
				}
				else {
					//更新日志
					m_EventVec[eventId].addEventLog(m_TTI, WITHDRAWING, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "Withdraw");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, WITHDRAWING, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, -1, -1, -1, "Withdraw");

					it++;
				}
			}	
		}
	}
}


void RRM_ICC_DRA::selectRBBasedOnP123() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {

			/*
			* 当前TTI当前簇可用的Pattern块编号
			* 每个内层vector代表该种类Pattern可用的Pattern编号(绝对Pattern编号)
			*/
			vector<int> curAvaliablePatternIdx;

			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				if (_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliablePatternIdx.push_back(patternIdx);
				}
			}

			for (int eventId:_RSU.m_RRM_ICC_DRA->m_AccessEventIdList[clusterIdx]) {//遍历该簇内接纳链表中的事件

				int VeUEId = m_EventVec[eventId].getVeUEId();

				//为当前用户在可用的对应其事件类型的Pattern块中随机选择一个，每个用户自行随机选择可用Pattern块
				int patternIdx = m_VeUEAry[VeUEId].m_RRM_ICC_DRA->selectRBBasedOnP2(curAvaliablePatternIdx);

				if (patternIdx == -1) {//该用户传输的信息类型没有pattern剩余了
					//更新日志
					m_EventVec[eventId].addEventLog(m_TTI, ACCESS_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "AllBusy");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, ACCESS_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "AllBusy");
								
					continue;
				}

				//将资源标记为占用
				_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

				//将调度信息压入m_TransimitEventIdList中
				_RSU.m_RRM_ICC_DRA->pushToTransmitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx));

			}
		}
		//将调度表中当前可以继续传输的用户压入传输链表中
		_RSU.m_RRM_ICC_DRA->pullFromScheduleInfoTable(m_TTI);
	}
}


void RRM_ICC_DRA::sendDelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];


		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//处理等待链表
			for (int eventId : _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx])
				m_EventVec[eventId].increaseQueueDelay();

			//处理此刻正在将要传输的传输链表
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
					m_EventVec[p->eventId].increaseSendDelay();
			}
		}
	}
}


void RRM_ICC_DRA::conflictListener() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx =0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {

				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
					for (RSU::RRM::ScheduleInfo* &info : lst) {
						//首先将事件压入等待列表
						_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx, info->eventId);

						//冲突后更新事件的状态
						m_EventVec[info->eventId].conflict();

						//更新日志
						m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Conflict");
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Conflict");

						//释放调度信息对象的内存资源
						Delete::safeDelete(info);
					}
					//释放Pattern资源
					_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					lst.clear();
				}
			}
		}
	}
}


void RRM_ICC_DRA::transimitPreparation() {
	//首先清空上一次干扰信息
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++)
		for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++)
			m_InterferenceVec[VeUEId][patternIdx].clear();

	//统计本次的干扰信息
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				list<RSU::RRM::ScheduleInfo*> &curList = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (curList.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
					RSU::RRM::ScheduleInfo *curInfo = *curList.begin();
					int curVeUEId = curInfo->VeUEId;
					for (int otherClusterIdx = 0; otherClusterIdx < _RSU.m_GTT->m_ClusterNum; otherClusterIdx++) {
						if (otherClusterIdx == clusterIdx)continue;
						list<RSU::RRM::ScheduleInfo*> &otherList = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[otherClusterIdx][patternIdx];
						if (otherList.size() == 1) {//其他簇中该pattern下有车辆在传输，那么将该车辆作为干扰车辆
							RSU::RRM::ScheduleInfo *otherInfo = *otherList.begin();
							int otherVeUEId = otherInfo->VeUEId;
							m_InterferenceVec[curVeUEId][patternIdx].push_back(otherVeUEId);
						}
					}	
				}
			}
		}
	}

	//更新每辆车的干扰车辆列表	
	for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
		for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
			list<int>& interList = m_InterferenceVec[VeUEId][patternIdx];

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUENum[patternIdx] = (int)interList.size();//写入干扰数目

			m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx].assign(interList.begin(), interList.end());//写入干扰车辆ID

			if (m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUENum[patternIdx]>0) {
				g_FileTemp << "VeUEId: " << VeUEId << " [";
				for (auto c : m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx])
					g_FileTemp << c << ", ";
				g_FileTemp << " ]" << endl;
			}	
		}
	}

	//请求地理拓扑单元计算干扰响应矩阵
	long double start = clock();
	m_GTTPoint->calculateInterference(m_InterferenceVec);
	long double end = clock();
	m_GTTTimeConsume += end - start;
}


void RRM_ICC_DRA::transimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_ICC_DRA::transimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}


void RRM_ICC_DRA::transimitStartThread(int t_FromRSUId, int t_ToRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//由于每个线程的该模块会有不同的状态且无法共享，因此这里拷贝该模块用于本次计算
	for (int RSUId = t_FromRSUId; RSUId <= t_ToRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {

				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
					RSU::RRM::ScheduleInfo *info = *lst.begin();
					int VeUEId = info->VeUEId;

					//计算SINR，获取调制编码方式
					pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(patternIdx);
					g_FileTemp << "NonEmergencyPatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

					double factor = m_VeUEAry[VeUEId].m_RRM->m_ModulationType * m_VeUEAry[VeUEId].m_RRM->m_CodeRate;

					//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
					int maxEquivalentBitNum = (int)((double)(ns_RRM_ICC_DRA::gc_RBNumPerPattern * gc_BitNumPerRB)* factor);

					//计算SINR
					double curSINR = 0;
					if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->isAlreadyCalculateSINR(patternIdx)) {//调制编码方式需要更新时
						curSINR = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
						m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
						m_VeUEAry[VeUEId].m_RRM->m_PreSINR[patternIdx] = curSINR;
					}
					else
						curSINR = m_VeUEAry[VeUEId].m_RRM->m_PreSINR[patternIdx];
		
					//记录调度信息
					if (curSINR < gc_CriticalPoint) m_EventVec[info->eventId].packetLoss();//记录丢包
					info->transimitBitNum = maxEquivalentBitNum;
					info->currentPackageIdx = m_EventVec[info->eventId].getCurrentPackageIdx();
					info->remainBitNum = m_EventVec[info->eventId].getRemainBitNum();

					//该编码方式下，该Pattern在一个TTI传输的实际的有效信息bit数量，并更新信息状态
					int realEquivalentBitNum = m_EventVec[info->eventId].transimit(maxEquivalentBitNum);
					
					//累计吞吐率
					m_TTIRSUThroughput[m_TTI][_RSU.m_GTT->m_RSUId] += realEquivalentBitNum;

					//更新日志
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMITTING, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMITTING, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
				}
			}
		}
		
	}
	Delete::safeDelete(copyWTPoint);//getCopy是通过new创建的，因此这里释放资源
}


void RRM_ICC_DRA::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {

				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
					RSU::RRM::ScheduleInfo* &info = *lst.begin();
					if (m_EventVec[info->eventId].isFinished()) {//说明该数据已经传输完毕

						//更新日志
						m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

						//释放调度信息对象的内存资源
						Delete::safeDelete(info);

						_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					}
					else {//该数据仍未传完，将其压回m_ScheduleInfoTable
						_RSU.m_RRM_ICC_DRA->pushToScheduleInfoTable(info);
						info = nullptr;
					}
				}
				//处理完后，将该pattern上的数据清空（此时要不本身就是空，要不就是nullptr指针）
				lst.clear();
			}
		}
	}
}


void RRM_ICC_DRA::writeScheduleInfo(ofstream& t_File) {
	t_File << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	t_File << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {

		RSU &_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			t_File << "    RSU[" << _RSU.m_GTT->m_RSUId << "] :" << endl;
			t_File << "    {" << endl;
			t_File << "        Cluster[" << clusterIdx << "] :" << endl;
			t_File << "        {" << endl;
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				t_File << "            Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
				bool isAvaliable = _RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx];
				if (!isAvaliable) {
					RSU::RRM::ScheduleInfo* &info = *(_RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					if (info == nullptr) throw Exp("logic error");
					t_File << info->toScheduleString(3) << endl;
				}
			}
			t_File << "        }" << endl;
			t_File << "    }" << endl;
		}

	}
	t_File << "}" << endl;
	t_File << "\n\n" << endl;
}


void RRM_ICC_DRA::writeTTILogInfo(ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description) {
	stringstream ss;
	switch (t_EventLogType) {
	case TRANSIMITTING:
		ss << " - Transimiting  At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WITHDRAWING:
		ss << " - Withdraw  At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SUCCEED:
		ss << " - Transimit Succeed At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case EVENT_TO_WAIT:
		ss << " - From: EventList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: SwitchList";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WAIT_TO_SWITCH:
		ss << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: SwitchList";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WAIT_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WAIT_TO_ACCESS:
		ss << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s AccessEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case ACCESS_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s AccessEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SWITCH_TO_WAIT:
		ss << " - From: SwitchList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case TRANSIMIT_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s TransimitScheduleInfoList[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		t_File << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	}
}


void RRM_ICC_DRA::writeClusterPerformInfo(bool isLocationUpdate, ofstream& t_File) {
	if (!isLocationUpdate) return;
	t_File << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	t_File << "{" << endl;

	//打印VeUE信息
	t_File << "    VUE Info: " << endl;
	t_File << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUE &_VeUE = m_VeUEAry[VeUEId];
		t_File << _VeUE.m_RRM_ICC_DRA->toString(2) << endl;
	}
	t_File << "    }\n" << endl;

	////打印基站信息
	//out << "    eNB Info: " << endl;
	//out << "    {" << endl;
	//for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
	//	ceNB &_eNB = m_eNBAry[eNBId];
	//	out << _eNB.toString(2) << endl;
	//}
	//out << "    }\n" << endl;

	//打印RSU信息
	t_File << "    RSU Info: " << endl;
	t_File << "    {" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		t_File << _RSU.m_RRM_ICC_DRA->toString(2) << endl;
	}
	t_File << "    }" << endl;

	t_File << "}\n\n";
}


pair<int, int> RRM_ICC_DRA::getOccupiedSubCarrierRange(int t_PatternIdx) {
	pair<int, int> res;

	res.first = ns_RRM_ICC_DRA::gc_RBNumPerPattern * t_PatternIdx;
	res.second = ns_RRM_ICC_DRA::gc_RBNumPerPattern * (t_PatternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
