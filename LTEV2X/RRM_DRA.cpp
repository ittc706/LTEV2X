/*
* =====================================================================================
*
*       Filename:  RRM_DRA.cpp
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

#include<tuple>
#include<vector>
#include<list>
#include<sstream>
#include<iomanip>
#include<set>
#include"RRM_DRA.h"
#include"Exception.h"

using namespace std;

RRM_DRA::RRM_DRA(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput, DRAMode systemDRAMode, GTAT_Basic* systemGTATPoint, WT_Basic* systemWTPoint, int threadNum) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList, systemTTIRSUThroughput), m_DRAMode(systemDRAMode), m_GTATPoint(systemGTATPoint), m_WTPoint(systemWTPoint), m_ThreadNum(threadNum) {

	m_DRAInterferenceVec = std::vector<std::list<int>>(gc_DRATotalPatternNum);
	m_ThreadsRSUIdRange = vector<pair<int, int>>(threadNum);

	int num = m_Config.RSUNum / threadNum;
	for (int threadIdx = 0; threadIdx < threadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num,(threadIdx+1)*num-1);
	}
	m_ThreadsRSUIdRange[threadNum - 1].second = m_Config.RSUNum - 1;//修正最后一个边界
}


void RRM_DRA::initialize() {
	//初始化VeUE的该模块参数部分
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeDRA();
	}

	//初始化RSU的该模块参数部分
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeDRA();
	}
}


void RRM_DRA::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();

		m_VeUEAry[VeUEId].m_RRM->m_isWTCached.assign(gc_DRATotalPatternNum, false);
	}
}


void RRM_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//资源分配信息清空:包括每个RSU内的接入链表等
	DRAInformationClean();

	//根据簇大小进行时域资源的划分
	//DRAGroupSizeBasedTDM(clusterFlag);
	DRAEqualTDM(clusterFlag);

	//建立接纳链表
	DRAUpdateAdmitEventIdList(clusterFlag);

	//当前m_TTI的DRA算法
	switch (m_DRAMode) {
	case P13:
		DRASelectBasedOnP13();
		break;
	case P23:
		DRASelectBasedOnP23();
		break;
	case P123:
		DRASelectBasedOnP123();
		break;
	}
	//统计时延信息
	DRADelaystatistics();

	//帧听冲突
	DRAConflictListener();

	//请求地理拓扑单元计算干扰响应矩阵
	DRATransimitPreparation();

	//模拟传输开始，更新调度信息
	DRATransimitStart();

	//写入调度信息
	DRAWriteScheduleInfo(g_FileDRAScheduleInfo);

	//模拟传输结束，统计吞吐量
	DRATransimitEnd();
}


void RRM_DRA::DRAInformationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_RRM_DRA->m_DRAAdmitEventIdList.clear();
		_RSU.m_RRM_DRA->m_DRAEmergencyAdmitEventIdList.clear();
	}
}


void RRM_DRA::DRAGroupSizeBasedTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		
		//特殊情况，当该RSU内无一辆车时
		if (_RSU.m_GTAT->m_VeUEIdList.size() == 0) {
			/*-----------------------ATTENTION-----------------------
			* 若赋值为(0,-1,0)会导致获取当前簇编号失败，导致其他地方需要讨论
			* 因此直接给每个簇都赋值为整个区间，反正也没有任何作用，免得其他部分讨论
			*------------------------ATTENTION-----------------------*/
			_RSU.m_RRM_DRA->m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTAT->m_ClusterNum, tuple<int, int, int>(0, gc_DRA_NTTI - 1, gc_DRA_NTTI));
			continue;
		}

		//初始化
		_RSU.m_RRM_DRA->m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTAT->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		//计算每个TTI时隙对应的VeUE数目(double)，!!!浮点数!!！
		double VeUESizePerTTI = static_cast<double>(_RSU.m_GTAT->m_VeUEIdList.size()) / static_cast<double>(gc_DRA_NTTI);

		//clusterSize存储每个簇的VeUE数目(double)，!!!浮点数!!！
		std::vector<double> clusterSize(_RSU.m_GTAT->m_ClusterNum, 0);

		//初始化clusterSize
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++)
			clusterSize[clusterIdx] = static_cast<double>(_RSU.m_GTAT->m_DRAClusterVeUEIdList[clusterIdx].size());

		//首先给至少有一辆车的簇分配一份TTI
		int basicNTTI = 0;
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			//如果该簇内至少有一辆VeUE，直接分配给一个单位的时域资源
			if (_RSU.m_GTAT->m_DRAClusterVeUEIdList[clusterIdx].size() != 0) {
				get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) = 1;
				clusterSize[clusterIdx] -= VeUESizePerTTI;
				basicNTTI++;
			}
		}

		//除了给不为空的簇分配的一个TTI外，剩余可分配的TTI数量
		int remainNTTI = gc_DRA_NTTI - basicNTTI;

		//对于剩下的资源块，循环将下一时隙分配给当前比例最高的簇，分配之后，更改对应的比例（减去该时隙对应的VeUE数）
		while (remainNTTI > 0) {
			int dex = DRAGetMaxIndex(clusterSize);
			if (dex == -1) throw Exp("还存在没有分配的时域资源，但是每个簇内的VeUE已经为负数");
			get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[dex])++;
			remainNTTI--;
			clusterSize[dex] -= VeUESizePerTTI;
		}

		//开始生成区间范围，闭区间
		get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[0]) = 0;
		get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[0]) = get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[0]) + get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[0]) - 1;
		for (int clusterIdx = 1; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) = get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx - 1]) + 1;
			get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) = get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) + get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]) - 1;
		}


		//将调度区间写入该RSU内的每一个车辆
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU.m_GTAT->m_DRAClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId].m_RRM_DRA->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]), get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]));
		}
	}
	DRAWriteClusterPerformInfo(g_FileClasterPerformInfo);
}


void RRM_DRA::DRAEqualTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		//初始化
		_RSU.m_RRM_DRA->m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSU.m_GTAT->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		int equalTimeLength = gc_DRA_NTTI / _RSU.m_GTAT->m_ClusterNum;

		int lastClusterLength = gc_DRA_NTTI - equalTimeLength*_RSU.m_GTAT->m_ClusterNum + equalTimeLength;

		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx] = tuple<int, int, int>(equalTimeLength*clusterIdx, equalTimeLength*(clusterIdx + 1) - 1, equalTimeLength);
		}

		//修复最后一个簇的时域长度，因为平均簇长可能被四舍五入了，因此，平均簇长度*簇数并不等于总调度时间，因此将差异填入最后一个簇
		get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[_RSU.m_GTAT->m_ClusterNum - 1]) = gc_DRA_NTTI - 1;
		get<2>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[_RSU.m_GTAT->m_ClusterNum - 1]) = lastClusterLength;

		

		//将调度区间写入该RSU内的每一个车辆
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU.m_GTAT->m_DRAClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId].m_RRM_DRA->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]), get<1>(_RSU.m_RRM_DRA->m_DRAClusterTDRInfo[clusterIdx]));
		}
	}
	DRAWriteClusterPerformInfo(g_FileClasterPerformInfo);
}


void RRM_DRA::DRAUpdateAdmitEventIdList(bool clusterFlag) {
	//首先，处理System级别的事件触发链表
	DRAProcessEventList();
	//其次，如果当前TTI进行了位置更新，需要处理调度表
	if (clusterFlag) {
		if (m_DRASwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
		//处理RSU级别的调度链表
		DRAProcessScheduleInfoTableWhenLocationUpdate();

		//处理RSU级别的等待链表
		DRAProcessWaitEventIdListWhenLocationUpdate();

		//处理System级别的切换链表
		DRAProcessSwitchListWhenLocationUpdate();
		if (m_DRASwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
	}
	//然后，处理RSU级别的等待链表
	DRAProcessWaitEventIdList();
}


void RRM_DRA::DRAProcessEventList() {
	for (int eventId : m_EventTTIList[m_TTI]) {
		Event event = m_EventVec[eventId];
		int VeUEId = event.VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTAT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];

		if (m_EventVec[eventId].message.messageType == EMERGENCY) {//若是紧急性事件
			/*  EMERGENCY  */
			//将该事件压入紧急事件等待链表
			_RSU.m_RRM_DRA->DRAPushToEmergencyWaitEventIdList(eventId);

			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Trigger");

			//记录TTI日志
			DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
			/*  EMERGENCY  */
		}
		else {//非紧急性事件
			//将该事件压入等待链表
			_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(eventId);

			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Trigger");

			//记录TTI日志
			DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
		}
	}
}


void RRM_DRA::DRAProcessScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			if (_RSU.m_RRM_DRA->m_DRAEmergencyScheduleInfoTable[patternIdx] == nullptr) continue;

			int eventId = _RSU.m_RRM_DRA->m_DRAEmergencyScheduleInfoTable[patternIdx]->eventId;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
				//压入Switch链表
				_RSU.m_RRM_DRA->DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);

				//将剩余待传bit重置
				m_EventVec[eventId].message.reset();

				//并释放该调度信息的资源
				delete _RSU.m_RRM_DRA->m_DRAEmergencyScheduleInfoTable[patternIdx];
				_RSU.m_RRM_DRA->m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;

				//释放Pattern资源
				_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx] = true;

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "LocationUpdate");

				//记录TTI日志
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1, patternIdx);
			}
		}
		/*  EMERGENCY  */

		//开始处理 m_DRAScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
				int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
				if (_RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx] == nullptr) continue;


				int eventId = _RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
					//压入Switch链表
					_RSU.m_RRM_DRA->DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);

					//将剩余待传bit重置
					m_EventVec[eventId].message.reset();

					//并释放该调度信息的资源
					delete _RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx];
					_RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx] = nullptr;

					//释放Pattern资源
					_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][relativePatternIdx] = true;

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU.m_GTAT->m_RSUId, clusterIdx, relativePatternIdx, "LocationUpdate");

					//记录TTI日志
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, clusterIdx, relativePatternIdx);
				}
				else {
					if (_RSU.m_RRM_DRA->DRAGetClusterIdxOfVeUE(VeUEId) != clusterIdx) {//RSU内部发生了簇切换，将其从调度表中取出，压入等待链表
						//压入该RSU的等待链表
						_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(eventId);

						//将剩余待传bit重置
						m_EventVec[eventId].message.reset();

						//并释放该调度信息的资源
						delete _RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx];
						_RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx] = nullptr;

						//释放Pattern资源
						_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][relativePatternIdx] = true;

						//更新该事件的日志
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSU.m_GTAT->m_RSUId, clusterIdx, relativePatternIdx, "LocationUpdate");

						//记录TTI日志
						DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, clusterIdx, relativePatternIdx);
					}
				}
			}
		}
	}
}


void RRM_DRA::DRAProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		//开始处理m_DRAEmergencyWaitEventIdList
		list<int>::iterator it = _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList.begin();
		while (it != _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//该VeUE已经不在该RSU范围内
                //将其添加到System级别的RSU切换链表中
				_RSU.m_RRM_DRA->DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);
				
                //从等待链表将其删除
				it = _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList.erase(it);

				//将剩余待传bit重置
				m_EventVec[eventId].message.reset();

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

				//记录TTI日志
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
			}
			else {//仍然处于当前RSU范围内
				it++;
				continue; //继续留在当前RSU的Emergency等待链表
			}
		}
		/*  EMERGENCY  */


		//开始处理 m_DRAWaitEventIdList
		it = _RSU.m_RRM_DRA->m_DRAWaitEventIdList.begin();
		while (it != _RSU.m_RRM_DRA->m_DRAWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_RSUId != _RSU.m_GTAT->m_RSUId) {//该VeUE已经不在该RSU范围内
				//将其添加到System级别的RSU切换链表中
				_RSU.m_RRM_DRA->DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);
				
			    //将其从等待链表中删除
				it = _RSU.m_RRM_DRA->m_DRAWaitEventIdList.erase(it);

				//将剩余待传bit重置
				m_EventVec[eventId].message.reset();

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

				//记录TTI日志
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
			}
			else {//仍然处于当前RSU范围内
				it++;
				continue; //继续留在当前RSU的等待链表
			}
		}
	}
}


void RRM_DRA::DRAProcessSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_DRASwitchEventIdList.begin();
	while (it != m_DRASwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = m_EventVec[eventId].VeUEId;
		int RSUId = m_VeUEAry[VeUEId].m_GTAT->m_RSUId;
		RSU &_RSU = m_RSUAry[RSUId];
		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);//当前可传输数据的簇编号

																
		if (m_EventVec[eventId].message.messageType == EMERGENCY) {//属于紧急事件
			/*  EMERGENCY  */
			//转入等待链表
			_RSU.m_RRM_DRA->DRAPushToEmergencyWaitEventIdList(eventId);

			//从Switch表中将其删除
			it = m_DRASwitchEventIdList.erase(it);

			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

			//记录TTI日志
			DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
			/*  EMERGENCY  */
		}else {//非紧急事件
			 //转入等待链表
			_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(eventId);

			//从Switch表中将其删除
			it = m_DRASwitchEventIdList.erase(it);

			//更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "LocationUpdate");

			//记录TTI日志
			DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
		}
	}
}


void RRM_DRA::DRAProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList) {
			_RSU.m_RRM_DRA->DRAPushToEmergencyAdmitEventIdList(eventId);//将事件压入接入链表

		    //更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTAT->m_RSUId, -1, -1, "TryAccept");

			//记录TTI日志
			DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
		}

		//由于会全部压入Emergency接入链表，因此直接清空就行了，不需要一个个删除
		_RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList.clear();
		/*  EMERGENCY  */


		//开始处理 m_DRAWaitEventIdList
		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);//处于调度中的簇编号
		list<int>::iterator it = _RSU.m_RRM_DRA->m_DRAWaitEventIdList.begin();
		while (it != _RSU.m_RRM_DRA->m_DRAWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx == clusterIdx) {//该事件当前可以进行调度
				_RSU.m_RRM_DRA->DRAPushToAdmitEventIdList(eventId);//添加到RSU级别的接纳链表中
				it = _RSU.m_RRM_DRA->m_DRAWaitEventIdList.erase(it);//将其从等待链表中删除

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSU.m_GTAT->m_RSUId, -1, -1, "TryAccept");

				//记录TTI日志
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
			}
			else {//该事件当前不可以进行调度
				it++;
				continue; //继续等待，当前TTI不做处理
			}
		}
	}
}


void RRM_DRA::DRASelectBasedOnP13() {
	
}


void RRM_DRA::DRASelectBasedOnP23() {

}


void RRM_DRA::DRASelectBasedOnP123() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		vector<int> curAvaliableEmergencyPatternIdx;//当前可用的EmergencyPattern编号

		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			if (_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx]) {
				curAvaliableEmergencyPatternIdx.push_back(patternIdx);
			}
		}

		for (int eventId : _RSU.m_RRM_DRA->m_DRAEmergencyAdmitEventIdList) {
			int VeUEId = m_EventVec[eventId].VeUEId;

			//为当前用户在可用的EmergencyPattern块中随机选择一个，每个用户自行随机选择可用EmergencyPattern块
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_DRA->DRARBEmergencySelectBasedOnP2(curAvaliableEmergencyPatternIdx);

			if (patternIdx == -1) {//无对应Pattern类型的pattern资源可用
				_RSU.m_RRM_DRA->DRAPushToEmergencyWaitEventIdList(eventId);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "AllBusy");

				//记录TTI日志
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);

				continue;
			}
			//将资源标记为占用
			_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx] = false;

			//将调度信息压入m_DRAEmergencyTransimitEventIdList中
			_RSU.m_RRM_DRA->DRAPushToEmergencyTransmitScheduleInfoList(new RSU::RRM_DRA::DRAScheduleInfo(eventId, VeUEId, _RSU.m_GTAT->m_RSUId, patternIdx), patternIdx);
		}
		_RSU.m_RRM_DRA->DRAPullFromEmergencyScheduleInfoTable();
		/*  EMERGENCY  */


		//开始处理非Emergency的事件
		int roundATTI = m_TTI%gc_DRA_NTTI;//将TTI映射到[0-gc_DRA_NTTI)的范围

		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);

		/*
		* 当前TTI可用的Pattern块编号
		* 下标代表的Pattern种类的编号，其中Emergency那一项是空的，但是保留，想避免编号产生偏移
		* 每个内层vector代表该种类Pattern可用的Pattern编号(绝对Pattern编号)
		*/
		vector<vector<int>> curAvaliablePatternIdx(gc_DRAPatternTypeNum);

		for (int patternTypeIdx = 1; patternTypeIdx < gc_DRAPatternTypeNum; patternTypeIdx++) {
			for (int patternIdx = gc_DRAPatternTypePatternIdxInterval[patternTypeIdx][0]; patternIdx <= gc_DRAPatternTypePatternIdxInterval[patternTypeIdx][1]; patternIdx++) {
				int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
				if (_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][relativePatternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		for (int eventId : _RSU.m_RRM_DRA->m_DRAAdmitEventIdList) {//遍历该簇内接纳链表中的事件

			int VeUEId = m_EventVec[eventId].VeUEId;

			//为当前用户在可用的对应其事件类型的Pattern块中随机选择一个，每个用户自行随机选择可用Pattern块
			int patternIdx = m_VeUEAry[VeUEId].m_RRM_DRA->DRARBSelectBasedOnP2(curAvaliablePatternIdx, m_EventVec[eventId].message.messageType);
			int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];

			if (patternIdx == -1) {//该用户传输的信息类型没有pattern剩余了
				_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(eventId);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "AllBusy");

				//记录TTI日志
				DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSU.m_GTAT->m_RSUId, -1, -1);
				continue;
			}

			//将资源标记为占用
			_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][relativePatternIdx] = false;

			//将调度信息压入m_DRATransimitEventIdList中
			_RSU.m_RRM_DRA->DRAPushToTransmitScheduleInfoList(new RSU::RRM_DRA::DRAScheduleInfo(eventId, VeUEId, _RSU.m_GTAT->m_RSUId, patternIdx), patternIdx);
		}

		//将调度表中当前可以继续传输的用户压入传输链表中
		_RSU.m_RRM_DRA->DRAPullFromScheduleInfoTable(m_TTI);
	}
}

void RRM_DRA::DRADelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*
		* 该函数应该在DRASelectBasedOnP..之后调用
		* 此时事件处于等待链表、传输链表、或者调度表中（非当前簇）
		*/

		//处理等待链表
		/*  EMERGENCY  */
		for (int eventId : _RSU.m_RRM_DRA->m_DRAEmergencyWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;
		/*  EMERGENCY  */

		for (int eventId : _RSU.m_RRM_DRA->m_DRAWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//处理此刻正在将要传输的传输链表
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++)
			for (RSU::RRM_DRA::DRAScheduleInfo* &p : _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		/*  EMERGENCY  */

		for (int patternIdx = gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
			for (RSU::RRM_DRA::DRAScheduleInfo* &p : _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[relativePatternIdx])
				m_EventVec[p->eventId].sendDelay++;
		}

		//处理此刻位于调度表中，但不属于当前簇的事件
		int curClusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			if (clusterIdx == curClusterIdx) continue;
			for (RSU::RRM_DRA::DRAScheduleInfo *p : _RSU.m_RRM_DRA->m_DRAScheduleInfoTable[clusterIdx]) {
				if (p == nullptr) continue;
				m_EventVec[p->eventId].queuingDelay++;
			}
		}
	}
}

void RRM_DRA::DRAConflictListener() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
				for (RSU::RRM_DRA::DRAScheduleInfo* &info : lst) {
					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Transimit");

					//首先将事件压入等待列表
					_RSU.m_RRM_DRA->DRAPushToEmergencyWaitEventIdList(info->eventId);

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Conflict");

					//记录TTI日志
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTAT->m_RSUId, -1, -1);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
				}

				//释放Pattern资源
				_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx] = true;

				lst.clear();
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int patternIdx = gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
			
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
				for (RSU::RRM_DRA::DRAScheduleInfo* &info : lst) {
					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx, "Transimit");

					//首先将事件压入等待列表
					_RSU.m_RRM_DRA->DRAPushToWaitEventIdList(info->eventId);

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSU.m_GTAT->m_RSUId, -1, -1, "Conflict");

					//记录TTI日志
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU.m_GTAT->m_RSUId, -1, -1);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
				}
				//释放Pattern资源
				_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][relativePatternIdx] = true;

				lst.clear();
			}
		}
	}
}


void RRM_DRA::DRATransimitPreparation() {
	//首先清空上一次干扰信息
	for (std::list<int>&lst : m_DRAInterferenceVec) {
		lst.clear();
	}

	//统计本次的干扰信息
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();

				m_DRAInterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
		/*  EMERGENCY  */

		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int patternIdx = gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
			
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();

				m_DRAInterferenceVec[patternIdx].push_back(info->VeUEId);
			}
		}
	}
    
	//更新每辆车的干扰车辆列表	
	for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
		list<int>& lst = m_DRAInterferenceVec[patternIdx];
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
	m_GTATPoint->calculateInterference(m_DRAInterferenceVec);
	long double end = clock();
	m_GTATTimeConsume += end - start;
}


void RRM_DRA::DRATransimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_DRA::DRATransimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}

void RRM_DRA::DRATransimitStartThread(int fromRSUId, int toRSUId) {
	WT_Basic* copyWTPoint = m_WTPoint->getCopy();//由于每个线程的该模块会有不同的状态且无法共享，因此这里拷贝该模块用于本次计算
	for (int RSUId = fromRSUId; RSUId <= toRSUId; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				//计算SINR，获取调制编码方式
				pair<int, int> &subCarrierIdxRange = DRAGetOccupiedSubCarrierRange(m_EventVec[info->eventId].message.messageType, patternIdx);
				g_FileTemp << "Emergency PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				//if (!m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
				int maxEquivalentBitNum = (int)((double)(gc_DRA_RBNumPerPatternType[EMERGENCY] * gc_BitNumPerRB)* factor);

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
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int patternIdx = gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];

			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				int patternType = DRAGetPatternType(patternIdx);

				//计算SINR，获取调制编码方式
				pair<int, int> &subCarrierIdxRange = DRAGetOccupiedSubCarrierRange(m_EventVec[info->eventId].message.messageType, patternIdx);
				g_FileTemp << "NonEmergencyPatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << ((patternType == 0) ? "Emergency" : (patternType == 1 ? "Period" : "Data")) << endl;

				//if ( !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
				if (m_VeUEAry[VeUEId].m_RRM->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx]) {//调制编码方式需要更新时
					m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx] = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId].m_RRM->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId].m_RRM->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId].m_RRM->m_isWTCached[patternIdx] = true;
				}
				double factor = get<1>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx])*get<2>(m_VeUEAry[VeUEId].m_RRM->m_WTInfo[patternIdx]);

				//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
				int maxEquivalentBitNum = (int)((double)(gc_DRA_RBNumPerPatternType[patternType] * gc_BitNumPerRB)* factor);
				
				//记录调度信息
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = m_EventVec[info->eventId].message.getCurrentPackageIdx();
				info->remainBitNum = m_EventVec[info->eventId].message.getRemainBitNum();

				//该编码方式下，该Pattern在一个TTI传输的实际的有效信息bit数量，并更新信息状态
				int realEquivalentBitNum = m_EventVec[info->eventId].message.transimit(maxEquivalentBitNum);

				//累计吞吐率
				m_TTIRSUThroughput[m_TTI][_RSU.m_GTAT->m_RSUId] += realEquivalentBitNum;

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx, "Transimit");
			}
		}
	}
	delete copyWTPoint;//getCopy是通过new创建的，因此这里释放资源
	copyWTPoint = nullptr;
}

void RRM_DRA::DRAWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		out << "    RSU[" << _RSU.m_GTAT->m_RSUId << "][TTI = " << m_TTI << "]" << endl;
		out << "    {" << endl;
		out << "    EMERGENCY:" << endl;
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			bool isAvaliable = _RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *(_RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    PERIOD:" << endl;
		for (int patternIdx = gc_DRAPatternTypePatternIdxInterval[PERIOD][0]; patternIdx <= gc_DRAPatternTypePatternIdxInterval[PERIOD][1]; patternIdx++) {
			int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
			bool isAvaliable = _RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][relativePatternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *(_RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[relativePatternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    DATA:" << endl;
		for (int patternIdx = gc_DRAPatternTypePatternIdxInterval[DATA][0]; patternIdx <= gc_DRAPatternTypePatternIdxInterval[DATA][1]; patternIdx++) {
			int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
			bool isAvaliable = _RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][relativePatternIdx];
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
			if (!isAvaliable) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *(_RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[relativePatternIdx].begin());
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    }" << endl;

	}
	out << "}" << endl;
	out << "\n\n" << endl;
}

void RRM_DRA::DRATransimitEnd() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() == 1) {
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();
				if (m_EventVec[info->eventId].message.isFinished()) {//已经传输完毕，将资源释放

					//设置传输成功标记
					m_EventVec[info->eventId].isSuccessded = true;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTAT->m_RSUId, -1, patternIdx, "Succeed");

					//记录TTI日志
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTAT->m_RSUId, -1, patternIdx);

					//释放调度信息对象的内存资源
					delete *lst.begin();
					*lst.begin() = nullptr;

					//释放Pattern资源
					_RSU.m_RRM_DRA->m_DRAEmergencyPatternIsAvailable[patternIdx] = true;
				}
				else {//尚未传输完毕
					_RSU.m_RRM_DRA->DRAPushToEmergencyScheduleInfoTable(patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			lst.clear();
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU.m_RRM_DRA->DRAGetClusterIdx(m_TTI);
		for (int patternIdx = gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			int relativePatternIdx= patternIdx- gc_DRAPatternNumPerPatternType[EMERGENCY];

			list<RSU::RRM_DRA::DRAScheduleInfo*> &lst = _RSU.m_RRM_DRA->m_DRATransimitScheduleInfoList[relativePatternIdx];
			if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
				RSU::RRM_DRA::DRAScheduleInfo *info = *lst.begin();
				if (m_EventVec[info->eventId].message.isFinished()) {//说明该数据已经传输完毕

					//设置传输成功标记
					m_EventVec[info->eventId].isSuccessded = true;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx, "Succeed");

					//记录TTI日志
					DRAWriteTTILogInfo(g_FileTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSU.m_GTAT->m_RSUId, clusterIdx, patternIdx);

					//释放调度信息对象的内存资源
					delete *lst.begin();
					*lst.begin() = nullptr;

					//释放Pattern资源
					_RSU.m_RRM_DRA->m_DRAPatternIsAvailable[clusterIdx][relativePatternIdx] = true;
				}
				else {//该数据仍未传完，将其压回m_DRAScheduleInfoTable
					_RSU.m_RRM_DRA->DRAPushToScheduleInfoTable(clusterIdx, patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			//处理完后，将该pattern上的数据清空（此时要不本身就是空，要不就是nullptr指针）
			lst.clear();
		}
	}
}


void RRM_DRA::DRAWriteTTILogInfo(std::ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx) {
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


void RRM_DRA::DRAWriteClusterPerformInfo(std::ofstream &out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;

	//打印VeUE信息
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUE &_VeUE = m_VeUEAry[VeUEId];
		out << _VeUE.m_RRM_DRA->toString(2) << endl;
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
		out << _RSU.m_RRM_DRA->toString(2) << endl;
	}
	out << "    }" << endl;

	out << "}\n\n";
}


int RRM_DRA::DRAGetMaxIndex(const std::vector<double>&clusterSize) {
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


int RRM_DRA::DRAGetPatternType(int patternIdx) {
	//patternIdx指所有事件类型的Pattern的绝对序号，从0开始编号，包括Emergency
	for (int patternType = 0; patternType < gc_DRAPatternTypeNum; patternType++) {
		if (patternIdx >= gc_DRAPatternTypePatternIdxInterval[patternType][0] && patternIdx <= gc_DRAPatternTypePatternIdxInterval[patternType][1])
			return patternType;
	}
	throw Exp("getPatternType");
}


pair<int, int> RRM_DRA::DRAGetOccupiedSubCarrierRange(MessageType messageType, int patternIdx) {

	pair<int, int> res;

	int offset = 0;
	for (int i = 0; i < messageType; i++) {
		offset += gc_DRA_RBNumPerPatternType[i] * gc_DRAPatternNumPerPatternType[i];
		patternIdx -= gc_DRAPatternNumPerPatternType[i];
	}
	res.first = offset + gc_DRA_RBNumPerPatternType[messageType] * patternIdx;
	res.second = offset + gc_DRA_RBNumPerPatternType[messageType] * (patternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
