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
	writeScheduleInfo(g_FileScheduleInfo);

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
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(m_SwitchEventIdList,eventId);

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
					_RSU.m_RRM_ICC_DRA->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

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

			list<int>::iterator it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].begin();
			while (it!= _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].end()) {//遍历该簇内接纳链表中的事件
				int eventId = *it;
				int VeUEId = m_EventVec[eventId].VeUEId;

				//为当前用户在可用的对应其事件类型的Pattern块中随机选择一个，每个用户自行随机选择可用Pattern块
				int patternIdx = m_VeUEAry[VeUEId].m_RRM_ICC_DRA->selectRBBasedOnP2(curAvaliablePatternIdx);

				if (patternIdx == -1) {//该用户传输的信息类型没有pattern剩余了
					_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx, eventId);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "AllBusy");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1);
					
					it++;
					continue;
				}

				//将资源标记为占用
				_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

				//将调度信息压入m_TransimitEventIdList中
				_RSU.m_RRM_ICC_DRA->pushToTransmitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx));

				it = _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx].erase(it);
			}

			//<UNDOWN>目前暂时不需要调度表，因为默认不会持续占用资源
			////将调度表中当前可以继续传输的用户压入传输链表中
			//_RSU.m_RRM_ICC_DRA->pullFromScheduleInfoTable(m_TTI);
		}
	}
}


void RRM_ICC_DRA::delaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];


		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//处理等待链表
			for (int eventId : _RSU.m_RRM_ICC_DRA->m_WaitEventIdList[clusterIdx])
				m_EventVec[eventId].queuingDelay++;

			//处理此刻正在将要传输的传输链表
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
					m_EventVec[p->eventId].sendDelay++;
			}

			//<UNDOWN>目前暂时不需要调度表，因为默认不会持续占用资源
			////处理此刻位于调度表中，但不属于当前簇的事件
			//int curClusterIdx = _RSU.m_RRM_ICC_DRA->getClusterIdx(m_TTI);
			//for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//	if (clusterIdx == curClusterIdx) continue;
			//	for (RSU::RRM::ScheduleInfo *p : _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx]) {
			//		if (p == nullptr) continue;
			//		m_EventVec[p->eventId].queuingDelay++;
			//	}
			//}
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
						//更新该事件的日志
						m_EventVec[info->eventId].addEventLog(m_TTI, CONFLICT, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Conflict");

						//首先将事件压入等待列表
						_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx, info->eventId);

						//更新该事件的日志
						m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, -1, "Conflict");

						//记录TTI日志
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, -1);

						//释放调度信息对象的内存资源
						delete info;
						info = nullptr;
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
	for (list<int>&lst : m_InterferenceVec) {
		lst.clear();
	}

	//统计本次的干扰信息
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
					RSU::RRM::ScheduleInfo *info = *lst.begin();
					m_InterferenceVec[patternIdx].push_back(info->VeUEId);
				}
			}
		}
	}

	//更新每辆车的干扰车辆列表	
	for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
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

					if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
						m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
						m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
						m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
					}
					double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

					//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
					int maxEquivalentBitNum = (int)((double)(ns_RRM_ICC_DRA::gc_RBNumPerPattern * gc_BitNumPerRB)* factor);

					//记录调度信息
					info->transimitBitNum = maxEquivalentBitNum;
					info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
					info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

					//该编码方式下，该Pattern在一个TTI传输的实际的有效信息bit数量，并更新信息状态
					int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);
					
					//累计吞吐率
					m_TTIRSUThroughput[m_TTI][_RSU.m_GTT->m_RSUId] += realEquivalentBitNum;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Transimit");
				}
			}
		}
		
	}
	delete copyWTPoint;//getCopy是通过new创建的，因此这里释放资源
	copyWTPoint = nullptr;
}


void RRM_ICC_DRA::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {

				list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_ICC_DRA->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
					RSU::RRM::ScheduleInfo *info = *lst.begin();
					if (m_EventVec[info->eventId].message.isFinished()) {//说明该数据已经传输完毕

						//设置传输成功标记
						m_EventVec[info->eventId].isSuccessded = true;

						//更新该事件的日志
						m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Succeed");

						//记录TTI日志
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx);

					}
					else {//该数据仍未传完，将其压回m_WaitEventIdList，目前暂时是不支持持续占用的<UNDOWN>
						_RSU.m_RRM_ICC_DRA->pushToWaitEventIdList(clusterIdx, info->eventId);
					}
					//释放调度信息对象的内存资源
					delete *lst.begin();
					*lst.begin() = nullptr;

					//释放Pattern资源
					_RSU.m_RRM_ICC_DRA->m_PatternIsAvailable[clusterIdx][patternIdx] = true;
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
				RSU::RRM::ScheduleInfo* &info = _RSU.m_RRM_ICC_DRA->m_ScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				t_File << info->toScheduleString(3) << endl;
			}
			t_File << "        }" << endl;
			t_File << "    }" << endl;
		}

	}
	t_File << "}" << endl;
	t_File << "\n\n" << endl;
}


void RRM_ICC_DRA::writeTTILogInfo(std::ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx) {
	stringstream ss;
	switch (t_EventLogType) {
	case SUCCEED:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ RSU[" << t_RSUId << "]   ClusterIdx[" << t_ClusterIdx << "]    PatternIdx[" << t_PatternIdx << "] }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[0]Succeed";
		t_File << "    " << ss.str() << endl;
		break;
	case EVENT_TO_WAIT:
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[2]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: RSU[" << t_RSUId << "]'s ScheduleTable[" << t_ClusterIdx << "][" << t_PatternIdx << "] ; To: SwitchList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[3]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: RSU[" << t_RSUId << "]'s ScheduleTable[" << t_ClusterIdx << "][" << t_PatternIdx << "] ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[4]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case WAIT_TO_SWITCH:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: RSU[" << t_RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[5]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case WAIT_TO_ADMIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: RSU[" << t_RSUId << "]'s WaitEventIdList ; To: RSU[" << t_RSUId << "]'s AdmitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[6]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case SWITCH_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[8]Switch";
		t_File << "    " << ss.str() << endl;
		break;
	case TRANSIMIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ RSU[" << t_RSUId << "]'s TransimitScheduleInfoList ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[9]Conflict";
		t_File << "    " << ss.str() << endl;
		break;
	case ADMIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << t_EventId << "]: ";
		ss << "{ RSU[" << t_RSUId << "]'s AdmitEventIdList ; To: RSU[" << t_RSUId << "]'s WaitEventIdList }";
		t_File << "[ TTI = " << left << setw(3) << t_TTI << "]";
		t_File << "    " << left << setw(13) << "[11]Conflict";
		t_File << "    " << ss.str() << endl;
		break;
	}
}


void RRM_ICC_DRA::writeClusterPerformInfo(ofstream& t_File) {
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
