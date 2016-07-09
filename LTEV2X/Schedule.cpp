/*
* =====================================================================================
*
*       Filename:  Scheduling.cpp
*
*    Description:  调度模块
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

#include<vector>
#include<iomanip>
#include<tuple>
#include"Schedule.h"
#include"System.h"
#include"Exception.h"
#include"Definition.h"
#include"Global.h"

using namespace std;

extern ofstream out;

void cSystem::centralizedSchedule() {
	//清除上一次调度信息
	scheduleInfoClean();

	schedulePF_RP_CSI_UL();
	
}


void cSystem::scheduleInfoClean() {
	for (ceNB _eNB : m_VeceNB) 
		_eNB.m_vecScheduleInfo.clear();
	
}


void cSystem::schedulePF_RP_CSI_UL() {
	for (ceNB& _eNB : m_VeceNB) {//对每一个基站进行一次调度
		int k = _eNB.m_VecRSU.size();
		vector<vector<bool>> SPU(k, vector<bool>(gc_RBNum));//每个RSU用一个vector<bool>来管理其SPU，true代表已选如该子带
		vector<int> S;//未分配的子带集合(存储子带的ID）
		
		//初始化子带集合S（这里需要不需要？每次调度前应该没有子带被占用吧）
		for (int subbandId = 0; subbandId < gc_RBNum; subbandId++) 
			if (_eNB.m_UnassignedSubband)S.push_back(subbandId);
		

		//计算每个RSU对应不同子带的PF因子
		vector<sPFInfo> F;//存储PF因子的容器
		for (int RSUId : _eNB.m_VecRSU) {
			for (int subbandId = 0; subbandId < gc_RBNum; subbandId++) {
				if (_eNB.m_UnassignedSubband[subbandId] == false) continue;//该子带已被分配
				double t_FactorPF= log10(1 + m_VecRSU[RSUId].m_SINR[subbandId]) / m_VecRSU[RSUId].m_AccumulateThroughput;
				F.push_back(sPFInfo(RSUId, subbandId, t_FactorPF));
			}
		}

		//开始排序算法
		int p = 1;
		while (S.size() != 0) {
			sPFInfo pPFInfo = selectKthPF(F, p, 0, F.size() - 1);
			int u = pPFInfo.RSUId;
			int v = pPFInfo.SubbandId;

			if (SPU[u].size() == 0 || (v>0 && SPU[u][v - 1] || v < gc_RBNum - 1 && SPU[u][v + 1])) {
				S.erase(S.begin() + v);
				SPU[u][v] = true;
				F.erase(F.begin() + p - 1);//这里注意，p代表的是顺序数，从1开始计
			}
			else {
				p++;
			}
		}

		//写入调度结果：(假传）
		for (int RSUId = 0; RSUId < k; RSUId++) {
			sScheduleInfo curRSUScheduleInfo;
			curRSUScheduleInfo.userId = RSUId;
			for (int subbandId = 0; subbandId < gc_RBNum; subbandId++) {
				if (SPU[RSUId][subbandId]) curRSUScheduleInfo.assignedSubbandId.push_back(subbandId);
			}
			if (curRSUScheduleInfo.assignedSubbandId.size() != 0) _eNB.m_vecScheduleInfo.push_back(curRSUScheduleInfo);
		}
	}
}


sPFInfo cSystem::selectKthPF(std::vector<sPFInfo>& v, int k,int p,int r) {
	if (p == r) return v[p];
	int q = partition(v, p, r);
	int n = q - p + 1;
	if (n == k) return v[q];
	else if (n > k) return  selectKthPF(v, k, p, q - 1);
	else return selectKthPF(v, k - n, q + 1, r);		
}

int cSystem::partition(std::vector<sPFInfo>& v, int p, int r) {
	int k = p - 1;
	double x = v[r].FactorPF;
	for (int j = p; j <= r - 1; j++) {
		if (v[j].FactorPF < x) {
			k++;
			exchange(v, k, j);
		}
	}
	exchange(v, k + 1, r);
	return k + 1;
}

void cSystem::exchange(std::vector<sPFInfo>& v, int i, int j) {
	sPFInfo tem = v[i];
	v[i] = v[j];
	v[j] = tem;
}


void cSystem::DRASchedule() {

	/*----------------------
	资源分配信息清空
	包括每个RSU内的m_CallList、
	-----------------------*/
	DRAInformationClean();


	/*----------------------
	建立呼叫链表，遍历RSU内的m_CallList
	转存如m_CallList
	-----------------------*/
	DRAPerformCluster();

	DRAbuildCallList();

	switch (m_DRAMode) {
	case P13:
		DRABasedOnP13();
		break;
	case P23:
		DRABasedOnP23();
		break;
	case P123:
		DRABasedOnP123();
		break;
	}

}


void cSystem::DRAInformationClean() {
	for (cRSU &_RSU : m_VecRSU) {
		for (vector<int> curCluster : _RSU.m_CallList)
			curCluster.clear();
	}
}

void cSystem::DRAbuildCallList() {
	for (cRSU &_RSU : m_VecRSU) {
		//根据m_VecRSU更新m_CallList
		for (int clusterIdx = 0; clusterIdx < _RSU.m_Cluster.size(); ++clusterIdx) {
			_RSU.m_CallList[clusterIdx].clear();
			for (int UEId : _RSU.m_Cluster[clusterIdx]) {
				if (m_VecVUE[UEId].m_isHavingDataToTransmit) //若车辆有数据要传，将其添加到m_CallList表中
					_RSU.m_CallList[clusterIdx].push_back(UEId);
			}
		}
	}
}


void cSystem::DRAPerformCluster() {
	/*
	假定已经分簇完毕，每个RSU有
	*/
	for (cRSU& _RSU : m_VecRSU) {
		_RSU.DRAPerformCluster();
	}
}


void cSystem::DRAGroupSizeBasedTDM() {

}


void cSystem::DRABasedOnP13() {

}

void cSystem::DRABasedOnP23() {

}

void cSystem::DRABasedOnP123() {
	int relativeTTI = g_TTI%gc_DRA_NTTI;
	cout << "relativeTTI: " << relativeTTI << endl;
	for (cRSU &_RSU : m_VecRSU) {//遍历所有RSU
		int clusterIdx = _RSU.getDRAClusterIdx();
		cout << "clusterIdx: " << clusterIdx << endl;
		vector<int> curAvaliableRB;//当前TTI
		for (int i = 0; i < gc_DRA_FBNum; i++)
			if (g_TTI <= _RSU.m_DRA_RBIsAvailable[clusterIdx][i]) curAvaliableRB.push_back(i); //将可以占用的RB编号存入
																										 //srand((unsigned)time(NULL));//iomanip
		for (int UEId : _RSU.m_CallList[clusterIdx]) {//遍历该簇内呼叫链表中的用户
													  //为当前用户在可用的RB块中随机选择一个
			int RBId = m_VecVUE[UEId].RBSelectBasedOnP2(curAvaliableRB);//每个用户自行随机选择可用RB块
			int occupiedTTI = m_VecVUE[UEId].m_Message.DRA_ONTTI;//获取当前用户将要传输的信息占用的时隙(Occupy TTI)

															//计算当前消息所占用资源块的释放时刻,并写入m_DRA_RBIsAvailable
			int remainTTI = get<1>(_RSU.m_DRAClusterTTI[clusterIdx]) - relativeTTI;//当前一轮分配中该簇剩余的可分配时隙
			int overTTI = occupiedTTI - remainTTI;//需要到下一轮，或下几轮进行传输的时隙数量
			if (overTTI <= 0)
				_RSU.m_DRA_RBIsAvailable[clusterIdx][RBId] = max(g_TTI + occupiedTTI, _RSU.m_DRA_RBIsAvailable[clusterIdx][RBId]);
			else
				_RSU.m_DRA_RBIsAvailable[clusterIdx][RBId] = max(g_TTI + remainTTI + overTTI / get<2>(_RSU.m_DRAClusterTTI[clusterIdx]) * gc_DRA_NTTI + overTTI%get<2>(_RSU.m_DRAClusterTTI[clusterIdx]), _RSU.m_DRA_RBIsAvailable[clusterIdx][RBId]);


			//写入调度信息
			_RSU.m_DRAScheduleList[clusterIdx][RBId] = sDRAScheduleInfo(UEId, _RSU.m_DRAClusterTTI[clusterIdx], occupiedTTI);
			
		}
		_RSU.writeDRAScheduleInfo();
	}
}



