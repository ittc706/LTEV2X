#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Event.h"
#include"Global.h"
#include"Utility.h"

int newCount = 0;
int deleteCount = 0;

using namespace std;

int cRSU::s_RSUCount = 0;

const int cRSU::s_DRAPatternNumPerPatternType[s_DRAPatternTypeNum] = { 20,5 };

const int cRSU::s_DRA_FBNumPerPatternType[s_DRAPatternTypeNum] = { 1,5 };

const std::list<int> cRSU::s_DRAPatternIdxList[s_DRAPatternTypeNum] = { 
	Function::makeContinuousSequence(0,19),
	Function::makeContinuousSequence(20,24)
};

const int cRSU::s_DRATotalPatternNum = Function::sumArray(s_DRAPatternNumPerPatternType);


cRSU::cRSU() :m_DRAClusterNum(4) {
	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);
	m_DRAPatternIsAvailable = vector<vector<bool>>(m_DRAClusterNum, vector<bool>(s_DRATotalPatternNum, true));
	m_DRAScheduleInfoTable = vector<vector<sDRAScheduleInfo*>>(m_DRAClusterNum, vector<sDRAScheduleInfo*>(s_DRATotalPatternNum,nullptr));
	m_DRATransimitScheduleInfoList = vector<list<sDRAScheduleInfo*>>(s_DRATotalPatternNum, list<sDRAScheduleInfo*>(0, nullptr));
}


int cRSU::DRAGetClusterIdx(int TTI) {
	int roundATTI = TTI%gc_DRA_NTTI; //将TTI映射到[0-gc_DRA_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_DRAClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


int cRSU::getMaxIndex(const std::vector<double>&clusterSize) {
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

int cRSU::getClusterIdxOfVeUE(int VeUEId) {
	int dex = -1;
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		for (int Id : m_DRAClusterVeUEIdList[clusterIdx])
			if (Id == VeUEId) return clusterIdx;
	}
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)：该车不在当前RSU中");
}


list<tuple<int, int>> cRSU::buildScheduleIntervalList(int TTI,sEvent event, std::tuple<int, int, int>ClasterTTI) {
	list<tuple<int,int>> scheduleIntervalList;
	int eventId = event.eventId;
	int occupiedTTI = event.message.DRA_ONTTIPerFB / cRSU::s_DRA_FBNumPerPatternType[event.message.messageType];
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




void cRSU::DRAInformationClean() {
	m_DRAAdmitEventIdList.clear();
}


void cRSU::DRAGroupSizeBasedTDM() {
	//特殊情况，当该RSU内无一辆车时
	if (m_VeUEIdList.size() == 0) {
		/*-----------------------WARN-----------------------
		* 若赋值为(0,-1,0)会导致获取当前簇编号失败，导致其他地方需要讨论
		* 因此直接给每个簇都赋值为整个区间，反正也没有任何作用，免得其他部分讨论
		*-----------------------WARN-----------------------*/
		m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, gc_DRA_NTTI-1, gc_DRA_NTTI));
		return;
	}

	//初始化
	m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, 0, 0));

	//计算每个TTI时隙对应的VeUE数目(double)，!!!浮点数!!！
	double VeUESizePerTTI = static_cast<double>(m_VeUEIdList.size()) / static_cast<double>(gc_DRA_NTTI);

	//clusterSize存储每个簇的VeUE数目(double)，!!!浮点数!!！
	std::vector<double> clusterSize(m_DRAClusterNum, 0);

	//初始化clusterSize
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++)
		clusterSize[clusterIdx] = static_cast<double>(m_DRAClusterVeUEIdList[clusterIdx].size());

	//首先给至少有一辆车的簇分配一份TTI
	int basicNTTI = 0;
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		//如果该簇内至少有一辆VeUE，直接分配给一个单位的时域资源
		if (m_DRAClusterVeUEIdList[clusterIdx].size() != 0) {
			get<2>(m_DRAClusterTDRInfo[clusterIdx]) = 1;
			clusterSize[clusterIdx] -= VeUESizePerTTI;
			basicNTTI++;
		}
	}

	//除了给不为空的簇分配的一个TTI外，剩余可分配的TTI数量
	int remainNTTI = gc_DRA_NTTI - basicNTTI;
	
	//对于剩下的资源块，循环将下一时隙分配给当前比例最高的簇，分配之后，更改对应的比例（减去该时隙对应的VeUE数）
	while (remainNTTI > 0) {
		int dex = getMaxIndex(clusterSize);
		if (dex == -1) throw Exp("还存在没有分配的时域资源，但是每个簇内的VeUE已经为负数");
		get<2>(m_DRAClusterTDRInfo[dex])++;
		remainNTTI--;
		clusterSize[dex] -= VeUESizePerTTI;
	}

	//开始生成区间范围，闭区间
	get<0>(m_DRAClusterTDRInfo[0]) = 0;
	get<1>(m_DRAClusterTDRInfo[0]) = get<0>(m_DRAClusterTDRInfo[0]) + get<2>(m_DRAClusterTDRInfo[0]) - 1;
	for (int clusterIdx = 1;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		get<0>(m_DRAClusterTDRInfo[clusterIdx]) = get<1>(m_DRAClusterTDRInfo[clusterIdx - 1]) + 1;
		get<1>(m_DRAClusterTDRInfo[clusterIdx]) = get<0>(m_DRAClusterTDRInfo[clusterIdx]) + get<2>(m_DRAClusterTDRInfo[clusterIdx]) - 1;
	}
}


void cRSU::DRAProcessSystemLevelEventList(int TTI,const std::vector<cVeUE>& systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList) {
	int clusterIdx = DRAGetClusterIdx(TTI);//当前可传输数据的簇编号
	for (int eventId : systemEventTTIList[TTI]) {
		sEvent event = systemEventVec[eventId];
		int VeUEId = event.VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//当前事件对应的VeUE不在当前RSU中，跳过即可
			continue;
		}
		else {//当前事件对应的VeUE在当前RSU中
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//当前时刻事件链表中的VeUE恰好位于该RSU的该簇内，添加到当前接纳链表
				//将该事件压入接入链表
				pushToRSULevelAdmitEventIdList(eventId);

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI,1, m_RSUId,-1,-1);
				
				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 1, eventId, m_RSUId,-1,-1);
				
			}
			else {//否则，当前事件在此时不能立即传输，应转入等待链表
				  //将该事件压入等待链表
				pushToRSULevelWaitEventIdList(eventId);

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 2, m_RSUId,-1,-1);
				
				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 2, eventId, m_RSUId,-1,-1);
			}
		}
	}
}


void cRSU::DRAProcessRSULevelScheduleInfoTable(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList) {
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++) {
		for (int patternIdx = 0; patternIdx < s_DRATotalPatternNum; patternIdx++) {
			if (m_DRAScheduleInfoTable[clusterIdx][patternIdx] == nullptr) {//当前Pattern块无事件在传输
				continue;
			}
			else {
				int eventId = m_DRAScheduleInfoTable[clusterIdx][patternIdx]->eventId;
				int VeUEId = systemEventVec[eventId].VeUEId;
				if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
					pushToSystemLevelSwitchEventIdList(eventId, systemDRASwitchEventIdList);

					//并释放该调度信息的资源
					delete m_DRAScheduleInfoTable[clusterIdx][patternIdx];
					deleteCount++;
					m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

					//释放Pattern资源
					m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

					//更新该事件的日志
					systemEventVec[eventId].addEventLog(TTI, 3, m_RSUId, clusterIdx, patternIdx);
					
					//记录TTI日志
					DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 3, eventId, m_RSUId, clusterIdx, patternIdx);
				}
				else {
					if (getClusterIdxOfVeUE(VeUEId) != clusterIdx) {//RSU内部发生了簇切换，将其从调度表中取出，压入等待链表
						pushToRSULevelWaitEventIdList(eventId);

						//并释放该调度信息的资源
						delete m_DRAScheduleInfoTable[clusterIdx][patternIdx];
						deleteCount++;
						m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;

						//释放Pattern资源
						m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

						//更新该事件的日志
						systemEventVec[eventId].addEventLog(TTI, 4, m_RSUId, clusterIdx, patternIdx);
						
						//记录TTI日志
						DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 4, eventId, m_RSUId, clusterIdx, patternIdx);
					}
					else {//既没有发生RSU切换，也没有发生RSU内簇切换，什么也不做
						//doing nothing
					}
				}
			}
		}
	}
}


void cRSU::DRAProcessRSULevelWaitEventIdList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::vector<sEvent>& systemEventVec,std::list<int> &systemDRASwitchEventIdList) {
	int clusterIdx = DRAGetClusterIdx(TTI);//处于调度中的簇编号
	list<int>::iterator it = m_DRAWaitEventIdList.begin();
	while (it != m_DRAWaitEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该VeUE已经不在该RSU范围内
			pushToSystemLevelSwitchEventIdList(eventId,systemDRASwitchEventIdList);//将其添加到System级别的RSU切换链表中
			it=m_DRAWaitEventIdList.erase(it);//将其从等待链表中删除

			//更新该事件的日志
			systemEventVec[eventId].addEventLog(TTI, 5, m_RSUId, -1, -1);
			
			//记录TTI日志
			DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 5, eventId, m_RSUId, -1, -1);
		}
		else {//仍然处于当前RSU范围内
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//该VeUE处于正在调度的簇内
				pushToRSULevelAdmitEventIdList(eventId);//添加到RSU级别的接纳链表中
				it=m_DRAWaitEventIdList.erase(it);//将其从冲突链表中删除

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 6, m_RSUId, -1, -1);
				
				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 6, eventId, m_RSUId, -1, -1);
			}
			else {//该VeUE不在当前被调度的簇内
				it++;
				continue; //继续等待，当前TTI不做处理
			}
		}
	}
}




void cRSU::DRAProcessSystemLevelSwitchList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::vector<sEvent>& systemEventVec,std::list<int> &systemDRASwitchEventIdList) {
	list<int>::iterator it = systemDRASwitchEventIdList.begin();
	int clusterIdx = DRAGetClusterIdx(TTI);//当前可传输数据的簇编号
	while (it != systemDRASwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该切换链表中的事件对应的VeUE，不属于当前簇，跳过即可
			it++;
			continue;
		}
		else {//该切换链表中的VeUE，属于当前簇
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//该切换链表中的VeUE恰好位于该RSU的当前簇内，添加到当前接纳链表
				pushToRSULevelAdmitEventIdList(eventId);
				it=systemDRASwitchEventIdList.erase(it);

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 7, m_RSUId, -1, -1);
				
				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 7, eventId, m_RSUId, -1, -1);
			}
			else {//不属于当前簇，转入等待链表
				pushToRSULevelWaitEventIdList(eventId);
				it=systemDRASwitchEventIdList.erase(it);

				//更新该事件的日志
				systemEventVec[eventId].addEventLog(TTI, 8, m_RSUId, -1, -1);
				
				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 8, eventId, m_RSUId, -1, -1);
			}
		}
	}
}


void cRSU::DRASelectBasedOnP13(int TTI, std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec) {
}

void cRSU::DRASelectBasedOnP23(int TTI, std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec) {
}

void cRSU::DRASelectBasedOnP123(int TTI, std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec) {
	int roundATTI = TTI%gc_DRA_NTTI;//将TTI映射到[0-gc_DRA_NTTI)的范围

	int clusterIdx = DRAGetClusterIdx(TTI);

	/*
	* 当前TTI可用的Pattern块编号
	* 下标代表的Pattern种类的编号
	* 每个list代表该种类Pattern可用的Pattern编号
	*/
	vector<vector<int>> curAvaliablePatternIdx(s_DRAPatternTypeNum);
	int temCnt = 0;
	for (int patternTypeIdx = 0; patternTypeIdx < s_DRAPatternTypeNum; patternTypeIdx++) {
		for (int patternIdx : s_DRAPatternIdxList[patternTypeIdx]) {
			if (m_DRAPatternIsAvailable[clusterIdx][patternIdx]) {
				curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				temCnt++;
			}
		}
	}

	if (temCnt == 0) {//如果资源全部已经被占用了，那么将接入链表全部转入等待链表
		//---------------WRONG----------------
		for (int eventId : m_DRAAdmitEventIdList)
			pushToRSULevelWaitEventIdList(eventId);
		return;//直接返回
		//---------------WRONG----------------
	}

	for (int eventId : m_DRAAdmitEventIdList) {//遍历该簇内接纳链表中的事件

		int VeUEId = systemEventVec[eventId].VeUEId;

		//为当前用户在可用的对应其事件类型的Pattern块中随机选择一个，每个用户自行随机选择可用Pattern块
		int patternIdx = systemVeUEVec[VeUEId].RBSelectBasedOnP2(curAvaliablePatternIdx, systemEventVec[eventId].message.messageType);

		//获取当前用户将要传输的信息占用的时隙(Occupy TTI)
		int occupiedTTI = systemEventVec[eventId].message.DRA_ONTTIPerFB;


		//将资源标记为占用
		m_DRAPatternIsAvailable[clusterIdx][patternIdx] = false;

		//将调度信息压入m_DRATransimitEventIdList中
		list<tuple<int,int>> scheduleIntervalList = buildScheduleIntervalList(TTI, systemEventVec[eventId], m_DRAClusterTDRInfo[clusterIdx]);
		pushToRSULevelTransmitScheduleInfoList(new sDRAScheduleInfo(eventId, VeUEId, m_RSUId, patternIdx, scheduleIntervalList), patternIdx);
		newCount++;
	}

	pullFromScheduleInfoTable(TTI);
	
	//-----------------------OUTPUT-----------------------
	DRAWriteScheduleInfo(g_OutDRAScheduleInfo,TTI);
	//-----------------------OUTPUT-----------------------
}

void cRSU::DRADelaystatistics(int TTI,std::vector<sEvent>& systemEventVec) {
	/*
	* 该函数应该在DRASelectBasedOnP..之后调用
	* 此时事件处于等待链表、传输链表、或者调度表中（非当前簇）
	*/
	
	//处理等待链表
	for (int eventId : m_DRAWaitEventIdList)
		systemEventVec[eventId].queuingDelay++;

	//处理此刻正在将要传输的传输链表
	for (int patternIdx = 0; patternIdx < s_DRATotalPatternNum; patternIdx++) {
		for (sDRAScheduleInfo* &p : m_DRATransimitScheduleInfoList[patternIdx])
			systemEventVec[p->eventId].sendDelay++;
	}

	//处理此刻位于调度表中，但不属于当前簇的事件
	int curClusterIdx = DRAGetClusterIdx(TTI);
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++) {
		if (clusterIdx == curClusterIdx) continue;
		for (sDRAScheduleInfo *p : m_DRAScheduleInfoTable[clusterIdx]) {
			if (p == nullptr) continue;
			systemEventVec[p->eventId].queuingDelay++;
		}		
	}
}

void cRSU::DRAConflictListener(int TTI, std::vector<sEvent>& systemEventVec) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	for (int patternIdx = 0;patternIdx < s_DRATotalPatternNum;patternIdx++) {
		list<sDRAScheduleInfo*> &lst = m_DRATransimitScheduleInfoList[patternIdx];
		if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
			//-----------------------冲突处理-----------------------
			for (sDRAScheduleInfo* &info : lst) {
				//更新该事件的日志
				systemEventVec[info->eventId].addEventLog(TTI, 10, m_RSUId, clusterIdx, patternIdx);

				//首先将事件压入waiting列表
				pushToRSULevelWaitEventIdList(info->eventId);
	
				//更新该事件的日志
				systemEventVec[info->eventId].addEventLog(TTI, 9, m_RSUId, -1, -1);
				
				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 9, info->eventId, m_RSUId, -1, -1);

				//释放资源
				delete info;
				info = nullptr;
				deleteCount++;
			}
			//释放资源
			m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;

		}
		else if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
			
			sDRAScheduleInfo *info = *lst.begin();

			//更新该事件的日志
			systemEventVec[info->eventId].addEventLog(TTI, 10, m_RSUId, clusterIdx, patternIdx);

			//维护占用区间
			list<tuple<int, int>> &OIList = info->occupiedIntervalList;

			tuple<int, int> &firstInterval = *OIList.begin();
			get<0>(firstInterval)++;
			if (get<0>(firstInterval) > get<1>(firstInterval)) 
				OIList.erase(OIList.begin());//删除第一个区间
					
			if(OIList.size()==0){//说明该数据已经传输完毕

				//更新该事件的日志
				systemEventVec[info->eventId].addEventLog(TTI,0, m_RSUId,clusterIdx,patternIdx);
				
				//记录TTI日志
				DRAWriteTTILogInfo(g_OutTTILogInfo, TTI, 0, info->eventId, m_RSUId, clusterIdx, patternIdx);

				//删除该调度信息
				delete *lst.begin();
				deleteCount++;

				//释放资源
				m_DRAPatternIsAvailable[clusterIdx][patternIdx] = true;
			}
			else {//该数据仍未传完，将其压回m_DRAScheduleInfoTable
				pushToScheduleInfoTable(clusterIdx, patternIdx, *lst.begin());
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













