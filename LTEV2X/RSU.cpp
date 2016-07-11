#include<vector>
#include<math.h>
#include<iostream>
#include<utility>
#include<tuple>
#include"RSU.h"
#include"Exception.h"
#include"Event.h"
#include"Global.h"

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


void cRSU::DRAPerformCluster() {
	testCluster();
}


void cRSU::DRAInformationClean() {
	m_DRACallList.clear();
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++) {
		m_DRAClusterVUESet[clusterIdx].clear();
	}
}


void cRSU::DRAGroupSizeBasedTDM() {
	/*假定每个簇至少有一辆车，因此每个簇至少分配一个TTI时隙*/
	m_DRAClusterTDRInfo = vector<tuple<int, int, int>>(m_DRAClusterNum, tuple<int, int, int>(0, 0, 1));
	/*除了每个簇分配的一个时隙外，剩余可分配的时隙数量*/
	int remainNTI = gc_DRA_NTTI - m_DRAClusterNum;

	/*clusterSize存储每个簇的车辆数目(double类型),！！！在接下来的迭代中，车辆的数目是个浮点数！！！*/
	std::vector<double> clusterSize(m_DRAClusterNum, 0);

	/*计算每个TTI时隙对应的车辆数目(double)，!!!浮点数!!！*/
	double VUESizePerTTI = 1 / static_cast<double>(gc_DRA_NTTI)*static_cast<double>(m_VUESet.size());

	/*由于直接给每个簇分配了一个TTI时隙，因此每个簇的大小需要进行调整，需要减去VUESizePerMTI*/
	for (int i = 0; i < m_DRAClusterNum; i++) 
		clusterSize[i] = static_cast<double>(m_DRAClusterVUESet[i].size()) - VUESizePerTTI;
	

	/*对于剩下的资源块，循环将下一时隙分配给当前比例最高的簇，分配之后，更改对应的比例（减去该时隙对应的车辆数）*/
	while (remainNTI > 0) {
		int dex = getMaxIndex(clusterSize);
		if (dex == -1) throw Exp("还存在没有分配的时域资源，但是每个簇内的车辆已经为负数");
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


void cRSU::DRABuildCallList(int TTI,const std::vector<std::list<sEvent>>&eventList) {
	/*首先添加上个TTI的冲突*/
	/*-----------------------WARN-----------------------
	* 这里即没有考虑上个TTI与当前TTI是否属于同一个簇
	*-----------------------WARN-----------------------*/
	DRAReaddConflictListToCallList(TTI);
	
	for (const sEvent& event : eventList[TTI%eventList.size()]) {
		int VEId = event.VEId;
		int clusterIdx = DRAGetClusterIdx(TTI);
		if (m_DRAClusterVUESet[clusterIdx].count(VEId))
			m_DRACallList.push_back(VEId);
	}
}


void cRSU::DRAReaddConflictListToCallList(int TTI) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	set<tuple<int,int,int>> copyConflictSet = m_DRAConflictSet;//在迭代过程中会进行删除，因此需要拷贝一份，并以此来循环
	for (const tuple<int, int, int > &t : copyConflictSet) {
		int VEId = get<0>(t);
		int clusterIdx = get<1>(t);
		int FBIdx = get<2>(t);
		if (m_VUESet.count(VEId) == 0) {
			m_DRAConflictSet.erase(t);//该VUE已经不在该RSU范围内，直接将其除去
			/*-----------------------WARN-----------------------
			* 这里没有处理这样一种情况:
			* 该RSU内冲突列表中的车辆，在当前TTI时已经处于别的RSU
			* 冲突列表的传递尚未完成
			-----------------------WARN-----------------------*/
			continue;
		}
		if (m_DRAClusterVUESet[clusterIdx].count(VEId)==1) {//该VUE在当前的簇内
			m_DRACallList.push_back(VEId);//添加到当前TTI呼叫链表中
			m_DRAConflictSet.erase(t);//将其从冲突链表中删除
		}
	}
}


void cRSU::DRASelectBasedOnP13(int TTI, std::vector<cVeUE>&v) {
}

void cRSU::DRASelectBasedOnP23(int TTI, std::vector<cVeUE>&v) {
}

void cRSU::DRASelectBasedOnP123(int TTI, std::vector<cVeUE>&v) {
	int relativeTTI = TTI%gc_DRA_NTTI;

	int clusterIdx = DRAGetClusterIdx(TTI);
	vector<int> curAvaliableFB;//当前TTI可用的频域块


	for (int i = 0; i < gc_DRA_FBNum; i++)
		if (TTI > m_DRA_RBIsAvailable[clusterIdx][i]) curAvaliableFB.push_back(i); //将可以占用的RB编号存入

	for (int VEId : m_DRACallList) {//遍历该簇内呼叫链表中的用户
		//为当前用户在可用的RB块中随机选择一个，每个用户自行随机选择可用FB块
		int FBId = v[VEId].RBSelectBasedOnP2(curAvaliableFB);

		//获取当前用户将要传输的信息占用的时隙(Occupy TTI)
		int occupiedTTI = v[VEId].m_Message.DRA_ONTTI;


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
		m_DRAScheduleList[clusterIdx][FBId].push_back(sDRAScheduleInfo(TTI,VEId, FBId, m_DRAClusterTDRInfo[clusterIdx], occupiedTTI));

	}
	DRAWriteScheduleInfo(g_OutDRAScheduleInfo);
}







void cRSU::DRAWriteScheduleInfo(std::ofstream& out) {
	out << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   RSU: " << m_RSUId <<"   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<< endl;
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++) {
		out << "  Cluster: " << clusterIdx << endl;
		for (int FBIdx = 0; FBIdx < gc_DRA_FBNum; FBIdx++) {
			out << "    FB: " << FBIdx << endl;
			out << "      Released TTI: " << m_DRA_RBIsAvailable[clusterIdx][FBIdx]<<endl;
			int cnt = 0;
			for (sDRAScheduleInfo & info : m_DRAScheduleList[clusterIdx][FBIdx]) {
				if (cnt++ == 0) out << "      ========================================" << endl;
				else out << "      ----------------------------------------" << endl;
				out << "      VEId: " << info.VEId << endl;
				out << "      OccupiedTTI: ";
				for (tuple<int, int> t : info.occupiedInterval)
					out << "[" << get<0>(t) << " , " << get<1>(t) << "] , ";
				out << endl;
			}
		}
	}
}


void cRSU::DRAConflictListener(int TTI) {
	//-----------------------TEST-----------------------
	/*如果每个簇的列表不为空，说明上一次的冲突还未处理完毕，说明程序需要修正*/
	if (m_DRAConflictSet.size() != 0) throw Exp("cRSU::DRAConflictListener()");
	//-----------------------TEST-----------------------
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
			list<sDRAScheduleInfo> &list = m_DRAScheduleList[clusterIdx][FBIdx];
			if (list.size() > 1) {//多于一个VE在当前TTI，该FB上传输，即发生了冲突，将其添加到冲突列表等待重新加入呼叫链表
				for (sDRAScheduleInfo &info : list) {
					m_DRAConflictSet.insert(tuple<int,int,int>(info.VEId, clusterIdx, FBIdx));
				}		
			}
			else if (list.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
			    /*-----------------------WARN-----------------------
				* 没有考虑当车辆正在传输信号时，分簇将其分入另一个簇的情况
				*-----------------------WARN-----------------------*/

				/*如果当前TTI==m_DRA_RBIsAvailable[clusterIdx][FBIdx]更新对应的数据*/
				if (TTI == m_DRA_RBIsAvailable[clusterIdx][FBIdx])
					m_DRAScheduleList[clusterIdx][FBIdx].clear();
			}
		}
	}

	/*处理冲突，维护m_DRA_RBIsAvailable以及m_DRAScheduleList*/
	DRAConflictSolve(TTI);
}


void cRSU::DRAConflictSolve(int TTI) {
	for (const tuple<int, int, int> t : m_DRAConflictSet) {
		int clusterIdx = get<1>(t);
		int FBIdx = get<2>(t);
		m_DRAScheduleList[clusterIdx][FBIdx].clear();
		m_DRA_RBIsAvailable[clusterIdx][FBIdx] = TTI;//将该FB资源在此时释放
	}
}



