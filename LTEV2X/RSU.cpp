#include<vector>
#include<math.h>
#include<iostream>
#include<utility>
#include<tuple>
#include"RSU.h"
#include"Exception.h"

using namespace std;


void cRSU::DRAPerformCluster() {
	testCluster();
}

/*该方法的主要目的是为了更新"cRSU.m_DRAClusterTDRInfo"*/
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

	for (int i = 1; i < m_DRAClusterTDRInfo.size(); i++) {
		get<1>(m_DRAClusterTDRInfo[i]) += get<1>(m_DRAClusterTDRInfo[i - 1]);
		get<0>(m_DRAClusterTDRInfo[i]) = get<1>(m_DRAClusterTDRInfo[i]) - get<2>(m_DRAClusterTDRInfo[i]) + 1;
	}

	get<0>(m_DRAClusterTDRInfo[0]) = get<1>(m_DRAClusterTDRInfo[0]) - get<2>(m_DRAClusterTDRInfo[0]) + 1;
}

int cRSU::DRAGetClusterIdx() {
	int relativeCNTI = g_TTI%gc_DRA_NTTI;
	for (int i = 0; i < m_DRAClusterNum; i++)
		if (relativeCNTI <= get<1>(m_DRAClusterTDRInfo[i])) return i;
	return -1;
}


int cRSU::getMaxIndex(const std::vector<double>&v) {
	double max = 0;
	int dex = -1;
	for (int i = 0;i < v.size();i++) {
		if (v[i] > max) {
			dex = i;
			max = v[i];
		}
	}
	return dex;
}



void cRSU::DRAWriteScheduleInfo(std::ofstream& out) {
	out << "RSU: " << m_RSUId << endl;
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


void cRSU::DRAConflictListener() {

	for (int clusterIdx = 0;clusterIdx < m_DRAClusterNum;clusterIdx++) {
		//-----------------------TEST-----------------------
		/*如果每个簇的列表不为空，说明上一次的冲突还未处理完毕，说明程序需要修正*/
		if (m_DRAConflictList[clusterIdx].size() != 0) throw Exp("cRSU::DRAConflictListener()");
		//-----------------------TEST-----------------------
		for (int FBIdx = 0;FBIdx < gc_DRA_FBNum;FBIdx++) {
			list<sDRAScheduleInfo> &list = m_DRAScheduleList[clusterIdx][FBIdx];
			if (list.size() > 1) {
				for (sDRAScheduleInfo &info : list) {
					m_DRAConflictList[clusterIdx].push_back(info.VEId);
				}
			}
		}
		
	}
}



