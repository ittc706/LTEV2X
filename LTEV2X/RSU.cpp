#include<vector>
#include<math.h>
#include<iostream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Event.h"
#include"Global.h"
#include"Utility.h"

using namespace std;

int cRSU::count = 0;

cRSU::cRSU() :m_DRAClusterNum(4) {
	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);
	m_DRA_RBIsAvailable = vector<vector<int>>(m_DRAClusterNum, vector<int>(gc_DRA_FBNum, -1));
	m_DRAScheduleList = vector<vector<list<sDRAScheduleInfo>>>(m_DRAClusterNum, vector<list<sDRAScheduleInfo>>(gc_DRA_FBNum));
}


int cRSU::DRAGetClusterIdx(int ATTI) {
	int roundATTI = ATTI%gc_DRA_NTTI; //将ATTI映射到[0-gc_DRA_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_DRAClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


int cRSU::getMaxIndex(const std::vector<double>&v) {
	double max = 0;
	int dex = -1;
	for (int i = 0; i < static_cast<int>(v.size()); i++) {
		if (v[i] > max) {
			dex = i;
			max = v[i];
		}
	}
	return dex;
}




void cRSU::DRAInformationClean() {
	m_DRAAdmissionEventIdList.clear();
}


void cRSU::DRAGroupSizeBasedTDM() {
	/*初始化*/
	m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, 0, 0));

	/*计算每个TTI时隙对应的VeUE数目(double)，!!!浮点数!!！*/
	double VUESizePerTTI = static_cast<double>(m_VeUEIdList.size()) / static_cast<double>(gc_DRA_NTTI);

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
			clusterSize[clusterIdx] -= VUESizePerTTI;
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
		clusterSize[dex] -= VUESizePerTTI;
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
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx)//当前时刻事件链表中的VeUE恰好位于该RSU的该簇内，添加到当前呼叫链表
				pushToRSULevelCallVeEventIdList(eventId);
			else//否则，当前事件在此时不能立即传输，应转入等待链表
				pushToRSULevelWaitingEventIdList(eventId);
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
			it=m_DRAWaitingEventIdList.erase(it);//将其从冲突链表中删除
		}
		else {//仍然处于当前RSU范围内
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//该VeUE处于正在调度的簇内
				pushToRSULevelCallVeEventIdList(eventId);//添加到RSU级别的呼叫链表中
				it=m_DRAWaitingEventIdList.erase(it);//将其从冲突链表中删除
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
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//该切换链表中的VeUE恰好位于该RSU的当前簇内，添加到当前呼叫链表
				pushToRSULevelCallVeEventIdList(eventId);
				it=systemDRA_RSUSwitchEventIdList.erase(it);
			}
			else {//不属于当前簇，转入等待链表
				pushToRSULevelWaitingEventIdList(eventId);
				it=systemDRA_RSUSwitchEventIdList.erase(it);
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


	for (int i = 0; i < gc_DRA_FBNum; i++)
		if (ATTI > m_DRA_RBIsAvailable[clusterIdx][i]) curAvaliableFB.push_back(i); //将可以占用的RB编号存入


	if (curAvaliableFB.size() == 0) {//如果资源全部已经被占用了，那么将接入链表全部转入等待链表
		for (int eventId : m_DRAAdmissionEventIdList)
			pushToRSULevelWaitingEventIdList(eventId);
		return;//直接返回
	}

	for (int eventId : m_DRAAdmissionEventIdList) {//遍历该簇内呼叫链表中的事件
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

		//写入调度信息
		m_DRAScheduleList[clusterIdx][FBId].push_back(sDRAScheduleInfo(ATTI, eventId, FBId, m_DRAClusterTDRInfo[clusterIdx], occupiedTTI));

	}
	DRAWriteScheduleInfo(g_OutDRAScheduleInfo);
}




void cRSU::DRAConflictListener(int ATTI) {
	/*-----------------------WARN-----------------------
	* 如果每个簇的列表不为空，说明上一次的冲突还未处理完毕，说明程序需要修正
	-----------------------WARN-----------------------*/
	if (m_DRAConflictInfoList.size() != 0) throw Exp("cRSU::DRAConflictListener()");


	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
			list<sDRAScheduleInfo> &lst = m_DRAScheduleList[clusterIdx][FBIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该FB上传输，即发生了冲突，将其添加到冲突列表等待重新加入呼叫链表
				for (sDRAScheduleInfo &info : lst) {
					m_DRAConflictInfoList.push_back(tuple<int,int,int>(info.eventId, clusterIdx, FBIdx));
				}		
			}
			else if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
			    /*-----------------------WARN-----------------------
				* 没有考虑当VeUE正在传输信号时，分簇将其分入另一个簇的情况
				*-----------------------WARN-----------------------*/

				/*如果当前TTI==m_DRA_RBIsAvailable[clusterIdx][FBIdx]更新对应的数据*/
				if (ATTI == m_DRA_RBIsAvailable[clusterIdx][FBIdx]) {
					Log::log("Transmit Succeed", lst.begin()->toLogString(0));
					m_DRAScheduleList[clusterIdx][FBIdx].clear();			
				}
			}
		}
	}

	DRAWriteProcessInfo(g_OutDRAProcessInfo, 2);

	/*处理冲突，维护m_DRA_RBIsAvailable以及m_DRAScheduleList*/
	DRAConflictSolve(ATTI);


	if (m_DRAConflictInfoList.size() != 0) throw Exp("cRSU::DRAConflictListener");
}


void cRSU::DRAConflictSolve(int ATTI) {
	for (const tuple<int, int, int> &t : m_DRAConflictInfoList) {
		int eventId = get<0>(t);
		int clusterIdx = get<1>(t);
		int FBIdx = get<2>(t);
		m_DRAScheduleList[clusterIdx][FBIdx].clear();
		m_DRA_RBIsAvailable[clusterIdx][FBIdx] = ATTI;//将该FB资源在此时释放
		pushToRSULevelWaitingEventIdList(eventId);
	}
	m_DRAConflictInfoList.clear();
}


void cRSU::pushToRSULevelCallVeEventIdList(int eventId) {
	m_DRAAdmissionEventIdList.push_back(eventId);
}
void cRSU::pushToRSULevelWaitingEventIdList(int eventId) {
	m_DRAWaitingEventIdList.push_back(eventId);
}
void cRSU::pushToSystemLevelRSUSwitchEventIdList(int VeUEId, std::list<int>& systemDRA_RSUSwitchVeUEIdList) {
	systemDRA_RSUSwitchVeUEIdList.push_back(VeUEId);
}



void cRSU::DRAWriteScheduleInfo(std::ofstream& out) {
	out << "    RSU[" << m_RSUId << "] :" << endl;
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
			for (sDRAScheduleInfo & info : m_DRAScheduleList[clusterIdx][FBIdx]) {
				out << "                    { eventId :" << info.eventId << " ,  List: ";
				for (const tuple<int, int> &t : info.occupiedInterval)
					out << "[ " << get<0>(t) << " , " << get<1>(t) << " ] , ";
				out << "}" << endl;
			}
			out << "                }" << endl;
			out << "            }" << endl;
		}
		out << "        }" << endl;
	}
	out << "    }" << endl;
}

void cRSU::DRAWriteProcessInfo(std::ofstream& out, int type) {
	switch (type) {
	case 0://写入呼叫链表的信息
		out << "    RSU[" << m_RSUId <<left<<setw(15)<< "] 's CallVeUEIdList : { ";
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
	case 2://写入冲突链表的信息
		out << "    RSU[" << m_RSUId << left << setw(15) << "] 's ConflictList : { ";
		for (const tuple<int,int,int> &t : m_DRAConflictInfoList)
			out << "[ " << get<0>(t)<<" , "<<get<1>(t)<<" , "<<get<2>(t)<<" ] ";
		out << "}" << endl;
		break;

	}
}
