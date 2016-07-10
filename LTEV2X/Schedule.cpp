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
#include<fstream>
#include"Schedule.h"
#include"System.h"
#include"Exception.h"
#include"Definition.h"
#include"Global.h"

using namespace std;

extern ofstream g_OutDRAScheduleInfo;

void cSystem::centralizedSchedule() {
	//清除上一次调度信息
	scheduleInfoClean();

	schedulePF_RP_CSI_UL();
	
}


void cSystem::scheduleInfoClean() {
	for (ceNB &_eNB : m_VeceNB) 
		_eNB.m_vecScheduleInfo.clear();
	
}


void cSystem::schedulePF_RP_CSI_UL() {
	for (ceNB &_eNB : m_VeceNB) {//对每一个基站进行一次调度
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

	/*资源分配信息清空:包括每个RSU内的m_CallList等*/
	DRAInformationClean();

	/*根据地理位置进行分簇*/
	DRAPerformCluster();

	/*根据簇大小进行时域资源的划分*/
	DRAGroupSizeBasedTDM();

	/*建立呼叫链表，遍历RSU内的m_VecVUE，生成m_CallList*/
	DRABuildCallList();

	/*当前m_TTI的DRA算法*/
	switch (m_DRAMode) {
	case P13:
		DRASelectBasedOnP13();
		break;
	case P23:
		DRASelectBasedOnP23();
		break;
	case P123:
		DRASelectBasedOnP123();
		break;
	}

	/*帧听冲突*/
	DRAConflictListener();

}


void cSystem::DRAInformationClean() {
	for (cRSU &_RSU : m_VecRSU)
		_RSU.DRAInformationClean();
}


void cSystem::DRAPerformCluster() {
	for (cRSU& _RSU : m_VecRSU)
		_RSU.DRAPerformCluster();

	//-----------------------TEST-----------------------

	for (cRSU& _RSU : m_VecRSU) {
		cout << "RSU: " << _RSU.m_RSUId << " 's Cluster  :" << endl;
		for (const vector<int>& v : _RSU.m_DRAVecCluster) {
			cout << "    :";
			for (int VEId : v)
				cout << VEId << " , ";
			cout << endl;
		}
		cout << endl;
	}
	//-----------------------TEST-----------------------
}


void cSystem::DRABuildCallList() {
	for (cRSU &_RSU : m_VecRSU)
		_RSU.DRABuildCallList(m_VecVUE);

	//-----------------------TEST-----------------------
	for (cRSU& _RSU : m_VecRSU) {
		cout << "RSU: " << _RSU.m_RSUId << " 's ClusterCallList  :" << endl;
		for (const vector<int>& v : _RSU.m_DRACallList) {
			cout << "    :";
			for(int VEId:v)
				cout << VEId << " , ";
			cout << endl;
		}
		cout << endl;
	}
	//-----------------------TEST-----------------------
}





void cSystem::DRAGroupSizeBasedTDM() {
	for (cRSU& _RSU : m_VecRSU) 
		_RSU.DRAGroupSizeBasedTDM();
}


void cSystem::DRASelectBasedOnP13() {

}

void cSystem::DRASelectBasedOnP23() {

}

void cSystem::DRASelectBasedOnP123() {
	for (cRSU &_RSU : m_VecRSU)
		_RSU.DRASelectBasedOnP123(m_TTI,m_VecVUE);
}



void cSystem::DRAConflictListener() {
	for (cRSU &_RSU : m_VecRSU) {
		_RSU.DRAConflictListener(m_TTI);
	}


	//-----------------------TEST-----------------------

	for (cRSU& _RSU : m_VecRSU) {
		cout << "RSU: " << _RSU.m_RSUId << " 's DRAConflictList  : ";
		for (tuple<int,int,int> &t : _RSU.m_DRAConflictList) {
			cout << "[ " << get<0>(t) << " , " << get<1>(t) << " , " << get<2>(t) << " ]  ,";
		}
		cout << endl;
	}
	//-----------------------TEST-----------------------
}
