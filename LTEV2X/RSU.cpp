#include<vector>
#include<math.h>
#include<iostream>
#include<utility>
#include<tuple>
#include"RSU.h"
#include"Exception.h"

using namespace std;

extern ofstream g_OutDRAScheduleInfo;

int cRSU::DRAGetClusterIdx(int TTI) {
	int relativeCNTI = TTI%gc_DRA_NTTI;
	for (int i = 0; i < m_DRAClusterNum; i++)
		if (relativeCNTI <= get<1>(m_DRAClusterTDRInfo[i])) return i;
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
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; clusterIdx++) {
		m_DRACallList[clusterIdx].clear();
		m_DRAVecCluster[clusterIdx].clear();
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
	double VUESizePerTTI = 1 / static_cast<double>(gc_DRA_NTTI)*static_cast<double>(m_VecVUE.size());

	/*由于直接给每个簇分配了一个TTI时隙，因此每个簇的大小需要进行调整，需要减去VUESizePerMTI*/
	for (int i = 0; i < m_DRAClusterNum; i++) 
		clusterSize[i] = static_cast<double>(m_DRAVecCluster[i].size()) - VUESizePerTTI;
	

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


void cRSU::DRABuildCallList(std::vector<cVeUE>&v) {
	DRAReaddConflictListToCallList();
	for (int clusterIdx = 0; clusterIdx < m_DRAClusterNum; ++clusterIdx) {
		for (int UEId : m_DRAVecCluster[clusterIdx]) {
			if (v[UEId].m_isHavingDataToTransmit) { //若车辆有数据要传，将其添加到m_CallList表中
				m_DRACallList[clusterIdx].push_back(UEId);
				v[UEId].m_isHavingDataToTransmit = false;
				//-----------------------WARN-----------------------
				//当车辆进入不同簇后m_isHavingDataToTransmit的重置尚未完成（即在上一个簇内尚未传递完信息）
				//进入下一个簇需要在当前簇中重新发送该信息
				//-----------------------WARN-----------------------
			}
		}
	}
}


void cRSU::DRAReaddConflictListToCallList() {
	for (const tuple<int, int, int> &t : m_DRAConflictList) {
		int VEId = get<0>(t);
		int clusterIdx = get<1>(t);
		int FBIdx = get<2>(t);
		//-----------------------WARN-----------------------
		//还需要处理当前后两轮调度中间经过了分簇，如果车辆仍然在该簇中，才会进行添加，现在尚未考虑
		//-----------------------WARN-----------------------
		m_DRACallList[clusterIdx].push_back(VEId);
	}
	/*上一TTI的冲突信息，到这里可以进行清除*/
	m_DRAConflictList.clear();
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

	for (int UEId : m_DRACallList[clusterIdx]) {//遍历该簇内呼叫链表中的用户
													 //为当前用户在可用的RB块中随机选择一个
		int FBId = v[UEId].RBSelectBasedOnP2(curAvaliableFB);//每个用户自行随机选择可用FB块
		int occupiedTTI = v[UEId].m_Message.DRA_ONTTI;//获取当前用户将要传输的信息占用的时隙(Occupy TTI)


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
		m_DRAScheduleList[clusterIdx][FBId].push_back(sDRAScheduleInfo(TTI,UEId, FBId, m_DRAClusterTDRInfo[clusterIdx], occupiedTTI));

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
	if (m_DRAConflictList.size() != 0) throw Exp("cRSU::DRAConflictListener()");
	//-----------------------TEST-----------------------
	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
			list<sDRAScheduleInfo> &list = m_DRAScheduleList[clusterIdx][FBIdx];
			if (list.size() > 1) {//多于一个VE在当前TTI，该FB上传输，即发生了冲突，将其添加到冲突列表等待重新加入呼叫链表
				for (sDRAScheduleInfo &info : list) {
					m_DRAConflictList.push_back(tuple<int,int,int>(info.VEId, clusterIdx, FBIdx));
				}		
			}
			else if (list.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
			    //-----------------------WARN-----------------------
				//没有考虑当车辆正在传输信号时，分簇将其分入另一个簇的情况
				//-----------------------WARN-----------------------

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
	for (const tuple<int, int, int> t : m_DRAConflictList) {
		int clusterIdx = get<1>(t);
		int FBIdx = get<2>(t);
		m_DRAScheduleList[clusterIdx][FBIdx].clear();
		m_DRA_RBIsAvailable[clusterIdx][FBIdx] = TTI;//将该FB资源在此时释放
	}
}



