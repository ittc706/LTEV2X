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

int cRSU::count = 0;

cRSU::cRSU() :m_DRAClusterNum(4) {
	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);
	m_DRA_RBIsAvailable = vector<vector<int>>(m_DRAClusterNum, vector<int>(gc_DRA_FBNum, -1));
	m_DRAScheduleInfoTable = vector<vector<sDRAScheduleInfo*>>(m_DRAClusterNum, vector<sDRAScheduleInfo*>(gc_DRA_FBNum,nullptr));
	m_DRATransimitEventIdList = vector<list<sDRAScheduleInfo*>>(gc_DRA_FBNum, list<sDRAScheduleInfo*>(0, nullptr));
}


int cRSU::DRAGetClusterIdx(int ATTI) {
	int roundATTI = ATTI%gc_DRA_NTTI; //将ATTI映射到[0-gc_DRA_NTTI)的范围
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


void cRSU::DRAInformationClean() {
	m_DRAAdmissionEventIdList.clear();
}


void cRSU::DRAGroupSizeBasedTDM() {
	/*特殊情况，当该RSU内无一辆车时*/
	if (m_VeUEIdList.size() == 0) {
		/*-----------------------WARN-----------------------
		* 若赋值为(0,-1,0)会导致获取当前簇编号失败，导致其他地方需要讨论
		* 因此直接给每个簇都赋值为整个区间，反正也没有任何作用，免得其他部分讨论
		*-----------------------WARN-----------------------*/
		m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, gc_DRA_NTTI-1, gc_DRA_NTTI));
		return;
	}

	/*初始化*/
	m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, 0, 0));

	/*计算每个TTI时隙对应的VeUE数目(double)，!!!浮点数!!！*/
	double VeUESizePerTTI = static_cast<double>(m_VeUEIdList.size()) / static_cast<double>(gc_DRA_NTTI);

	/*clusterSize存储每个簇的VeUE数目(double)，!!!浮点数!!！*/
	std::vector<double> clusterSize(m_DRAClusterNum, 0);

	/*初始化clusterSize*/
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++)
		clusterSize[clusterIdx] = static_cast<double>(m_DRAClusterVeUEIdList[clusterIdx].size());

	/*首先给至少有一辆车的簇分配一份TTI*/
	int basicNTTI = 0;
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		if (m_DRAClusterVeUEIdList[clusterIdx].size() != 0) {//如果该簇内至少有一辆VeUE，直接分配给一个单位的时域资源
			get<2>(m_DRAClusterTDRInfo[clusterIdx]) = 1;
			clusterSize[clusterIdx] -= VeUESizePerTTI;
			basicNTTI++;
		}
	}

	/*除了给不为空的簇分配的一个TTI外，剩余可分配的TTI数量*/
	int remainNTTI = gc_DRA_NTTI - basicNTTI;
	
	/*对于剩下的资源块，循环将下一时隙分配给当前比例最高的簇，分配之后，更改对应的比例（减去该时隙对应的VeUE数）*/
	while (remainNTTI > 0) {
		int dex = getMaxIndex(clusterSize);
		if (dex == -1) throw Exp("还存在没有分配的时域资源，但是每个簇内的VeUE已经为负数");
		get<2>(m_DRAClusterTDRInfo[dex])++;
		remainNTTI--;
		clusterSize[dex] -= VeUESizePerTTI;
	}

	/*开始生成区间范围，闭区间*/
	get<0>(m_DRAClusterTDRInfo[0]) = 0;
	get<1>(m_DRAClusterTDRInfo[0]) = get<0>(m_DRAClusterTDRInfo[0]) + get<2>(m_DRAClusterTDRInfo[0]) - 1;
	for (int clusterIdx = 1;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		get<0>(m_DRAClusterTDRInfo[clusterIdx]) = get<1>(m_DRAClusterTDRInfo[clusterIdx - 1]) + 1;
		get<1>(m_DRAClusterTDRInfo[clusterIdx]) = get<0>(m_DRAClusterTDRInfo[clusterIdx]) + get<2>(m_DRAClusterTDRInfo[clusterIdx]) - 1;
	}
}


void cRSU::DRAProcessSystemLevelEventList(int ATTI, int STTI,const std::vector<cVeUE>& systemVeUEVec, const std::vector<sEvent>& systemEventList, const std::vector<std::list<int>>& systemEventTTIList) {
	int clusterIdx = DRAGetClusterIdx(ATTI);//当前可传输数据的簇编号
	for (int eventId : systemEventTTIList[(ATTI-STTI)% systemEventTTIList.size()]) {
		sEvent event = systemEventList[eventId];
		int VeUEId = event.VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//当前事件对应的VeUE不在当前RSU中，跳过即可
			continue;
		}
		else {//当前事件对应的VeUE在当前RSU中
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//当前时刻事件链表中的VeUE恰好位于该RSU的该簇内，添加到当前接纳链表
				pushToRSULevelAdmissionEventIdList(eventId);
				//-----------------------TEST-----------------------
				ostringstream ss;
				ss<<"Event[" << eventId << "]：From_EventList_To_AdmissiontList";
				Log::log(g_OutLogInfo, "Switch", ss.str());
				//-----------------------TEST-----------------------
			}
			else {//否则，当前事件在此时不能立即传输，应转入等待链表
				pushToRSULevelWaitingEventIdList(eventId);
				//-----------------------TEST-----------------------
				ostringstream ss;
				ss << "Event[" << eventId << "]：From_EventList_To_WaitingList";
				Log::log(g_OutLogInfo, "Switch", ss.str());
				//-----------------------TEST-----------------------
			}
		}
	}
}


void cRSU::DRAProcessRSULevelScheduleInfoTable(int ATTI, const std::vector<cVeUE>& systemVeUEVec, const std::vector<sEvent>& systemEventVec, std::list<int> &systemDRA_RSUSwitchEventIdList) {
	int clusterIdx = DRAGetClusterIdx(ATTI);//处于调度中的簇编号
	for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
		if (m_DRAScheduleInfoTable[clusterIdx][FBIdx] == nullptr) {//当前FB块无事件在传输
			continue;
		}
		else {
			int eventId = m_DRAScheduleInfoTable[clusterIdx][FBIdx]->eventId;
			int VeUEId = systemEventVec[eventId].VeUEId;
			if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该VeUE不在当前RSU中，应将其压入System级别的切换链表
				pushToSystemLevelRSUSwitchEventIdList(eventId, systemDRA_RSUSwitchEventIdList);
				
				//并释放该调度信息的资源
				delete m_DRAScheduleInfoTable[clusterIdx][FBIdx];
				deleteCount++;
				m_DRAScheduleInfoTable[clusterIdx][FBIdx] = nullptr;

				//释放FB资源
				m_DRA_RBIsAvailable[clusterIdx][FBIdx] = ATTI;

				//-----------------------TEST-----------------------
				ostringstream ss;
				ss << "Event[" << eventId << "]：From_ScheduleTable_To_RSUSwitchList";
				Log::log(g_OutLogInfo, "Switch", ss.str());
				//-----------------------TEST-----------------------
			}
			else {
				if (getClusterIdxOfVeUE(VeUEId) != clusterIdx) {//RSU内部发生了簇切换，将其从调度表中取出，压入等待链表
					pushToRSULevelWaitingEventIdList(eventId);

					//并释放该调度信息的资源
					delete m_DRAScheduleInfoTable[clusterIdx][FBIdx];
					deleteCount++;
					m_DRAScheduleInfoTable[clusterIdx][FBIdx] = nullptr;

					//释放FB资源
					m_DRA_RBIsAvailable[clusterIdx][FBIdx] = ATTI;

					//-----------------------TEST-----------------------
					ostringstream ss;
					ss << "Event[" << eventId << "]：From_ScheduleTable_To_WaitingList";
					Log::log(g_OutLogInfo, "Switch", ss.str());
					//-----------------------TEST-----------------------
				}
				else {//既没有发生RSU切换，也没有发生RSU内簇切换，什么也不做
					//doing nothing
				}
			}
		}
	}
}


void cRSU::DRAProcessRSULevelWaitingVeUEIdList(int ATTI, const std::vector<cVeUE>& systemVeUEVec, const std::vector<sEvent>& systemEventVec,std::list<int> &systemDRA_RSUSwitchEventIdList) {
	int clusterIdx = DRAGetClusterIdx(ATTI);//处于调度中的簇编号
	list<int>::iterator it = m_DRAWaitingEventIdList.begin();
	while (it != m_DRAWaitingEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该VeUE已经不在该RSU范围内
			pushToSystemLevelRSUSwitchEventIdList(eventId,systemDRA_RSUSwitchEventIdList);//将其添加到System级别的RSU切换链表中
			it=m_DRAWaitingEventIdList.erase(it);//将其从等待链表中删除
			//-----------------------TEST-----------------------
			ostringstream ss;
			ss << "Event[" << eventId << "]：From_WaitingList_To_RSUSwitchList";
			Log::log(g_OutLogInfo, "Switch", ss.str());
			//-----------------------TEST-----------------------
		}
		else {//仍然处于当前RSU范围内
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//该VeUE处于正在调度的簇内
				pushToRSULevelAdmissionEventIdList(eventId);//添加到RSU级别的接纳链表中
				it=m_DRAWaitingEventIdList.erase(it);//将其从冲突链表中删除
				//-----------------------TEST-----------------------
				ostringstream ss;
				ss << "Event[" << eventId << "]：From_WaitingList_To_AdmissionList";
				Log::log(g_OutLogInfo, "Switch", ss.str());
				//-----------------------TEST-----------------------
			}
			else {//该VeUE不在当前被调度的簇内
				it++;
				continue; //继续等待，当前TTI不做处理
			}
		}
	}
}




void cRSU::DRAProcessSystemLevelRSUSwitchList(int ATTI, const std::vector<cVeUE>& systemVeUEVec, const std::vector<sEvent>& systemEventVec,std::list<int> &systemDRA_RSUSwitchEventIdList) {
	list<int>::iterator it = systemDRA_RSUSwitchEventIdList.begin();
	int clusterIdx = DRAGetClusterIdx(ATTI);//当前可传输数据的簇编号
	while (it != systemDRA_RSUSwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = systemEventVec[eventId].VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该切换链表中的事件对应的VeUE，不属于当前簇，跳过即可
			it++;
			continue;
		}
		else {////该切换链表中的VeUE，属于当前簇
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//该切换链表中的VeUE恰好位于该RSU的当前簇内，添加到当前接纳链表
				pushToRSULevelAdmissionEventIdList(eventId);
				it=systemDRA_RSUSwitchEventIdList.erase(it);
				//-----------------------TEST-----------------------
				ostringstream ss;
				ss << "Event[" << eventId << "]：From_RSUSwitchList_To_AdmissionList";
				Log::log(g_OutLogInfo, "Switch", ss.str());
				//-----------------------TEST-----------------------
			}
			else {//不属于当前簇，转入等待链表
				pushToRSULevelWaitingEventIdList(eventId);
				it=systemDRA_RSUSwitchEventIdList.erase(it);
				//-----------------------TEST-----------------------
				ostringstream ss;
				ss << "Event[" << eventId << "]：From_RSUSwitchList_To_WaitingList";
				Log::log(g_OutLogInfo, "Switch", ss.str());
				//-----------------------TEST-----------------------
			}
		}
	}
}


void cRSU::DRASelectBasedOnP13(int ATTI, std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec) {
}

void cRSU::DRASelectBasedOnP23(int ATTI, std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec) {
}

void cRSU::DRASelectBasedOnP123(int ATTI, std::vector<cVeUE>&systemVeUEVec, const std::vector<sEvent>& systemEventVec) {
	int roundATTI = ATTI%gc_DRA_NTTI;//将ATTI映射到[0-gc_DRA_NTTI)的范围

	int clusterIdx = DRAGetClusterIdx(ATTI);

	vector<int> curAvaliableFB;//当前TTI可用的频域块

	for (int FBIdx = 0; FBIdx < gc_DRA_FBNum; FBIdx++) 
		if (ATTI > m_DRA_RBIsAvailable[clusterIdx][FBIdx]) curAvaliableFB.push_back(FBIdx); //将可以占用的RB编号存入

	if (curAvaliableFB.size() == 0) {//如果资源全部已经被占用了，那么将接入链表全部转入等待链表
		for (int eventId : m_DRAAdmissionEventIdList)
			pushToRSULevelWaitingEventIdList(eventId);
		return;//直接返回
	}

	for (int eventId : m_DRAAdmissionEventIdList) {//遍历该簇内接纳链表中的事件

		int VeUEId = systemEventVec[eventId].VeUEId;

		//为当前用户在可用的RB块中随机选择一个，每个用户自行随机选择可用FB块
		int FBId = systemVeUEVec[VeUEId].RBSelectBasedOnP2(curAvaliableFB);

		//获取当前用户将要传输的信息占用的时隙(Occupy TTI)
		int occupiedTTI = systemEventVec[eventId].message.DRA_ONTTI;


		//计算当前消息所占用资源块的释放时刻,并写入m_DRA_RBIsAvailable
		int begin = get<0>(m_DRAClusterTDRInfo[clusterIdx]),
			end = get<1>(m_DRAClusterTDRInfo[clusterIdx]),
			len = get<2>(m_DRAClusterTDRInfo[clusterIdx]);
		int nextTurnBeginTTI = ATTI - roundATTI + gc_DRA_NTTI;//该RSU下一轮调度的起始TTI（第一个簇的开始时刻）
		int remainTTI = end - roundATTI + 1;//当前一轮分配中该簇剩余的可分配时隙
		int overTTI = occupiedTTI - remainTTI;//需要到下一轮，或下几轮进行传输的时隙数量
		if (overTTI <= 0)
			m_DRA_RBIsAvailable[clusterIdx][FBId] = max(ATTI + occupiedTTI - 1, m_DRA_RBIsAvailable[clusterIdx][FBId]);
		else {
			int n = overTTI / len;
			if (overTTI%len == 0) m_DRA_RBIsAvailable[clusterIdx][FBId] = max(nextTurnBeginTTI + end + (n - 1)*gc_DRA_NTTI, m_DRA_RBIsAvailable[clusterIdx][FBId]);
			else m_DRA_RBIsAvailable[clusterIdx][FBId] = max(nextTurnBeginTTI + begin + n*gc_DRA_NTTI + overTTI%len - 1, m_DRA_RBIsAvailable[clusterIdx][FBId]);
		}

		//将调度信息压入m_DRATransimitEventIdList中
		m_DRATransimitEventIdList[FBId].push_back(new sDRAScheduleInfo(ATTI, eventId, m_RSUId,FBId, m_DRAClusterTDRInfo[clusterIdx], occupiedTTI));
		newCount++;
	}

	pullFromScheduleInfoTable(ATTI);
	

	DRAWriteScheduleInfo(g_OutDRAScheduleInfo);
}



void cRSU::DRAConflictListener(int ATTI) {
	int clusterIdx = DRAGetClusterIdx(ATTI);
	for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
		list<sDRAScheduleInfo*> &lst = m_DRATransimitEventIdList[FBIdx];
		if (lst.size() > 1) {//多于一个VeUE在当前TTI，该FB上传输，即发生了冲突，将其添加到等待列表
			//-----------------------冲突处理-----------------------
			for (sDRAScheduleInfo* info : lst) {
				//首先将事件压入waiting列表
				pushToRSULevelWaitingEventIdList(info->eventId);
				delete info;
				info = nullptr;
				deleteCount++;
			}
			//释放资源
			m_DRA_RBIsAvailable[clusterIdx][FBIdx] = ATTI;
			//-----------------------冲突处理-----------------------
		}
		else if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
			sDRAScheduleInfo *info = *lst.begin();
			//维护占用区间
			list<tuple<int, int>> &OIList = info->occupiedIntervalList;

			tuple<int, int> &firstInterval = *OIList.begin();
			get<0>(firstInterval)++;
			if (get<0>(firstInterval) > get<1>(firstInterval)) 
				OIList.erase(OIList.begin());//删除第一个区间
					
			if(OIList.size()==0){//说明该数据已经传输完毕
				//-----------------------TEST-----------------------
				ostringstream ss;
				ss << "{ EventId = " << left << setw(3) << info->eventId;
				ss << "   RSUId = " << left << setw(3) << info->RSUId;
				ss << "   FBIdx = " << info->FBIdx << " }";
				Log::log(g_OutLogInfo,"Succeed", ss.str());
				//-----------------------TEST-----------------------
				//删除该调度信息
				delete *lst.begin();
				deleteCount++;
			}
			else {//该数据仍未传完，将其压回m_DRAScheduleInfoTable
				pushToScheduleInfoTable(clusterIdx, FBIdx, *lst.begin());
				*lst.begin() = nullptr;
			}
		}
		lst.clear();
	}
}




void cRSU::pushToRSULevelAdmissionEventIdList(int eventId) {
	m_DRAAdmissionEventIdList.push_back(eventId);
}

void cRSU::pushToRSULevelWaitingEventIdList(int eventId) {
	m_DRAWaitingEventIdList.push_back(eventId);
}

void cRSU::pushToSystemLevelRSUSwitchEventIdList(int VeUEId, std::list<int>& systemDRA_RSUSwitchVeUEIdList) {
	systemDRA_RSUSwitchVeUEIdList.push_back(VeUEId);
}

void cRSU::pushToScheduleInfoTable(int clusterIdx, int FBIdx, sDRAScheduleInfo*p) {
	m_DRAScheduleInfoTable[clusterIdx][FBIdx] = p;
}

void cRSU::pullFromScheduleInfoTable(int ATTI) {
	int clusterIdx = DRAGetClusterIdx(ATTI);
	/*将处于调度表中当前可以传输的信息压入m_DRATransimitEventIdList*/
	for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
		if (m_DRAScheduleInfoTable[clusterIdx][FBIdx] != nullptr) {
			m_DRATransimitEventIdList[FBIdx].push_back(m_DRAScheduleInfoTable[clusterIdx][FBIdx]);
			m_DRAScheduleInfoTable[clusterIdx][FBIdx] = nullptr;
		}
	}
}


void cRSU::DRAWriteScheduleInfo(std::ofstream& out) {
	out << "    RSU[" << m_RSUId << "] :" << endl;
	out << "    {" << endl;
	for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
		out << "        FB[" << FBIdx << "] :" << endl;
		for (sDRAScheduleInfo* info : m_DRATransimitEventIdList[FBIdx]) {
			out<< info->toString(3) << endl;
		}
	}
	out << "    }" << endl;

	/*out << "    RSU[" << m_RSUId << "] :" << endl;
	out << "    {" << endl;
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++) {
	out << "        Cluster[" << clusterIdx << "] :" << endl;
	out << "        {" << endl;
	for (int FBIdx = 0; FBIdx < gc_DRA_FBNum; FBIdx++) {
	out << "            FB[" << FBIdx << "] :" << endl;
	out << "            {" << endl;
	out << "                Released TTI: " << m_DRA_RBIsAvailable[clusterIdx][FBIdx] << endl;
	int cnt = 0;
	out << "                ScheduleTTLInterval List: " << endl;
	out << "                {" << endl;
	sDRAScheduleInfo *info = m_DRAScheduleInfoTable[clusterIdx][FBIdx];
	if (info != nullptr) {
	out << "                    { eventId :" << info->eventId << " ,  List: ";
	for (const tuple<int, int> &t : info->occupiedIntervalList)
	out << "[ " << get<0>(t) << " , " << get<1>(t) << " ] , ";
	out << "}" << endl;
	}
	out << "                }" << endl;
	out << "            }" << endl;
	}
	out << "        }" << endl;
	}
	out << "    }" << endl;*/
}

void cRSU::DRAWriteProcessInfo(std::ofstream& out, int type) {
	switch (type) {
	case 0://写入接纳链表的信息
		out << "    RSU[" << m_RSUId << left << setw(15) << "] 's CallVeUEIdList : { ";
		for (int VeUEId : m_DRAAdmissionEventIdList)
			out << VeUEId << " , ";
		out << "}" << endl;
		break;
	case 1://写入等待链表的信息
		out << "    RSU[" << m_RSUId << left << setw(15) << "] 's WaitingVeUEIdList : { ";
		for (int VeUEId : m_DRAWaitingEventIdList)
			g_OutDRAProcessInfo << VeUEId << " , ";
		out << "}" << endl;
		break;
	}
}


