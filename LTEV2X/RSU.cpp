#include<vector>
#include<math.h>
#include<iostream>
#include<utility>
#include<tuple>
#include"RSU.h"
#include"Exception.h"

using namespace std;


void cRSU::DRAPerformCluster() {
	/*
	对该RSU范围内的UE进行分簇，会得到m_Cluster
	*/

	//生成m_DRA_ETI容器
	m_DRAClusterTTI = vector<tuple<int, int, int>>(m_ClusterNum,tuple<int, int, int>(0,0,1));//每个簇至少分配一个大小的DRA_MTI
	int remainNTI = gc_DRA_NTTI - m_ClusterNum;//然后对剩下的时域资源循环进行分配
	
	//clusterSize存储每个簇的车辆数目(double类型)
	std::vector<double> clusterSize(m_ClusterNum, 0);

	//VUESizePerMTI为1个MTI时隙所对应的车辆数目(double)
	double VUESizePerMTI = 1 / static_cast<double>(gc_DRA_NTTI)*static_cast<double>(m_VecVUE.size());
	
	//由于直接给每个簇分配了一个MTI时隙，因此每个簇的大小需要进行调整，需要减去VUESizePerMTI
	for (int i = 0; i < m_ClusterNum; i++) {
		clusterSize[i] = static_cast<double>(m_Cluster[i].size()) - VUESizePerMTI;
	}

	//对于剩下的资源块，循环分配给当前比例最高的簇，分配之后，更改对应的比例
	while (remainNTI > 0) {
		int dex = getMaxIndex(clusterSize);
		if (dex == -1) throw Exp("还存在没有分配的时域资源，但是每个簇内的车辆已经为负数");
		get<2>(m_DRAClusterTTI[dex])++;
		remainNTI--;
		clusterSize[dex] -= VUESizePerMTI;
	}

	for (int i = 0;i < m_ClusterNum;i++)
		get<1>(m_DRAClusterTTI[i]) = get<2>(m_DRAClusterTTI[i]);
	get<1>(m_DRAClusterTTI[0])--;//使区间范围从0开始

	for (int i = 1; i < m_DRAClusterTTI.size(); i++) {
		get<1>(m_DRAClusterTTI[i]) += get<1>(m_DRAClusterTTI[i - 1]);
		get<0>(m_DRAClusterTTI[i]) = get<1>(m_DRAClusterTTI[i]) - get<2>(m_DRAClusterTTI[i]) + 1;
	}
	get<0>(m_DRAClusterTTI[0]) = get<1>(m_DRAClusterTTI[0]) - get<2>(m_DRAClusterTTI[0]) + 1;


}

int cRSU::getDRAClusterIdx() {
	int relativeCNTI = g_TTI%gc_DRA_NTTI;
	for (int i = 0; i < m_ClusterNum; i++)
		if (relativeCNTI < get<1>(m_DRAClusterTTI[i])) return i;
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




