#include<vector>
#include<math.h>
#include<iostream>
#include"RSU.h"
#include"Exception.h"

using namespace std;


void cRSU::DRAPerformCluster() {
	/*
	对该RSU范围内的UE进行分簇，会得到m_Cluster
	*/

	//生成m_DRA_ETI容器
	m_DRA_ETI = vector<int>(m_ClusterNum, 1);//每个簇至少分配一个大小的DRA_MTI
	const int remainTotalNTI = mc_DRA_NTI - m_DRA_ETI.size();//剩下可分配的时隙
	std::cout << remainTotalNTI << std::endl;
	int remainNTI = remainTotalNTI;//剩下的时域资源按组的大小进行分配

	//由于直接给每个簇分配了一个MTI时隙，因此每个簇的大小需要进行调整，需要减去这一个车辆对应的时隙数（double）
	std::vector<double> size(m_ClusterNum, 0);
	double temSize = 1 / static_cast<double>(mc_DRA_NTI)*static_cast<double>(m_VecVUE.size());
	for (int i = 0; i < m_ClusterNum; i++) {
		size[i] = static_cast<double>(m_Cluster[i].size()) - temSize;
	}

	//对应的，总的车辆数也要改变，要扣除这m_ClusterNum个时隙所对应的车辆数
	double totalUENum = static_cast<double>(m_VecVUE.size())- static_cast<double>(m_ClusterNum)*temSize;

	for (int i = 0; i < m_Cluster.size() - 1; i++) {
		if (size[i] <= 0) continue;//若当前簇车辆为负数，说明分配1个时隙已经足够了
		int tem= static_cast<int>(size[i] / totalUENum*remainTotalNTI + 0.5f);
		m_DRA_ETI[i] += tem;
		remainNTI -= tem;
		if (remainNTI < 0) {
			throw Exp("cRSU::DRAPerformCluster()");
		}
	}
	m_DRA_ETI[m_DRA_ETI.size() - 1] += remainNTI;
	for (int i = 1; i < m_DRA_ETI.size(); i++)
		m_DRA_ETI[i] += m_DRA_ETI[i - 1];
}

int cRSU::getDRAClusterIdx() {
	int relativeCNTI = m_DRA_CNTI%mc_DRA_NTI;

	return -1;
}


