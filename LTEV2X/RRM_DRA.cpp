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
#include"RRM_DRA.h"
#include"Exception.h"

using namespace std;
string sDRAScheduleInfo::toLogString(int n) {
	ostringstream ss;
	ss << "[ eventId = ";
	ss << left << setw(3) << eventId;
	ss << " , PatternIdx = " << left << setw(3) << patternIdx << " ] ";
	return ss.str();
}


std::string sDRAScheduleInfo::toScheduleString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");
	ostringstream ss;
	ss << indent << "{ ";
	ss << "[ eventId = " << left << setw(3) << eventId << " , VeUEId = " << left << setw(3) << VeUEId << " ]";
	ss << " : occupy Interval = { ";
	for (tuple<int, int> t : occupiedIntervalList) {
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] , ";
	}
	ss << "} }";
	return ss.str();
}


string VeUEAdapterDRA::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << m_HoldObj.m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << m_HoldObj.m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << m_HoldObj.m_ClusterIdx;
	ss << " , ScheduleInterval = [" << left << setw(3) << get<0>(m_ScheduleInterval) << "," << left << setw(3) << get<1>(m_ScheduleInterval) << "] }";
	return ss.str();
}


RSUAdapterDRA::RSUAdapterDRA(cRSU& _RSU) :m_HoldObj(_RSU) {

	//WRONG
	/*  EMERGENCY  */
	m_DRAEmergencyPatternIsAvailable = vector<bool>(gc_DRAEmergencyTotalPatternNum, true);
	m_DRAEmergencyScheduleInfoTable = vector<sDRAScheduleInfo*>(gc_DRAEmergencyTotalPatternNum);
	m_DRAEmergencyTransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(gc_DRAEmergencyTotalPatternNum);
	/*  EMERGENCY  */



	m_DRAPatternIsAvailable = vector<vector<bool>>(m_HoldObj.m_DRAClusterNum, vector<bool>(gc_DRATotalPatternNum, true));
	m_DRAScheduleInfoTable = vector<vector<sDRAScheduleInfo*>>(m_HoldObj.m_DRAClusterNum, vector<sDRAScheduleInfo*>(gc_DRATotalPatternNum, nullptr));
	m_DRATransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(gc_DRATotalPatternNum, list<sDRAScheduleInfo*>(0, nullptr));
}


int RSUAdapterDRA::DRAGetClusterIdx(int TTI) {
	int roundATTI = TTI%gc_DRA_NTTI; //将TTI映射到[0-gc_DRA_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < m_HoldObj.m_DRAClusterNum; clusterIdx++) 
		if (roundATTI <= get<1>(m_DRAClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


string RSUAdapterDRA::toString(int n) {
	string indent;
	for (int i = 0; i < n; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << m_HoldObj.m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < m_HoldObj.m_DRAClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx]) {
			if (cnt % 10 == 0)
				ss << indent << "            [ ";
			ss << left << setw(3) << RSUId << " , ";
			if (cnt % 10 == 9)
				ss << " ]" << endl;
			cnt++;
		}
		if (cnt != 0 && cnt % 10 != 0)
			ss << " ]" << endl;
		ss << indent << "        " << "}" << endl;
	}
	ss << indent << "    " << "}" << endl;

	//开始打印clusterInfo
	ss << indent << "    " << "clusterInfo :" << endl;
	ss << indent << "    " << "{" << endl;

	ss << indent << "        ";
	for (const tuple<int, int, int>&t : m_DRAClusterTDRInfo)
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << endl;
	ss << indent << "    " << "}" << endl;


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}




RRM_DRA::RRM_DRA(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, eDRAMode m_DRAMode) :
	RRM_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList), m_DRAMode(m_DRAMode) {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAdapterVec.push_back(VeUEAdapterDRA(m_VeUEAry[VeUEId]));
	}
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAdapterVec.push_back(RSUAdapterDRA(m_RSUAry[RSUId]));
	}
}


void RRM_DRA::schedule() {
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//资源分配信息清空:包括每个RSU内的接入链表等
	DRAInformationClean();

	//  WRONG
	//根据簇大小进行时域资源的划分
	DRAGroupSizeBasedTDM(clusterFlag);
	//  WRONG

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
	//写入调度信息
	DRAWriteScheduleInfo(g_OutDRAScheduleInfo);

	//统计时延信息
	DRADelaystatistics();

	//帧听冲突
	DRAConflictListener();
}


void RRM_DRA::DRAInformationClean() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];
		_RSUAdapterDRA.m_DRAAdmitEventIdList.clear();
		_RSUAdapterDRA.m_DRAEmergencyAdmitEventIdList.clear();
	}
}


void RRM_DRA::DRAGroupSizeBasedTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];
		
		//特殊情况，当该RSU内无一辆车时
		if (_RSUAdapterDRA.m_HoldObj.m_VeUEIdList.size() == 0) {
			/*-----------------------ATTENTION-----------------------
			* 若赋值为(0,-1,0)会导致获取当前簇编号失败，导致其他地方需要讨论
			* 因此直接给每个簇都赋值为整个区间，反正也没有任何作用，免得其他部分讨论
			*------------------------ATTENTION-----------------------*/
			_RSUAdapterDRA.m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSUAdapterDRA.m_HoldObj.m_DRAClusterNum, tuple<int, int, int>(0, gc_DRA_NTTI - 1, gc_DRA_NTTI));
			continue;
		}

		//初始化
		_RSUAdapterDRA.m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(_RSUAdapterDRA.m_HoldObj.m_DRAClusterNum, tuple<int, int, int>(0, 0, 0));

		//计算每个TTI时隙对应的VeUE数目(double)，!!!浮点数!!！
		double VeUESizePerTTI = static_cast<double>(_RSUAdapterDRA.m_HoldObj.m_VeUEIdList.size()) / static_cast<double>(gc_DRA_NTTI);

		//clusterSize存储每个簇的VeUE数目(double)，!!!浮点数!!！
		std::vector<double> clusterSize(_RSUAdapterDRA.m_HoldObj.m_DRAClusterNum, 0);

		//初始化clusterSize
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++)
			clusterSize[clusterIdx] = static_cast<double>(_RSUAdapterDRA.m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx].size());

		//首先给至少有一辆车的簇分配一份TTI
		int basicNTTI = 0;
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++) {
			//如果该簇内至少有一辆VeUE，直接分配给一个单位的时域资源
			if (_RSUAdapterDRA.m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx].size() != 0) {
				get<2>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) = 1;
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
			get<2>(_RSUAdapterDRA.m_DRAClusterTDRInfo[dex])++;
			remainNTTI--;
			clusterSize[dex] -= VeUESizePerTTI;
		}

		//开始生成区间范围，闭区间
		get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[0]) = 0;
		get<1>(_RSUAdapterDRA.m_DRAClusterTDRInfo[0]) = get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[0]) + get<2>(_RSUAdapterDRA.m_DRAClusterTDRInfo[0]) - 1;
		for (int clusterIdx = 1; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++) {
			get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) = get<1>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx - 1]) + 1;
			get<1>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) = get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) + get<2>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]) - 1;
		}


		//将调度区间写入该RSU内的每一个车辆
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSUAdapterDRA.m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx])
				m_VeUEAdapterVec[VeUEId].m_ScheduleInterval = tuple<int, int>(get<0>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]), get<1>(_RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]));
		}
	}

	writeClusterPerformInfo(g_OutClasterPerformInfo);
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
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);//当前可传输数据的簇编号
		for (int eventId : m_EventTTIList[m_TTI]) {
			sEvent event = m_EventVec[eventId];
			
			int VeUEId = event.VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//当前事件对应的VeUE不在当前RSU中，跳过即可
				continue;
			}
			else {//当前事件对应的VeUE在当前RSU中
				if (m_EventVec[eventId].message.messageType == EMERGENCY) {//若是紧急性事件
					/*  EMERGENCY  */
					//将该事件压入紧急事件等待链表
					_RSUAdapterDRA.DRAPushToEmergencyWaitEventIdList(eventId);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
					/*  EMERGENCY  */
				}
				else {//非紧急性事件
				    //将该事件压入等待链表
					_RSUAdapterDRA.DRAPushToWaitEventIdList(eventId);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, EVENT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, EVENT_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
				}
			}
		}
	}
}


void RRM_DRA::DRAProcessScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			if (_RSUAdapterDRA.m_DRAEmergencyScheduleInfoTable[patternIdx] == nullptr) {//当前EmergencyPattern无事件传输
				continue;
			}
			else {
				int eventId = _RSUAdapterDRA.m_DRAEmergencyScheduleInfoTable[patternIdx]->eventId;
				int VeUEId = m_EventVec[eventId].VeUEId;
				if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
					_RSUAdapterDRA.DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);

					//并释放该调度信息的资源
					delete _RSUAdapterDRA.m_DRAEmergencyScheduleInfoTable[patternIdx];
					deleteCount++;
					_RSUAdapterDRA.m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;

					//释放Pattern资源
					_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] = true;

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);
				}
				else {//该车辆仍在当前RSU内
					  //doing nothing 
					continue;
				}
			}
		}
		/*  EMERGENCY  */

		//开始处理 m_DRAScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
				if (_RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx] == nullptr) {//当前Pattern块无事件在传输
					continue;
				}
				else {
					int eventId = _RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx]->eventId;
					int VeUEId = m_EventVec[eventId].VeUEId;
					if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
						_RSUAdapterDRA.DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);

						//并释放该调度信息的资源
						delete _RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx];
						deleteCount++;
						_RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

						//释放Pattern资源
						_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

						//更新该事件的日志
						m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_SWITCH, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

						//记录TTI日志
						DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);
					}
					else {
						if (_RSUAdapterDRA.DRAGetClusterIdxOfVeUE(VeUEId) != clusterIdx) {//RSU内部发生了簇切换，将其从调度表中取出，压入等待链表
							_RSUAdapterDRA.DRAPushToWaitEventIdList(eventId);

							//并释放该调度信息的资源
							delete _RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx];
							deleteCount++;
							_RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

							//释放Pattern资源
							_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

							//更新该事件的日志
							m_EventVec[eventId].addEventLog(m_TTI, SCHEDULETABLE_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

							//记录TTI日志
							DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);
						}
						else {//既没有发生RSU切换，也没有发生RSU内簇切换，什么也不做
							  //doing nothing
							continue;
						}
					}
				}
			}
		}
	}
}


void RRM_DRA::DRAProcessWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		//开始处理m_DRAEmergencyWaitEventIdList
		list<int>::iterator it = _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList.begin();
		while (it != _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//该VeUE已经不在该RSU范围内
				_RSUAdapterDRA.DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);//将其添加到System级别的RSU切换链表中
				it = _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList.erase(it);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
			}
			else {//仍然处于当前RSU范围内
				it++;
				continue; //继续留在当前RSU的Emergency等待链表
			}
		}
		/*  EMERGENCY  */


		//开始处理 m_DRAWaitEventIdList
		it = _RSUAdapterDRA.m_DRAWaitEventIdList.begin();
		while (it != _RSUAdapterDRA.m_DRAWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//该VeUE已经不在该RSU范围内
				_RSUAdapterDRA.DRAPushToSwitchEventIdList(eventId, m_DRASwitchEventIdList);//将其添加到System级别的RSU切换链表中
				it = _RSUAdapterDRA.m_DRAWaitEventIdList.erase(it);//将其从等待链表中删除

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_SWITCH, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_SWITCH, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
			}
			else {//仍然处于当前RSU范围内
				it++;
				continue; //继续留在当前RSU的等待链表
			}
		}
	}
}


void RRM_DRA::DRAProcessSwitchListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		list<int>::iterator it = m_DRASwitchEventIdList.begin();
		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);//当前可传输数据的簇编号
		while (it !=m_DRASwitchEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_RSUId != _RSUAdapterDRA.m_HoldObj.m_RSUId) {//该切换链表中的事件对应的VeUE，不属于当前簇，跳过即可
				it++;
				continue;
			}
			else {//该切换链表中的VeUE，属于当RSU
				  /*  EMERGENCY  */
				if (m_EventVec[eventId].message.messageType == EMERGENCY) {//属于紧急事件
					//转入等待链表
					_RSUAdapterDRA.DRAPushToEmergencyWaitEventIdList(eventId);

					//从Switch表中将其删除
					it = m_DRASwitchEventIdList.erase(it);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
				}
				/*  EMERGENCY  */

				else {//非紧急事件
					//转入等待链表
					_RSUAdapterDRA.DRAPushToWaitEventIdList(eventId);

					//从Switch表中将其删除
					it = m_DRASwitchEventIdList.erase(it);

					//更新该事件的日志
					m_EventVec[eventId].addEventLog(m_TTI, SWITCH_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SWITCH_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
				}
			}
		}
	}
}


void RRM_DRA::DRAProcessWaitEventIdList() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		for (int eventId : _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList) {
			_RSUAdapterDRA.DRAPushToEmergencyAdmitEventIdList(eventId);//将事件压入接入链表

		    //更新该事件的日志
			m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

			//记录TTI日志
			DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
		}

		//由于会全部压入Emergency接入链表，因此直接清空就行了，不需要一个个删除
		_RSUAdapterDRA.m_DRAEmergencyWaitEventIdList.clear();
		/*  EMERGENCY  */


		//开始处理 m_DRAWaitEventIdList
		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);//处于调度中的簇编号
		list<int>::iterator it = _RSUAdapterDRA.m_DRAWaitEventIdList.begin();
		while (it != _RSUAdapterDRA.m_DRAWaitEventIdList.end()) {
			int eventId = *it;
			int VeUEId = m_EventVec[eventId].VeUEId;
			if (m_VeUEAdapterVec[VeUEId].m_HoldObj.m_ClusterIdx == clusterIdx) {//该事件当前可以进行调度
				_RSUAdapterDRA.DRAPushToAdmitEventIdList(eventId);//添加到RSU级别的接纳链表中
				it = _RSUAdapterDRA.m_DRAWaitEventIdList.erase(it);//将其从等待链表中删除

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, WAIT_TO_ADMIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, WAIT_TO_ADMIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
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
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		//WRONG
		//vector<int> curAvaliableEmergencyPatternIdx(gc_DRAPatternTypeNum);//当前可用的EmergencyPattern编号
		vector<int> curAvaliableEmergencyPatternIdx;//当前可用的EmergencyPattern编号

		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			if (_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx]) {
				curAvaliableEmergencyPatternIdx.push_back(patternIdx);
			}
		}

		for (int eventId : _RSUAdapterDRA.m_DRAEmergencyAdmitEventIdList) {
			int VeUEId = m_EventVec[eventId].VeUEId;

			//为当前用户在可用的EmergencyPattern块中随机选择一个，每个用户自行随机选择可用EmergencyPattern块
			int patternIdx = m_VeUEAdapterVec[VeUEId].RBEmergencySelectBasedOnP2(curAvaliableEmergencyPatternIdx);

			if (patternIdx == -1) {//无对应Pattern类型的pattern资源可用
				_RSUAdapterDRA.DRAPushToEmergencyWaitEventIdList(eventId);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				continue;
			}

			//将资源标记为占用
			_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] = false;

			//将调度信息压入m_DRAEmergencyTransimitEventIdList中
			list<tuple<int, int>> scheduleIntervalList = DRABuildEmergencyScheduleInterval(m_TTI, m_EventVec[eventId]);
			_RSUAdapterDRA.DRAPushToEmergencyTransmitScheduleInfoList(new sDRAScheduleInfo(eventId, VeUEId, _RSUAdapterDRA.m_HoldObj.m_RSUId, patternIdx, scheduleIntervalList), patternIdx);
			newCount++;
		}
		_RSUAdapterDRA.DRAPullFromEmergencyScheduleInfoTable();
		/*  EMERGENCY  */


		//开始处理非Emergency的事件
		int roundATTI = m_TTI%gc_DRA_NTTI;//将TTI映射到[0-gc_DRA_NTTI)的范围

		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);

		/*
		* 当前TTI可用的Pattern块编号
		* 下标代表的Pattern种类的编号
		* 每个内层vector代表该种类Pattern可用的Pattern编号
		*/
		vector<vector<int>> curAvaliablePatternIdx(gc_DRAPatternTypeNum);

		for (int patternTypeIdx = 0; patternTypeIdx < gc_DRAPatternTypeNum; patternTypeIdx++) {
			for (int patternIdx : gc_DRAPatternIdxTable[patternTypeIdx]) {
				if (_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		for (int eventId : _RSUAdapterDRA.m_DRAAdmitEventIdList) {//遍历该簇内接纳链表中的事件

			int VeUEId = m_EventVec[eventId].VeUEId;

			//为当前用户在可用的对应其事件类型的Pattern块中随机选择一个，每个用户自行随机选择可用Pattern块
			int patternIdx = m_VeUEAdapterVec[VeUEId].RBSelectBasedOnP2(curAvaliablePatternIdx, m_EventVec[eventId].message.messageType);

			if (patternIdx == -1) {//该用户传输的信息类型没有pattern剩余了
				_RSUAdapterDRA.DRAPushToWaitEventIdList(eventId);

				//更新该事件的日志
				m_EventVec[eventId].addEventLog(m_TTI, ADMIT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, ADMIT_TO_WAIT, eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);
				continue;
			}

			//将资源标记为占用
			_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = false;

			//将调度信息压入m_DRATransimitEventIdList中
			list<tuple<int, int>> scheduleIntervalList = DRABuildScheduleIntervalList(m_TTI, m_EventVec[eventId], _RSUAdapterDRA.m_DRAClusterTDRInfo[clusterIdx]);
			_RSUAdapterDRA.DRAPushToTransmitScheduleInfoList(new sDRAScheduleInfo(eventId, VeUEId, _RSUAdapterDRA.m_HoldObj.m_RSUId, patternIdx, scheduleIntervalList), patternIdx);
			newCount++;
		}

		//将调度表中当前可以继续传输的用户压入传输链表中
		_RSUAdapterDRA.DRAPullFromScheduleInfoTable(m_TTI);

	}
}


void RRM_DRA::DRAWriteScheduleInfo(std::ofstream& out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];
		
		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);
		out << "    RSU[" << _RSUAdapterDRA.m_HoldObj.m_RSUId << "] :" << endl;
		out << "    {" << endl;
		out << "    EMERGENCY:" << endl;
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] ? "Available" : "Unavailable") << endl;
			for (sDRAScheduleInfo* info : _RSUAdapterDRA.m_DRAEmergencyTransimitScheduleInfoList[patternIdx]) {
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    PERIOD:" << endl;
		for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[PERIOD]; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] ? "Available" : "Unavailable") << endl;
			for (sDRAScheduleInfo* info : _RSUAdapterDRA.m_DRATransimitScheduleInfoList[patternIdx]) {
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    DATA:" << endl;
		for (int patternIdx = gc_DRAPatternNumPerPatternType[PERIOD]; patternIdx < gc_DRAPatternNumPerPatternType[PERIOD] + gc_DRAPatternNumPerPatternType[DATA]; patternIdx++) {
			out << "        Pattern[ " << left << setw(3) << patternIdx << "] : " << (_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] ? "Available" : "Unavailable") << endl;
			for (sDRAScheduleInfo* info : _RSUAdapterDRA.m_DRATransimitScheduleInfoList[patternIdx]) {
				out << info->toScheduleString(3) << endl;
			}
		}
		out << "    }" << endl;

	}
	out << "}" << endl;
	out << "\n\n" << endl;
}


void RRM_DRA::DRADelaystatistics() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*
		* 该函数应该在DRASelectBasedOnP..之后调用
		* 此时事件处于等待链表、传输链表、或者调度表中（非当前簇）
		*/

		//处理等待链表
		/*  EMERGENCY  */
		for (int eventId : _RSUAdapterDRA.m_DRAEmergencyWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;
		/*  EMERGENCY  */
		for (int eventId : _RSUAdapterDRA.m_DRAWaitEventIdList)
			m_EventVec[eventId].queuingDelay++;

		//处理此刻正在将要传输的传输链表
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++)
			for (sDRAScheduleInfo* &p : _RSUAdapterDRA.m_DRAEmergencyTransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			for (sDRAScheduleInfo* &p : _RSUAdapterDRA.m_DRATransimitScheduleInfoList[patternIdx])
				m_EventVec[p->eventId].sendDelay++;
		}

		//处理此刻位于调度表中，但不属于当前簇的事件
		int curClusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);
		for (int clusterIdx = 0; clusterIdx < _RSUAdapterDRA.m_HoldObj.m_DRAClusterNum; clusterIdx++) {
			if (clusterIdx == curClusterIdx) continue;
			for (sDRAScheduleInfo *p : _RSUAdapterDRA.m_DRAScheduleInfoTable[clusterIdx]) {
				if (p == nullptr) continue;
				m_EventVec[p->eventId].queuingDelay++;
			}
		}
	}
}

void RRM_DRA::DRAConflictListener() {
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSUAdapterDRA &_RSUAdapterDRA = m_RSUAdapterVec[RSUId];

		/*  EMERGENCY  */
		for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
			list<sDRAScheduleInfo*> &lst = _RSUAdapterDRA.m_DRAEmergencyTransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
				for (sDRAScheduleInfo* &info : lst) {
					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

					//首先将事件压入等待列表
					_RSUAdapterDRA.DRAPushToEmergencyWaitEventIdList(info->eventId);

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
					deleteCount++;
				}

				//释放Pattern资源
				_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] = true;
			}
			else if (lst.size() == 1) {
				sDRAScheduleInfo *info = *lst.begin();

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

				//维护占用区间
				tuple<int, int> &scheduleInterval = *(info->occupiedIntervalList.begin());

				++get<0>(scheduleInterval);
				if (get<0>(scheduleInterval) > get<1>(scheduleInterval)) {//已经传输完毕，将资源释放

					//设置传输成功标记
					m_EventVec[info->eventId].isSuccessded = true;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, patternIdx);

					//释放调度信息对象的内存资源
					delete *lst.begin();
					deleteCount++;

					//释放Pattern资源
					_RSUAdapterDRA.m_DRAEmergencyPatternIsAvailable[patternIdx] = true;
				}
				else {//尚未传输完毕
					_RSUAdapterDRA.DRAPushToEmergencyScheduleInfoTable(patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			else {//没有紧急事件在该pattern上传输
				  //doting nothing
				continue;
			}
			lst.clear();
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSUAdapterDRA.DRAGetClusterIdx(m_TTI);
		for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
			list<sDRAScheduleInfo*> &lst = _RSUAdapterDRA.m_DRATransimitScheduleInfoList[patternIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
				for (sDRAScheduleInfo* &info : lst) {
					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

					//首先将事件压入等待列表
					_RSUAdapterDRA.DRAPushToWaitEventIdList(info->eventId);

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, TRANSIMIT_TO_WAIT, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, -1, -1);

					//释放调度信息对象的内存资源
					delete info;
					info = nullptr;
					deleteCount++;
				}
				//释放Pattern资源
				_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

			}
			else if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）

				sDRAScheduleInfo *info = *lst.begin();

				//更新该事件的日志
				m_EventVec[info->eventId].addEventLog(m_TTI, IS_TRANSIMITTING, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

				//维护占用区间
				list<tuple<int, int>> &OIList = info->occupiedIntervalList;

				tuple<int, int> &firstInterval = *OIList.begin();
				get<0>(firstInterval)++;
				if (get<0>(firstInterval) > get<1>(firstInterval))
					OIList.erase(OIList.begin());//删除第一个区间

				if (OIList.size() == 0) {//说明该数据已经传输完毕

					//设置传输成功标记
					m_EventVec[info->eventId].isSuccessded = true;

					//更新该事件的日志
					m_EventVec[info->eventId].addEventLog(m_TTI, SUCCEED, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, m_TTI, SUCCEED, info->eventId, _RSUAdapterDRA.m_HoldObj.m_RSUId, clusterIdx, patternIdx);

					//释放调度信息对象的内存资源
					delete *lst.begin();
					deleteCount++;

					//释放Pattern资源
					_RSUAdapterDRA.m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;
				}
				else {//该数据仍未传完，将其压回m_DRAScheduleInfoTable
					_RSUAdapterDRA.DRAPushToScheduleInfoTable(clusterIdx, patternIdx, *lst.begin());
					*lst.begin() = nullptr;
				}
			}
			else {//没有事件在该pattern上传输
				  //do nothing
			}
			//处理完后，将该pattern上的数据清空（此时要不本身就是空，要不就是nullptr指针）
			lst.clear();
		}
	}
}


void RRM_DRA::DRAWriteTTILogInfo(std::ofstream& out, int TTI, eEventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx) {
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


void RRM_DRA::writeClusterPerformInfo(std::ofstream &out) {
	out << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	out << "{" << endl;

	//打印VeUE信息
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		VeUEAdapterDRA &_VeUE = m_VeUEAdapterVec[VeUEId];
		out << _VeUE.toString(2) << endl;
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
		RSUAdapterDRA &_RSU = m_RSUAdapterVec[RSUId];
		out << _RSU.toString(2) << endl;
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


list<tuple<int, int>> RRM_DRA::DRABuildScheduleIntervalList(int TTI, const sEvent& event, std::tuple<int, int, int>ClasterTTI) {
	list<tuple<int, int>> scheduleIntervalList;
	int eventId = event.eventId;
	int occupiedTTI = event.message.bitNum / gc_BitNumPerRB / gc_DRA_FBNumPerPatternType[event.message.messageType];
	int begin = std::get<0>(ClasterTTI),
		end = std::get<1>(ClasterTTI),
		len = std::get<2>(ClasterTTI);

	//当前时刻的相对TTI
	int roundTTI = TTI%gc_DRA_NTTI;

	//该RSU下一轮调度的起始TTI（第一个簇的开始时刻）
	int nextTurnBeginTTI = TTI - roundTTI + gc_DRA_NTTI;

	//当前一轮调度中剩余可用的时隙数量
	int remainTTI = end - roundTTI + 1;

	//超出当前一轮调度可用时隙数量的部分
	int overTTI = occupiedTTI - remainTTI;


	if (overTTI <= 0) scheduleIntervalList.push_back(std::tuple<int, int>(TTI, TTI + occupiedTTI - 1));
	else {
		scheduleIntervalList.push_back(std::tuple<int, int>(TTI, TTI + remainTTI - 1));
		int n = overTTI / len;
		for (int i = 0; i < n; i++) scheduleIntervalList.push_back(std::tuple<int, int>(nextTurnBeginTTI + i*gc_DRA_NTTI + begin, nextTurnBeginTTI + begin + len - 1 + i*gc_DRA_NTTI));
		if (overTTI%len != 0) scheduleIntervalList.push_back(std::tuple<int, int>(nextTurnBeginTTI + n*gc_DRA_NTTI + begin, nextTurnBeginTTI + begin + n*gc_DRA_NTTI + overTTI%len - 1));
	}
	return scheduleIntervalList;
}


std::list<std::tuple<int, int>> RRM_DRA::DRABuildEmergencyScheduleInterval(int TTI, const sEvent& event) {
	std::list<std::tuple<int, int>> scheduleIntervalList;
	std::tuple<int, int> scheduleInterval;
	int eventId = event.eventId;
	int occupiedTTI = event.message.bitNum / gc_BitNumPerRB / gc_DRAEmergencyFBNumPerPattern;
	get<0>(scheduleInterval) = TTI;
	get<1>(scheduleInterval) = TTI + occupiedTTI - 1;
	scheduleIntervalList.push_back(scheduleInterval);
	return scheduleIntervalList;
}



