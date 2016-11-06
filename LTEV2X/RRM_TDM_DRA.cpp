/*
* =====================================================================================
*
*       Filename:  RRM_TDM_DRA.cpp
*
*    Description:  RRM_TDM_DRA模块
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

#include<tuple>
#include<vector>
#include<list>
#include<sstream>
#include<iomanip>
#include<set>
#include"RRM_TDM_DRA.h"
#include"Exception.h"

using namespace std;

RRM_TDM_DRA::RRM_TDM_DRA(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {

	m_InterferenceVec = vector<list<int>>(ns_RRM_TDM_DRA::gc_TotalPatternNum);
	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num,(threadIdx+1)*num-1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = m_Config.RSUNum - 1;//修正最后一个边界
}


void RRM_TDM_DRA::initialize() {
	//初始化VeUE的该模块参数部分
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeRRM_TDM_DRA();
	}

	//初始化RSU的该模块参数部分
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeRRM_TDM_DRA();
	}
}


void RRM_TDM_DRA::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();

		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(ns_RRM_TDM_DRA::gc_TotalPatternNum, false);
	}
}


void RRM_TDM_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//资源分配信息清空:包括每个RSU内的接入链表等
	informationClean();

	//根据簇大小进行时域资源的划分
	//groupSizeBasedTDM(clusterFlag);
	uniformTDM(clusterFlag);

	//建立接纳链表
	updateAdmitEventIdList(clusterFlag);

	
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


void RRM_TDM_DRA::informationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_RRM_TDM_DRA->m_AdmitEventIdList.clear();
		_RSU.m_RRM_TDM_DRA->m_EmergencyAdmitEventIdList.clear();
	}
}


void RRM_TDM_DRA::groupSizeBasedTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		
		//特殊情况，当该RSU内无一辆车时
		if (_RSU.m_GTT->m_VeUEIdList.size() == 0) {
			/*-----------------------ATTENTION-----------------------
			* 若赋值为(0,-1,0)会导致获取当前簇编号失败，导致其他地方需要讨论
			* 因此直接给每个簇都赋值为整个区间，反正也没有任何作用，免得其他部分讨论
			*------------------------ATTENTION-----------------------*/
			_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTT->m_ClusterNum, tuple<int, int, int>(0, ns_RRM_TDM_DRA::gc_NTTI - 1, ns_RRM_TDM_DRA::gc_NTTI));
			continue;
		}

		//初始化
		_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTT->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		//计算每个TTI时隙对应的VeUE数目(double)，!!!浮点数!!！
		double VeUESizePerTTI = static_cast<double>(_RSU.m_GTT->m_VeUEIdList.size()) / static_cast<double>(ns_RRM_TDM_DRA::gc_NTTI);

		//clusterSize存储每个簇的VeUE数目(double)，!!!浮点数!!！
		vector<double> clusterSize(_RSU.m_GTT->m_ClusterNum, 0);

		//初始化clusterSize
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++)
			clusterSize[clusterIdx] = static_cast<double>(_RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx].size());

		//首先给至少有一辆车的簇分配一份TTI
		int basicNTTI = 0;
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			//如果该簇内至少有一辆VeUE，直接分配给一个单位的时域资源
			if (_RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx].size() != 0) {
				get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) = 1;
				clusterSize[clusterIdx] -= VeUESizePerTTI;
				basicNTTI++;
			}
		}

		//除了给不为空的簇分配的一个TTI外，剩余可分配的TTI数量
		int remainNTTI = ns_RRM_TDM_DRA::gc_NTTI - basicNTTI;

		//对于剩下的资源块，循环将下一时隙分配给当前比例最高的簇，分配之后，更改对应的比例（减去该时隙对应的VeUE数）
		while (remainNTTI > 0) {
			int dex = getMaxIndex(clusterSize);
			if (dex == -1) throw Exp("还存在没有分配的时域资源，但是每个簇内的VeUE已经为负数");
			get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[dex])++;
			remainNTTI--;
			clusterSize[dex] -= VeUESizePerTTI;
		}

		//开始生成区间范围，闭区间
		get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[0]) = 0;
		get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[0]) = get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[0]) + get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[0]) - 1;
		for (int clusterIdx = 1; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) = get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx - 1]) + 1;
			get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) = get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) + get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]) - 1;
		}


		//将调度区间写入该RSU内的每一个车辆
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId].m_RRM_TDM_DRA->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]), get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]));
		}
	}
	writeClusterPerformInfo(g_FileClasterPerformInfo);
}


void RRM_TDM_DRA::uniformTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		//初始化
		_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTT->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		int equalTimeLength = ns_RRM_TDM_DRA::gc_NTTI / _RSU.m_GTT->m_ClusterNum;

		int lastClusterLength = ns_RRM_TDM_DRA::gc_NTTI - equalTimeLength*_RSU.m_GTT->m_ClusterNum + equalTimeLength;

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx] = tuple<int, int, int>(equalTimeLength*clusterIdx, equalTimeLength*(clusterIdx + 1) - 1, equalTimeLength);
		}

		//修复最后一个簇的时域长度，因为平均簇长可能被四舍五入了，因此，平均簇长度*簇数并不等于总调度时间，因此将差异填入最后一个簇
		get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[_RSU.m_GTT->m_ClusterNum - 1]) = ns_RRM_TDM_DRA::gc_NTTI - 1;
		get<2>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[_RSU.m_GTT->m_ClusterNum - 1]) = lastClusterLength;

		

		//将调度区间写入该RSU内的每一个车辆
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId].m_RRM_TDM_DRA->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]), get<1>(_RSU.m_RRM_TDM_DRA->m_ClusterTDRInfo[clusterIdx]));
		}
	}
	writeClusterPerformInfo(g_FileClasterPerformInfo);
}


void RRM_TDM_DRA::updateAdmitEventIdList(bool clusterFlag) {
	//首先，处理System级别的事件触发链表
	processEventList();
	//其次，如果当前TTI进行了位置更新，需要处理调度表
	if (clusterFlag) {
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
		//处理RSU级别的调度链表
		processScheduleInfoTableWhenLocationUpdate();

		//处理RSU级别的等待链表
		processWaitEventIdListWhenLocationUpdate();

		//处理System级别的切换链表
		processSwitchListWhenLocationUpdate();
		if (m_SwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
	}
	//然后，处理RSU级别的等待链表
	processWaitEventIdList();
}


void RRM_TDM_DRA::processEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event &event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];

		if (m_EventVec[eventId].message.messageType == EMERGENCY) {//若是紧急性事件
			/*  EMERGENCY  */
			//将该事件压入紧急事件等待链表
			_RSU.m_RRM_TDM_DRA->pushToEmergencyWaitEventIdList(eventId);

			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "Trigger");

			//记录TTI日志
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			/*  EMERGENCY  */
		}
		else {//非紧急性事件
			//将该事件压入等待链表
			_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(eventId);

			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "Trigger");

			//记录TTI日志
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
		}
	}
}


void RRM_TDM_DRA::processScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			if (_RSU.m_RRM_TDM_DRA->m_EmergencyScheduleInfoTable[patternIdx] == nullptr) continue;

			int eventId = _RSU.m_RRM_TDM_DRA->m_EmergencyScheduleInfoTable[patternIdx]->eventId;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
				//压入Switch链表
				_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

				//将剩余待传bit重置
				m_EventVec[eventId].message.reset();

				//并释放该调度信息的资源
				delete _RSU.m_RRM_TDM_DRA->m_EmergencyScheduleInfoTable[patternIdx];
				_RSU.m_RRM_TDM_DRA->m_EmergencyScheduleInfoTable[patternIdx] = nullptr;

				//释放Pattern资源
				_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx] = true;

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTT->m_RSUId, -1, patternIdx, "LocationUpdate");

				//记录TTI日志
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, -1, patternIdx);
			}
		}
		/*  EMERGENCY  */

		//开始处理 m_ScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
				int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
				if (_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx] == nullptr) continue;


				int eventId = _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
					//压入Switch链表
					_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//将剩余待传bit重置
					m_EventVec[eventId].message.reset();

					//并释放该调度信息的资源
					delete _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx];
					_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx] = nullptr;

					//释放Pattern资源
					_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = true;

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTT->m_RSUId, clusterIdx, relativePatternIdx, "LocationUpdate");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, relativePatternIdx);
				}
				else {
					if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx != clusterIdx) {//RSU内部发生了簇切换，将其从调度表中取出，压入等待链表
						//压入该RSU的等待链表
						_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(eventId);

						//将剩余待传bit重置
						m_EventVec[eventId].message.reset();

						//并释放该调度信息的资源
						delete _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx];
						_RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx][relativePatternIdx] = nullptr;

						//释放Pattern资源
						_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = true;

						//更新该事件的日志
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTT->m_RSUId, clusterIdx, relativePatternIdx, "LocationUpdate");

						//记录TTI日志
						writeTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, clusterIdx, relativePatternIdx);
					}
				}
			}
		}
	}
}


void RRM_TDM_DRA::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		//开始处理m_EmergencyWaitEventIdList
		list<int>::iterator it = _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList.begin();
		while (it != _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE已经不在该RSU范围内
                //将其添加到System级别的RSU切换链表中
				_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);
				
                //从等待链表将其删除
				it = _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList.erase(it);

				//将剩余待传bit重置
				m_EventVec[eventId].message.reset();

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

				//记录TTI日志
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			}
			else {//仍然处于当前RSU范围内
				it++;
				continue; //继续留在当前RSU的Emergency等待链表
			}
		}
		/*  EMERGENCY  */


		//开始处理 m_WaitEventIdList
		it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.begin();
		while (it != _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTT->m_RSUId != _RSU.m_GTT->m_RSUId) {//该VeUE已经不在该RSU范围内
				//将其添加到System级别的RSU切换链表中
				_RSU.m_RRM_TDM_DRA->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);
				
			    //将其从等待链表中删除
				it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.erase(it);

				//将剩余待传bit重置
				m_EventVec[eventId].message.reset();

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

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


void RRM_TDM_DRA::processSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_SwitchEventIdList.begin();
	while (it != m_SwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
																
		if (m_EventVec[eventId].message.messageType == EMERGENCY) {//属于紧急事件
			/*  EMERGENCY  */
			//转入等待链表
			_RSU.m_RRM_TDM_DRA->pushToEmergencyWaitEventIdList(eventId);

			//从Switch表中将其删除
			it = m_SwitchEventIdList.erase(it);

			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

			//记录TTI日志
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			/*  EMERGENCY  */
		}else {//非紧急事件
			 //转入等待链表
			_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(eventId);

			//从Switch表中将其删除
			it = m_SwitchEventIdList.erase(it);

			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "LocationUpdate");

			//记录TTI日志
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
		}
	}
}


void RRM_TDM_DRA::processWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList) {
			_RSU.m_RRM_TDM_DRA->pushToEmergencyAdmitEventIdList(eventId);//将事件压入接入链表

		    //更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTT->m_RSUId, -1, -1, "TryAccept");

			//记录TTI日志
			writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
		}

		//由于会全部压入Emergency接入链表，因此直接清空就行了，不需要一个个删除
		_RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList.clear();
		/*  EMERGENCY  */


		//开始处理 m_WaitEventIdList
		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);//处于调度中的簇编号
		list<int>::iterator it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.begin();
		while (it != _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx == clusterIdx) {//该事件当前可以进行调度
				_RSU.m_RRM_TDM_DRA->pushToAdmitEventIdList(eventId);//添加到RSU级别的接纳链表中
				it = _RSU.m_RRM_TDM_DRA->m_WaitEventIdList.erase(it);//将其从等待链表中删除

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTT->m_RSUId, -1, -1, "TryAccept");

				//记录TTI日志
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
			}
			else {//该事件当前不可以进行调度
				it++;
				continue; //继续等待，当前TTI不做处理
			}
		}
	}
}


void RRM_TDM_DRA::selectBasedOnP123() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		vector<int> curAvaliableEmergencyPatternIdx;//当前可用的EmergencyPattern编号

		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			if (_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx]) {
				curAvaliableEmergencyPatternIdx.push_back(patternIdx);
			}
		}

		for (int eventId : _RSU.m_RRM_TDM_DRA->m_EmergencyAdmitEventIdList) {
			int VeUEId = m_EventVec[eventId].VeUEId;

			//为当前用户在可用的EmergencyPattern块中随机选择一个，每个用户自行随机选择可用EmergencyPattern块
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_TDM_DRA->selectEmergencyRBBasedOnP2(curAvaliableEmergencyPatternIdx);

			if (patternIdx == -1) {//无对应Pattern类型的pattern资源可用
				_RSU.m_RRM_TDM_DRA->pushToEmergencyWaitEventIdList(eventId);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "AllBusy");

				//记录TTI日志
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);

				continue;
			}
			//将资源标记为占用
			_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx] = false;

			//将调度信息压入m_EmergencyTransimitEventIdList中
			_RSU.m_RRM_TDM_DRA->pushToEmergencyTransmitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, patternIdx), patternIdx);
		}
		_RSU.m_RRM_TDM_DRA->pullFromEmergencyScheduleInfoTable();
		/*  EMERGENCY  */


		//开始处理非Emergency的事件
		int roundATTI = m_TTI%ns_RRM_TDM_DRA::gc_NTTI;//将TTI映射到[0-gc_DRA_NTTI)的范围

		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);

		/*
		* 当前TTI可用的Pattern块编号
		* 下标代表的Pattern种类的编号，其中Emergency那一项是空的，但是保留，想避免编号产生偏移
		* 每个内层vector代表该种类Pattern可用的Pattern编号(绝对Pattern编号)
		*/
		vector<vector<int>> curAvaliablePatternIdx(ns_RRM_TDM_DRA::gc_PatternTypeNum);

		for (int patternTypeIdx = 1; patternTypeIdx < ns_RRM_TDM_DRA::gc_PatternTypeNum; patternTypeIdx++) {
			for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[patternTypeIdx][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[patternTypeIdx][1]; patternIdx++) {
				int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
				if (_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		for (int eventId : _RSU.m_RRM_TDM_DRA->m_AdmitEventIdList) {//遍历该簇内接纳链表中的事件

			int VeUEId = m_EventVec[eventId].VeUEId;

			//为当前用户在可用的对应其事件类型的Pattern块中随机选择一个，每个用户自行随机选择可用Pattern块
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_TDM_DRA->selectRBBasedOnP2(curAvaliablePatternIdx, m_EventVec[eventId].message.messageType);
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];

			if (patternIdx == -1) {//该用户传输的信息类型没有pattern剩余了
				_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(eventId);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "AllBusy");

				//记录TTI日志
				writeTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTT->m_RSUId, -1, -1);
				continue;
			}

			//将资源标记为占用
			_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = false;

			//将调度信息压入m_TransimitEventIdList中
			_RSU.m_RRM_TDM_DRA->pushToTransmitScheduleInfoList(new RSU::RRM::ScheduleInfo(eventId, VeUEId, _RSU.m_GTT->m_RSUId, patternIdx), patternIdx);
		}

		//将调度表中当前可以继续传输的用户压入传输链表中
		_RSU.m_RRM_TDM_DRA->pullFromScheduleInfoTable(m_TTI);
	}
}

void RRM_TDM_DRA::delaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*
		* 该函数应该在selectBasedOnP..之后调用
		* 此时事件处于等待链表、传输链表、或者调度表中（非当前簇）
		*/

		//处理等待链表
		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_TDM_DRA->m_EmergencyWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;
		/*  EMERGENCY  */

		for (int eventId : _RSU.m_RRM_TDM_DRA->m_WaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//处理此刻正在将要传输的传输链表
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++)
			for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		/*  EMERGENCY  */

		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			for (RSU::RRM::ScheduleInfo* &p : _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx])
				m_EventVec[p->eventId].sendDelay++;
		}

		//处理此刻位于调度表中，但不属于当前簇的事件
		int curClusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			if (clusterIdx == curClusterIdx) continue;
			for (RSU::RRM::ScheduleInfo *p : _RSU.m_RRM_TDM_DRA->m_ScheduleInfoTable[clusterIdx]) {
				if (p == nullptr) continue;
				m_EventVec[p->eventId].queuingDelay++;
			}
		}
	}
}

void RRM_TDM_DRA::conflictListener() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
				for (RSU::RRM::ScheduleInfo* &info : lst) {
					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTT->m_RSUId, -1, patternIdx, "Transimit");

					//首先将事件压入等待列表
					_RSU.m_RRM_TDM_DRA->pushToEmergencyWaitEventIdList(info->eventId);

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "Conflict");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, -1, -1);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
				}

				//释放Pattern资源
				_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx] = true;

				lst.clear();
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
				for (RSU::RRM::ScheduleInfo* &info : lst) {
					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Transimit");

					//首先将事件压入等待列表
					_RSU.m_RRM_TDM_DRA->pushToWaitEventIdList(info->eventId);

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTT->m_RSUId, -1, -1, "Conflict");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTT->m_RSUId, -1, -1);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
				}
				//释放Pattern资源
				_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = true;

				lst.clear();
			}
		}
	}
}


void RRM_TDM_DRA::transimitPreparation() {
	//首先清空上一次干扰信息
	for (list<int>&lst : m_InterferenceVec) {
		lst.clear();
	}

	//统计本次的干扰信息
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM::ScheduleInfo *info = *lst.begin();

				m_InterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
		/*  EMERGENCY  */

		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
				RSU::RRM::ScheduleInfo *info = *lst.begin();

				m_InterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
	}
    
	//更新每辆车的干扰车辆列表	
	for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
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


void RRM_TDM_DRA::transimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_TDM_DRA::transimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}

void RRM_TDM_DRA::transimitStartThread(int fromRSUId, int toRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//由于每个线程的该模块会有不同的状态且无法共享，因此这里拷贝该模块用于本次计算
	for (int RSUId = fromRSUId; RSUId <= toRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				//计算SINR，获取调制编码方式
				pair<int, int> &subCarrierIdxRange = getOccupiedSubCarrierRange(m_EventVec[info->eventId].message.messageType, patternIdx);
				g_FileTemp << "Emergency PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				//if (!m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
				int maxEquivalentBitNum = (int)((double)(ns_RRM_TDM_DRA::gc_RBNumPerPatternType[EMERGENCY] * gc_BitNumPerRB)* factor);

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
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];

			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				int patternType = getPatternType(patternIdx);

				//计算SINR，获取调制编码方式
				pair<int, int> &subCarrierIdxRange = getOccupiedSubCarrierRange(m_EventVec[info->eventId].message.messageType, patternIdx);
				g_FileTemp << "NonEmergencyPatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << ((patternType == 0) ? "Emergency" : (patternType == 1 ? "Period" : "Data")) << endl;

				//if ( !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
				int maxEquivalentBitNum = (int)((double)(ns_RRM_TDM_DRA::gc_RBNumPerPatternType[patternType] * gc_BitNumPerRB)* factor);
				
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
	delete copyWTPoint;//getCopy是通过new创建的，因此这里释放资源
	copyWTPoint = nullptr;
}

void RRM_TDM_DRA::writeScheduleInfo(ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		out << "    RSU[" << _RSU.m_GTT->m_RSUId << "][TTI = " << m_TTI << "]" << endl;
		out << "    {" << endl;
		out << "    EMERGENCY:" << endl;
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    PERIOD:" << endl;
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[PERIOD][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[PERIOD][1]; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    DATA:" << endl;
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[DATA][0]; patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[DATA][1]; patternIdx++) {
			int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
			bool isAvaliable = _RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM::ScheduleInfo *info = *(_RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    }" << endl;

	}
	out << "}" << endl;
	out << "\n\n" << endl;
}

void RRM_TDM_DRA::transimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_EmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				if (m_EventVec[info->eventId].message.isFinished()) {//已经传输完毕，将资源释放

					//设置传输成功标记
					m_EventVec[info->eventId].isSuccessded = true;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, -1, patternIdx, "Succeed");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, -1, patternIdx);

					//释放调度信息对象的内存资源
					delete *lst.begin();
					*lst.begin() = nullptr;

					//释放Pattern资源
					_RSU.m_RRM_TDM_DRA->m_EmergencyPatternIsAvailable[patternIdx] = true;
				}
				else {//尚未传输完毕
					_RSU.m_RRM_TDM_DRA->pushToEmergencyScheduleInfoTable(patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			lst.clear();
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_TDM_DRA->getClusterIdx(m_TTI);
		for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
			int relativePatternIdx= patternIdx- ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];

			list<RSU::RRM::ScheduleInfo*> &lst = _RSU.m_RRM_TDM_DRA->m_TransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
				RSU::RRM::ScheduleInfo *info = *lst.begin();
				if (m_EventVec[info->eventId].message.isFinished()) {//说明该数据已经传输完毕

					//设置传输成功标记
					m_EventVec[info->eventId].isSuccessded = true;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx, "Succeed");

					//记录TTI日志
					writeTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTT->m_RSUId, clusterIdx, patternIdx);

					//释放调度信息对象的内存资源
					delete *lst.begin();
					*lst.begin() = nullptr;

					//释放Pattern资源
					_RSU.m_RRM_TDM_DRA->m_PatternIsAvailable[clusterIdx][relativePatternIdx] = true;
				}
				else {//该数据仍未传完，将其压回m_ScheduleInfoTable
					_RSU.m_RRM_TDM_DRA->pushToScheduleInfoTable(clusterIdx, patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			//处理完后，将该pattern上的数据清空（此时要不本身就是空，要不就是nullptr指针）
			lst.clear();
		}
	}
}


void RRM_TDM_DRA::writeTTILogInfo(ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx) {
	stringstream ss;
	switch (type) {
	case SUCCEED:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]   ClusterIdx[" << clusterIdx << "]    PatternIdx[" << patternIdx << "] }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[0]Succeed";
		out << "    " << ss.str() << endl;
		break;
	case EVENT_TO_WAIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: EventList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[2]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[3]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s ScheduleTable[" << clusterIdx << "][" << patternIdx << "] ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[4]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_SWITCH:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: SwitchList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[5]Switch";
		out << "    " << ss.str() << endl;
		break;
	case WAIT_TO_ADMIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: RSU[" << RSUId << "]'s WaitEventIdList ; To: RSU[" << RSUId << "]'s AdmitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[6]Switch";
		out << "    " << ss.str() << endl;
		break;
	case SWITCH_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ From: SwitchList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[8]Switch";
		out << "    " << ss.str() << endl;
		break;
	case TRANSIMIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]'s TransimitScheduleInfoList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[9]Conflict";
		out << "    " << ss.str() << endl;
		break;
	case ADMIT_TO_WAIT:
		ss.str("");
		ss << "Event[ " << left << setw(3) << eventId << "]: ";
		ss << "{ RSU[" << RSUId << "]'s AdmitEventIdList ; To: RSU[" << RSUId << "]'s WaitEventIdList }";
		out << "[ TTI = " << left << setw(3) << TTI << "]";
		out << "    " << left << setw(13) << "[11]Conflict";
		out << "    " << ss.str() << endl;
		break;
	}
}


void RRM_TDM_DRA::writeClusterPerformInfo(ofstream &out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;

	//打印VeUE信息
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUE &_VeUE = m_VeUEAry[VeUEId];
		out << _VeUE.m_RRM_TDM_DRA->toString(2) << endl;
	}
	out << "    }\n" << endl;

	////打印基站信息
	//out << "    eNB Info: " << endl;
	//out << "    {" << endl;
	//for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
	//	ceNB &_eNB = m_eNBAry[eNBId];
	//	out << _eNB.toString(2) << endl;
	//}
	//out << "    }\n" << endl;

	//打印RSU信息
	out << "    RSU Info: " << endl;
	out << "    {" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		out << _RSU.m_RRM_TDM_DRA->toString(2) << endl;
	}
	out << "    }" << endl;

	out << "}\n\n";
}


int RRM_TDM_DRA::getMaxIndex(const vector<double>&clusterSize) {
	double max = 0;
	int dex = -1;
	for (int i = 0; i < static_cast<int>(clusterSize.size()); i++) {
		if (clusterSize[i] > max) {
			dex = i;
			max = clusterSize[i];
		}
	}
	return dex;
}


int RRM_TDM_DRA::getPatternType(int patternIdx) {
	//patternIdx指所有事件类型的Pattern的绝对序号，从0开始编号，包括Emergency
	for (int patternType = 0; patternType < ns_RRM_TDM_DRA::gc_PatternTypeNum; patternType++) {
		if (patternIdx >= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[patternType][0] && patternIdx <= ns_RRM_TDM_DRA::gc_PatternTypePatternIdxInterval[patternType][1])
			return patternType;
	}
	throw Exp("getPatternType");
}


pair<int, int> RRM_TDM_DRA::getOccupiedSubCarrierRange(MessageType messageType, int patternIdx) {

	pair<int, int> res;

	int offset = 0;
	for (int i = 0; i < messageType; i++) {
		offset += ns_RRM_TDM_DRA::gc_RBNumPerPatternType[i] * ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[i];
		patternIdx -= ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[i];
	}
	res.first = offset + ns_RRM_TDM_DRA::gc_RBNumPerPatternType[messageType] * patternIdx;
	res.second = offset + ns_RRM_TDM_DRA::gc_RBNumPerPatternType[messageType] * (patternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
