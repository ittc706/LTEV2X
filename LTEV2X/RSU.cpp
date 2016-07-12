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

int cRSU::DRAGetClusterIdx(int TTI) {
	int ATTI = TTI%gc_DRA_NTTI;
	for (int i = 0; i < m_DRAClusterNum; i++)
		if (ATTI <= get<1>(m_DRAClusterTDRInfo[i])) return i;
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
	m_DRACallVeUEIdList.clear();
}


void cRSU::DRAGroupSizeBasedTDM() {
	/*假定每个簇至少有一辆车，因此每个簇至少分配一个TTI时隙*/
	m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, 0, 1));
	/*除了每个簇分配的一个时隙外，剩余可分配的时隙数量*/
	int remainNTI = gc_DRA_NTTI - m_DRAClusterNum;

	/*clusterSize存储每个簇的VeUE数目(double类型),！！！在接下来的迭代中，VeUE的数目是个浮点数！！！*/
	std::vector<double> clusterSize(m_DRAClusterNum, 0);

	/*计算每个TTI时隙对应的VeUE数目(double)，!!!浮点数!!！*/
	double VUESizePerTTI = 1 / static_cast<double>(gc_DRA_NTTI)*static_cast<double>(m_VeUEList.size());

	/*由于直接给每个簇分配了一个TTI时隙，因此每个簇的大小需要进行调整，需要减去VUESizePerMTI*/
	for (int i = 0; i < m_DRAClusterNum; i++) 
		clusterSize[i] = static_cast<double>(m_DRAClusterVeUEIdList[i].size()) - VUESizePerTTI;
	

	/*对于剩下的资源块，循环将下一时隙分配给当前比例最高的簇，分配之后，更改对应的比例（减去该时隙对应的VeUE数）*/
	while (remainNTI > 0) {
		int dex = getMaxIndex(clusterSize);
		if (dex == -1) throw Exp("还存在没有分配的时域资源，但是每个簇内的VeUE已经为负数");
		get<2>(m_DRAClusterTDRInfo[dex])++;
		remainNTI--;
		clusterSize[dex] -= VUESizePerTTI;
	}

	for (int i = 0;i < m_DRAClusterNum;i++)
		get<1>(m_DRAClusterTDRInfo[i]) = get<2>(m_DRAClusterTDRInfo[i]);

	get<1>(m_DRAClusterTDRInfo[0])--;//使区间范围从0开始

	for (int i = 1; i < m_DRAClusterNum; i++) {
		get<1>(m_DRAClusterTDRInfo[i]) += get<1>(m_DRAClusterTDRInfo[i - 1]);
		get<0>(m_DRAClusterTDRInfo[i]) = get<1>(m_DRAClusterTDRInfo[i]) - get<2>(m_DRAClusterTDRInfo[i]) + 1;
	}

	get<0>(m_DRAClusterTDRInfo[0]) = get<1>(m_DRAClusterTDRInfo[0]) - get<2>(m_DRAClusterTDRInfo[0]) + 1;
}


void cRSU::DRAProcessSystemLevelEventList(int TTI, int STTI,const std::vector<cVeUE>& systemVeUEVec, const std::vector<std::list<sEvent>>& systemEventList) {
	int clusterIdx = DRAGetClusterIdx(TTI);//当前可传输数据的簇编号
	for (const sEvent& event : systemEventList[(TTI-STTI)%systemEventList.size()]) {
		int VeUEId = event.VeUEId;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//当前事件对应的VeUE不在当前RSU中，跳过即可
			continue;
		}
		else {//当前事件对应的VeUE在当前RSU中
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx)//当前时刻事件链表中的VeUE恰好位于该RSU的该簇内，添加到当前呼叫链表
				pushToRSULevelCallVeUEIdList(VeUEId);
			else//否则，当前事件在此时不能立即传输，应转入等待链表
				pushToRSULevelWaitingVeUEIdList(VeUEId);
		}
	}
}


void cRSU::DRAProcessRSULevelWaitingVeUEIdList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::list<int> &systemDRA_RSUSwitchVeUEIdList) {
	int clusterIdx = DRAGetClusterIdx(TTI);//处于调度中的簇编号
	list<int>::iterator it = m_DRAWaitingVeUEIdList.begin();
	while (it != m_DRAWaitingVeUEIdList.end()) {
		int VeUEId = *it;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该VeUE已经不在该RSU范围内
			pushToSystemLevelRSUSwitchVeUEIdList(VeUEId,systemDRA_RSUSwitchVeUEIdList);//将其添加到System级别的切换链表中
			it=m_DRAWaitingVeUEIdList.erase(it);//将其从冲突链表中删除
		}
		else {//仍然处于当前RSU范围内
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//该VeUE处于正在调度的簇内
				pushToRSULevelCallVeUEIdList(VeUEId);//添加到RSU级别的呼叫链表中
				it=m_DRAWaitingVeUEIdList.erase(it);//将其从冲突链表中删除
			}
			else {//该VeUE不在当前被调度的簇内
				it++;
				continue; //继续等待，当前TTI不做处理
			}
		}
	}
}




void cRSU::DRAProcessSystemLevelRSUSwitchList(int TTI, const std::vector<cVeUE>& systemVeUEVec, std::list<int> &systemDRA_RSUSwitchVeUEIdList) {
	list<int>::iterator it = systemDRA_RSUSwitchVeUEIdList.begin();
	int clusterIdx = DRAGetClusterIdx(TTI);//当前可传输数据的簇编号
	while (it != systemDRA_RSUSwitchVeUEIdList.end()) {
		int VeUEId = *it;
		if (systemVeUEVec[VeUEId].m_RSUId != m_RSUId) {//该切换链表中的VeUE，不属于当前簇，跳过即可
			it++;
			continue;
		}
		else {////该切换链表中的VeUE，属于当前簇
			if (systemVeUEVec[VeUEId].m_ClusterIdx == clusterIdx) {//该切换链表中的VeUE恰好位于该RSU的当前簇内，添加到当前呼叫链表
				pushToRSULevelCallVeUEIdList(VeUEId);
				it=systemDRA_RSUSwitchVeUEIdList.erase(it);
			}
			else {//不属于当前簇，转入等待链表
				pushToRSULevelWaitingVeUEIdList(VeUEId);
				it=systemDRA_RSUSwitchVeUEIdList.erase(it);
			}
		}
	}
}


void cRSU::DRASelectBasedOnP13(int TTI, std::vector<cVeUE>&systemVeUEVec) {
}

void cRSU::DRASelectBasedOnP23(int TTI, std::vector<cVeUE>&systemVeUEVec) {
}

void cRSU::DRASelectBasedOnP123(int TTI, std::vector<cVeUE>&systemVeUEVec) {
	int relativeTTI = TTI%gc_DRA_NTTI;

	int clusterIdx = DRAGetClusterIdx(TTI);
	vector<int> curAvaliableFB;//当前TTI可用的频域块


	for (int i = 0; i < gc_DRA_FBNum; i++)
		if (TTI > m_DRA_RBIsAvailable[clusterIdx][i]) curAvaliableFB.push_back(i); //将可以占用的RB编号存入

	for (int VeUEId : m_DRACallVeUEIdList) {//遍历该簇内呼叫链表中的用户
		//为当前用户在可用的RB块中随机选择一个，每个用户自行随机选择可用FB块

		/*-----------------------WARN-----------------------
		* 没有考虑到如果可用资源为0的情况
		*-----------------------WARN-----------------------*/
		int FBId = systemVeUEVec[VeUEId].RBSelectBasedOnP2(curAvaliableFB);

		//获取当前用户将要传输的信息占用的时隙(Occupy TTI)
		int occupiedTTI = systemVeUEVec[VeUEId].m_Message.DRA_ONTTI;


		//计算当前消息所占用资源块的释放时刻,并写入m_DRA_RBIsAvailable
		int begin = get<0>(m_DRAClusterTDRInfo[clusterIdx]),
			end = get<1>(m_DRAClusterTDRInfo[clusterIdx]),
			len = get<2>(m_DRAClusterTDRInfo[clusterIdx]);
		int nextTurnBeginTTI = TTI - relativeTTI + gc_DRA_NTTI;//该RSU下一轮调度的起始TTI（第一个簇的开始时刻）
		int remainTTI = end - relativeTTI + 1;//当前一轮分配中该簇剩余的可分配时隙
		int overTTI = occupiedTTI - remainTTI;//需要到下一轮，或下几轮进行传输的时隙数量
		if (overTTI <= 0)
			m_DRA_RBIsAvailable[clusterIdx][FBId] = max(TTI + occupiedTTI - 1, m_DRA_RBIsAvailable[clusterIdx][FBId]);
		else {
			int n = overTTI / len;
			if (overTTI%len == 0) m_DRA_RBIsAvailable[clusterIdx][FBId] = max(nextTurnBeginTTI + end + (n - 1)*gc_DRA_NTTI, m_DRA_RBIsAvailable[clusterIdx][FBId]);
			else m_DRA_RBIsAvailable[clusterIdx][FBId] = max(nextTurnBeginTTI + begin + n*gc_DRA_NTTI + overTTI%len - 1, m_DRA_RBIsAvailable[clusterIdx][FBId]);
		}

		//写入调度信息
		m_DRAScheduleList[clusterIdx][FBId].push_back(sDRAScheduleInfo(TTI,VeUEId, FBId, m_DRAClusterTDRInfo[clusterIdx], occupiedTTI));

	}
	DRAWriteScheduleInfo(g_OutDRAScheduleInfo);
}




void cRSU::DRAConflictListener(int TTI) {
	//-----------------------TEST-----------------------
	/*如果每个簇的列表不为空，说明上一次的冲突还未处理完毕，说明程序需要修正*/
	if (m_DRAConflictInfoList.size() != 0) throw Exp("cRSU::DRAConflictListener()");
	//-----------------------TEST-----------------------
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
			list<sDRAScheduleInfo> &lst = m_DRAScheduleList[clusterIdx][FBIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该FB上传输，即发生了冲突，将其添加到冲突列表等待重新加入呼叫链表
				for (sDRAScheduleInfo &info : lst) {
					m_DRAConflictInfoList.push_back(tuple<int,int,int>(info.VeUEId, clusterIdx, FBIdx));
				}		
			}
			else if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
			    /*-----------------------WARN-----------------------
				* 没有考虑当VeUE正在传输信号时，分簇将其分入另一个簇的情况
				*-----------------------WARN-----------------------*/

				/*如果当前TTI==m_DRA_RBIsAvailable[clusterIdx][FBIdx]更新对应的数据*/
				if (TTI == m_DRA_RBIsAvailable[clusterIdx][FBIdx]) {
					Log::log("Transmit Succeed", lst.begin()->toLogString());
					m_DRAScheduleList[clusterIdx][FBIdx].clear();			
				}
			}
		}
	}

	DRAWriteProcessInfo(g_OutDRAProcessInfo, 2);

	/*处理冲突，维护m_DRA_RBIsAvailable以及m_DRAScheduleList*/
	DRAConflictSolve(TTI);


	if (m_DRAConflictInfoList.size() != 0) throw Exp("cRSU::DRAConflictListener");
}


void cRSU::DRAConflictSolve(int TTI) {
	for (const tuple<int, int, int> &t : m_DRAConflictInfoList) {
		int VeUEId= get<0>(t);
		int clusterIdx = get<1>(t);
		int FBIdx = get<2>(t);
		m_DRAScheduleList[clusterIdx][FBIdx].clear();
		m_DRA_RBIsAvailable[clusterIdx][FBIdx] = TTI;//将该FB资源在此时释放
		pushToRSULevelWaitingVeUEIdList(VeUEId);
	}
	m_DRAConflictInfoList.clear();
}

void cRSU::pushToRSULevelWaitingVeUEIdList(int VeUEId) {
	m_DRAWaitingVeUEIdList.push_back(VeUEId);
}
void cRSU::pushToSystemLevelRSUSwitchVeUEIdList(int VeUEId, std::list<int>& systemDRA_RSUSwitchVeUEIdList) {
	systemDRA_RSUSwitchVeUEIdList.push_back(VeUEId);
}

void cRSU::pushToRSULevelCallVeUEIdList(int VeUEId) {
	m_DRACallVeUEIdList.push_back(VeUEId);
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
				out << "                    { VeUEId :" << info.VeUEId << " ,  List: ";
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
		for (int VeUEId : m_DRACallVeUEIdList)
			out << VeUEId << " , ";
		out << "}" << endl;
		break;
	case 1://写入等待链表的信息
		out << "    RSU[" << m_RSUId << left << setw(15) << "] 's WaitingVeUEIdList : { ";
		for (int VeUEId : m_DRAWaitingVeUEIdList)
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
