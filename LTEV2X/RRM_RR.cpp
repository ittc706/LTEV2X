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
#include"Function.h"
#include"System.h"

using namespace std;


RRM_RR_VeUE::RRM_RR_VeUE() :RRM_VeUE(ns_RRM_RR::gc_TotalPatternNum) {}


std::string RRM_RR_VeUE::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_ClusterIdx << " }";
	return ss.str();
}


RRM_RR_RSU::RRM_RR_RSU() {}


void RRM_RR_RSU::initialize() {
	m_AccessEventIdList = vector<list<int>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_WaitEventIdList = vector<list<int>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_TransimitScheduleInfoTable = vector<vector<ScheduleInfo*>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<ScheduleInfo*>(ns_RRM_RR::gc_TotalPatternNum, nullptr));
}

std::string RRM_RR_RSU::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << getSystemPoint()->getGTTPoint()->m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx]) {
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


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}


RRM_RR::RRM_RR(System* t_Context) :
	RRM(t_Context) {
	m_ThreadNum = t_Context->m_Config.threadNum;
	m_InterferenceVec = vector<vector<list<int>>>(getContext()->m_Config.VeUENum, vector<list<int>>(ns_RRM_RR::gc_TotalPatternNum));

	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = getContext()->m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = getContext()->m_Config.RSUNum - 1;//修正最后一个边界
}


void RRM_RR::initialize() {
	//初始化VeUE的该模块参数部分
	m_VeUEAry = new RRM_VeUE*[getContext()->m_Config.VeUENum];
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId] = new RRM_RR_VeUE();
	}

	//初始化RSU的该模块参数部分
	m_RSUAry = new RRM_RSU*[getContext()->m_Config.RSUNum];
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId] = new RRM_RR_RSU();
	}
}


void RRM_RR::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId]->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();
		m_VeUEAry[VeUEId]->m_PreSINR.assign(ns_RRM_RR::gc_TotalPatternNum, (numeric_limits<double>::min)());
	}
}


void RRM_RR::schedule() {
	bool isLocationUpdate = getContext()->m_TTI  % getContext()->m_Config.locationUpdateNTTI == 0;

	//写入地理位置信息
	writeClusterPerformInfo(isLocationUpdate, g_FileClasterPerformInfo);

	//调度前清理工作
	informationClean();

	//建立接纳链表
	updateAccessEventIdList(isLocationUpdate);

	//开始本次调度
	roundRobin();

	//统计时延信息
	delaystatistics();

	//统计干扰信息
	transimitPreparation();

	//模拟传输开始，更新调度信息，累计吞吐量
	transimitStart();

	//写调度日志
	writeScheduleInfo(g_FileScheduleInfo);

	//传输结束
	transimitEnd();
}



void RRM_RR::informationClean() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			_RSU->getRRPoint()->m_AccessEventIdList[clusterIdx].clear();
		}
	}
}


void RRM_RR::updateAccessEventIdList(bool t_ClusterFlag) {
	//首先，处理System级别的事件触发链表
	processEventList();

	//其次，如果当前TTI进行了分簇，需要处理调度表
	if (t_ClusterFlag) {
		//处理RSU级别的等待链表
		processWaitEventIdListWhenLocationUpdate();

		//处理System级别的切换链表
		processSwitchListWhenLocationUpdate();
	}

	//最后，由等待表生成接入表
	processWaitEventIdList();
}

void RRM_RR::processEventList() {
	for (int eventId : getContext()->m_EventTTIList[getContext()->m_TTI]) {
		Event event = getContext()->m_EventVec[eventId];
		int VeUEId = event.getVeUEId();
		int RSUId = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId;
		int clusterIdx = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx;
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		//将事件压入等待链表
		bool isEmergency = event.getMessageType() == EMERGENCY;
		_RSU->getRRPoint()->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//更新日志
		getContext()->m_EventVec[eventId].addEventLog(getContext()->m_TTI, EVENT_TO_WAIT, -1, -1, -1, RSUId, clusterIdx, -1, "Trigger");
		writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, EVENT_TO_WAIT, eventId, -1, -1, -1, RSUId, clusterIdx, -1, "Trigger");
	}
}


void RRM_RR::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {

			list<int>::iterator it = _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx].end()) {
				int eventId = *it;
				int VeUEId = getContext()->m_EventVec[eventId].getVeUEId();
				bool isEmergency = getContext()->m_EventVec[eventId].getMessageType() == EMERGENCY;
				//该VeUE已经不在该RSU范围内
				if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId != _RSU->getSystemPoint()->getGTTPoint()->m_RSUId) {

					//将其添加到System级别的RSU切换链表中
					_RSU->getRRPoint()->pushToSwitchEventIdList(eventId, m_SwitchEventIdList);

					//将其从等待链表中删除
					it = _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx].erase(it);

					//将剩余待传bit重置
					getContext()->m_EventVec[eventId].reset();

					//更新日志
					getContext()->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_SWITCH, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, WAIT_TO_SWITCH, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				//仍然处于当前RSU范围内，但是位于不同的簇
				else if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx != clusterIdx) {
					//将其转移到当前RSU的其他簇内
					_RSU->getRRPoint()->pushToWaitEventIdList(isEmergency, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, eventId);

					//将其从等待链表中删除
					it = _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx].erase(it);

					//更新日志
					getContext()->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, WAIT_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
				}
				else {
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
		int VeUEId = getContext()->m_EventVec[eventId].getVeUEId();
		int clusterIdx = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx;
		int RSUId = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId;
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		bool isEmergency = getContext()->m_EventVec[eventId].getMessageType() == EMERGENCY;
		_RSU->getRRPoint()->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//从Switch链表中删除
		it = m_SwitchEventIdList.erase(it);

		//更新日志
		getContext()->m_EventVec[eventId].addEventLog(getContext()->m_TTI, SWITCH_TO_WAIT, -1, -1, -1, RSUId, clusterIdx, -1, "LocationUpdate");
		writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, SWITCH_TO_WAIT, eventId, -1, -1, -1, RSUId, clusterIdx, -1, "LocationUpdate");

	}
}


void RRM_RR::processWaitEventIdList() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			int patternIdx = 0;
			list<int>::iterator it = _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx].begin();
			while (it != _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx].end() && patternIdx < ns_RRM_RR::gc_TotalPatternNum) {
				int eventId = *it;

				//更新日志
				getContext()->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_ACCESS, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");
				writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, WAIT_TO_ACCESS, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");

				_RSU->getRRPoint()->pushToAccessEventIdList(clusterIdx, eventId);
				it = _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx].erase(it);
				++patternIdx;
			}

			//当等待表中还有剩余，这些事件将等到下个TTI再尝试进行接入
			while (it != _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx].end()) {
				int eventId = *it;

				//更新日志
				getContext()->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");
				writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, WAIT_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");

				++it;
			}
		}
	}
}


void RRM_RR::roundRobin() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			int patternIdx = 0;
			for (int eventId : _RSU->getRRPoint()->m_AccessEventIdList[clusterIdx]) {
				int VeUEId = getContext()->m_EventVec[eventId].getVeUEId();
				_RSU->getRRPoint()->pushToTransimitScheduleInfoTable(new RRM_RSU::ScheduleInfo(eventId, VeUEId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx));
				++patternIdx;
			}
		}
	}
}


void RRM_RR::delaystatistics() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			//处理等待链表
			for (int eventId : _RSU->getRRPoint()->m_WaitEventIdList[clusterIdx])
				getContext()->m_EventVec[eventId].increaseQueueDelay();

			//处理此刻正在将要传输的调度表
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
				if (_RSU->getRRPoint()->m_TransimitScheduleInfoTable[clusterIdx][patternIdx] == nullptr)continue;
				getContext()->m_EventVec[_RSU->getRRPoint()->m_TransimitScheduleInfoTable[clusterIdx][patternIdx]->eventId].increaseSendDelay();
			}
		}
	}
}


void RRM_RR::transimitPreparation() {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++)
		for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++)
			m_InterferenceVec[VeUEId][patternIdx].clear();


	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
				RRM_RSU::ScheduleInfo *&curInfo = _RSU->getRRPoint()->m_TransimitScheduleInfoTable[clusterIdx][patternIdx];
				if (curInfo == nullptr) continue;
				int curVeUEId = curInfo->VeUEId;
				for (int otherClusterIdx = 0; otherClusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; otherClusterIdx++) {
					if (otherClusterIdx == clusterIdx)continue;
					RRM_RSU::ScheduleInfo *&otherInfo = _RSU->getRRPoint()->m_TransimitScheduleInfoTable[otherClusterIdx][patternIdx];
					if (otherInfo == nullptr) continue;
					int otherVeUEId = otherInfo->VeUEId;
					m_InterferenceVec[curVeUEId][patternIdx].push_back(otherVeUEId);
				}
			}
		}
	}


	//更新每辆车的干扰车辆列表	
	for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
		for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
			list<int>& interList = m_InterferenceVec[VeUEId][patternIdx];

			m_VeUEAry[VeUEId]->m_InterferenceVeUENum[patternIdx] = (int)interList.size();//写入干扰数目

			m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx].assign(interList.begin(), interList.end());//写入干扰车辆ID

			if (m_VeUEAry[VeUEId]->m_InterferenceVeUENum[patternIdx]>0) {
				g_FileTemp << "VeUEId: " << VeUEId << " [";
				for (auto c : m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx])
					g_FileTemp << c << ", ";
				g_FileTemp << " ]" << endl;
			}
		}
	}

	//请求地理拓扑单元计算干扰响应矩阵
	long double start = clock();
	getContext()->m_GTTPoint->calculateInterference(m_InterferenceVec);
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


void RRM_RR::transimitStartThread(int t_FromRSUId, int t_ToRSUId) {
	WT* copyWTPoint = getContext()->m_WTPoint->getCopy();//由于每个线程的该模块会有不同的状态且无法共享，因此这里拷贝该模块用于本次计算
	for (int RSUId = t_FromRSUId; RSUId <= t_ToRSUId; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
				RRM_RSU::ScheduleInfo *&info = _RSU->getRRPoint()->m_TransimitScheduleInfoTable[clusterIdx][patternIdx];

				if (info == nullptr) continue;
				int VeUEId = info->VeUEId;

				//计算SINR，获取调制编码方式
				pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(patternIdx);
				g_FileTemp << "PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

				double factor = m_VeUEAry[VeUEId]->m_ModulationType * m_VeUEAry[VeUEId]->m_CodeRate;

				//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
				int maxEquivalentBitNum = (int)((double)(ns_RRM_RR::gc_RBNumPerPattern * gc_BitNumPerRB)* factor);

				//计算SINR
				double curSINR = 0;
				if (m_VeUEAry[VeUEId]->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId]->isAlreadyCalculateSINR(patternIdx)) {//调制编码方式需要更新时
					curSINR = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId]->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId]->m_PreSINR[patternIdx] = curSINR;
				}
				else
					curSINR = m_VeUEAry[VeUEId]->m_PreSINR[patternIdx];

				//记录调度信息
				double tmpDistance = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_Distance[RSUId];
				if (curSINR < gc_CriticalPoint) {
					//记录丢包			
					getContext()->m_EventVec[info->eventId].packetLoss(tmpDistance);
				}
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = getContext()->m_EventVec[info->eventId].getCurrentPackageIdx();
				info->remainBitNum = getContext()->m_EventVec[info->eventId].getRemainBitNum();

				//该编码方式下，该Pattern在一个TTI传输的实际的有效信息bit数量，并更新信息状态
				int realEquivalentBitNum = getContext()->m_EventVec[info->eventId].transimit(maxEquivalentBitNum, tmpDistance);

				//累计吞吐率
				getContext()->m_TTIRSUThroughput[getContext()->m_TTI][_RSU->getSystemPoint()->getGTTPoint()->m_RSUId] += realEquivalentBitNum;

				//更新日志
				getContext()->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMITTING, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
				writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, TRANSIMITTING, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
			}
		}
	}
	copyWTPoint->freeCopy();//getCopy是通过new创建的，因此这里释放资源
}


void RRM_RR::writeScheduleInfo(ofstream& t_File) {
	t_File << "[ TTI = " << left << setw(3) << getContext()->m_TTI << "]" << endl;
	t_File << "{" << endl;
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {

		RRM_RSU *_RSU = m_RSUAry[RSUId];
		t_File << "    RSU[" << _RSU->getSystemPoint()->getGTTPoint()->m_RSUId << "] :" << endl;
		t_File << "    {" << endl;
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			t_File << "        Cluster[" << clusterIdx << "] :" << endl;
			t_File << "        {" << endl;
			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {
				t_File << "            Pattern[ " << left << setw(3) << patternIdx << "] : " << endl;
				RRM_RSU::ScheduleInfo* &info = _RSU->getRRPoint()->m_TransimitScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;
				t_File << info->toScheduleString(3) << endl;
			}
			t_File << "        }" << endl;
		}
		t_File << "    }" << endl;
	}
	t_File << "}" << endl;
	t_File << "\n\n" << endl;
}


void RRM_RR::writeTTILogInfo(ofstream& t_File, int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description) {
	stringstream ss;
	switch (t_EventLogType) {
	case TRANSIMITTING:
		ss << " - Transimiting  At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "]";
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


void RRM_RR::transimitEnd() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {

			for (int patternIdx = 0; patternIdx < ns_RRM_RR::gc_TotalPatternNum; patternIdx++) {

				RRM_RSU::ScheduleInfo* &info = _RSU->getRRPoint()->m_TransimitScheduleInfoTable[clusterIdx][patternIdx];
				if (info == nullptr) continue;

				//说明已经传输完毕
				if (getContext()->m_EventVec[info->eventId].isFinished()) {

					//更新日志
					getContext()->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, SUCCEED, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
					writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, SUCCEED, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

					//释放调度信息对象的内存资源
					Delete::safeDelete(info);
				}
				else {//没有传输完毕，转到Wait链表，等待下一次调度
					bool isEmergency = getContext()->m_EventVec[info->eventId].getMessageType() == EMERGENCY;
					_RSU->getRRPoint()->pushToWaitEventIdList(isEmergency, clusterIdx, info->eventId);

					//更新日志
					getContext()->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, SCHEDULETABLE_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "NextTurn");
					writeTTILogInfo(g_FileTTILogInfo, getContext()->m_TTI, SCHEDULETABLE_TO_WAIT, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "NextTurn");

					//释放调度信息对象的内存资源
					Delete::safeDelete(info);
				}
			}
		}
	}
}



void RRM_RR::writeClusterPerformInfo(bool isLocationUpdate, ofstream& t_File) {
	if (!isLocationUpdate) return;
	t_File << "[ TTI = " << left << setw(3) << getContext()->m_TTI << "]" << endl;
	t_File << "{" << endl;

	//打印VeUE信息
	t_File << "    VUE Info: " << endl;
	t_File << "    {" << endl;
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		RRM_VeUE &_VeUE = *(m_VeUEAry[VeUEId]);
		t_File << _VeUE.getRRPoint()->toString(2) << endl;
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
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		t_File << _RSU->getRRPoint()->toString(2) << endl;
	}
	t_File << "    }" << endl;

	t_File << "}\n\n";
}


pair<int, int> RRM_RR::getOccupiedSubCarrierRange(int t_PatternIdx) {
	pair<int, int> res;

	res.first = ns_RRM_RR::gc_RBNumPerPattern * t_PatternIdx;
	res.second = ns_RRM_RR::gc_RBNumPerPattern * (t_PatternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
