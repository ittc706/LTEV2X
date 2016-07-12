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
#include<string>
#include<tuple>
#include<fstream>
#include"Schedule.h"
#include"System.h"
#include"Exception.h"
#include"Definition.h"
#include"Global.h"

using namespace std;

void cSystem::centralizedSchedule() {
	//清除上一次调度信息
	scheduleInfoClean();

	schedulePF_RP_CSI_UL();
	
}


void cSystem::scheduleInfoClean() {
	for (ceNB &_eNB : m_eNBVec) 
		_eNB.m_vecScheduleInfo.clear();
	
}


void cSystem::schedulePF_RP_CSI_UL() {
	for (ceNB &_eNB : m_eNBVec) {//对每一个基站进行一次调度
		int k = _eNB.m_RSUIdList.size();
		vector<vector<bool>> SPU(k, vector<bool>(gc_RBNum));//每个RSU用一个vector<bool>来管理其SPU，true代表已选如该子带
		vector<int> S;//未分配的子带集合(存储子带的ID）
		
		//初始化子带集合S（这里需要不需要？每次调度前应该没有子带被占用吧）
		for (int subbandId = 0; subbandId < gc_RBNum; subbandId++) 
			if (_eNB.m_UnassignedSubband)S.push_back(subbandId);
		

		//计算每个RSU对应不同子带的PF因子
		vector<sPFInfo> F;//存储PF因子的容器
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int subbandId = 0; subbandId < gc_RBNum; subbandId++) {
				if (_eNB.m_UnassignedSubband[subbandId] == false) continue;//该子带已被分配
				double t_FactorPF= log10(1 + m_RSUVec[RSUId].m_SINR[subbandId]) / m_RSUVec[RSUId].m_AccumulateThroughput;
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
	//-----------------------TEST-----------------------
	g_OutDRAScheduleInfo << "ATTI = " << left << setw(6) << m_ATTI << "RTTI = " << left << setw(6) << m_ATTI - m_STTI << endl;
	g_OutDRAScheduleInfo << "{" << endl;
	g_OutDRAProcessInfo << "ATTI = " << left << setw(6) << m_ATTI << "RTTI = " << left << setw(6) << m_ATTI - m_STTI << endl;
	g_OutDRAProcessInfo << "{" << endl;
	//-----------------------TEST-----------------------


	bool clusterFlag = (m_ATTI - m_STTI) % m_Config.locationUpdateNTTI == 0;

	/*资源分配信息清空:包括每个RSU内的m_CallList等*/
	DRAInformationClean();

	/*根据地理位置进行分簇*/
	DRAPerformCluster(clusterFlag);

	/*根据簇大小进行时域资源的划分*/
	DRAGroupSizeBasedTDM(clusterFlag);

	/*建立接纳链表，遍历RSU内的m_VecVUE，生成m_CallList*/
	DRAUpdateAdmissionList();

	/*-----------------------WARN-----------------------
	* 如果m_DRA_RSUSwitchEventIdList不为空，说明程序需要修正
	-----------------------WARN-----------------------*/
	if (m_DRA_RSUSwitchEventIdList.size() != 0) throw Exp("接纳链表全部生成后，System级别的切换链表仍不为空！");
	
	g_OutDRAProcessInfo << "    处理完事件链表、等待链表以及切换链表后的接纳链表：" << endl;
	for (cRSU &_RSU : m_RSUVec) {
		_RSU.DRAWriteProcessInfo(g_OutDRAProcessInfo, 0);//写入接纳链表的信息
	}
	g_OutDRAProcessInfo << endl;



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



	//-----------------------TEST-----------------------
	g_OutDRAScheduleInfo << "}" << endl;
	g_OutDRAScheduleInfo << "\n\n" << endl;
	g_OutDRAProcessInfo << "}" << endl;
	g_OutDRAProcessInfo << "\n\n" << endl;
	//-----------------------TEST-----------------------
}


void cSystem::DRAInformationClean() {
	for (cRSU &_RSU : m_RSUVec)
		_RSU.DRAInformationClean();
}


void cSystem::DRAPerformCluster(bool clusterFlag) {
	if (!clusterFlag)return;

	/*清除上一次的分簇信息*/
	for (ceNB &_eNB : m_eNBVec) {
		_eNB.m_VeUEIdList.clear();
	}
	for (cRSU &_RSU : m_RSUVec) {
		_RSU.m_VeUEIdList.clear();
	}


	/*随机将VeUE分配给RSU*/
	for (int VeUEId = 0;VeUEId < m_Config.VUENum;VeUEId++) {
		int RSUId = rand() % m_Config.RSUNum;
		m_RSUVec[RSUId].m_VeUEIdList.push_back(VeUEId);
		m_VeUEVec[VeUEId].m_RSUId = RSUId;
	}

	/*将RSU内的VeUE随机分给簇*/
	for (cRSU &_RSU : m_RSUVec) {
		//首先清除上次分簇的集合
		for (int clusterIdx = 0;clusterIdx < _RSU.m_DRAClusterNum;clusterIdx++)
			_RSU.m_DRAClusterVeUEIdList[clusterIdx].clear();
		//将现在RSU内的VeUE随机分入不同的簇
		for (int VeUEId : _RSU.m_VeUEIdList) {
			int clusterIdx = rand() % _RSU.m_DRAClusterNum;
			_RSU.m_DRAClusterVeUEIdList[clusterIdx].push_back(VeUEId);
			m_VeUEVec[VeUEId].m_ClusterIdx = clusterIdx;
		}
	}

	//更新基站的VeUE容器
	for (ceNB &_eNB:m_eNBVec) {
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int VeUEId : m_RSUVec[RSUId].m_VeUEIdList) {
				_eNB.m_VeUEIdList.push_back(VeUEId);
			}
		}
	}

}








void cSystem::DRAGroupSizeBasedTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (cRSU& _RSU : m_RSUVec) 
		_RSU.DRAGroupSizeBasedTDM();

	writeClusterPerformInfo(g_OutClasterPerformInfo);
}


void cSystem::DRAUpdateAdmissionList() {
	/*首先，处理System级别的事件触发链表*/
	for (cRSU &_RSU : m_RSUVec)
		_RSU.DRAProcessSystemLevelEventList(m_ATTI, m_STTI, m_VeUEVec, m_EventVec,m_EventTTIList);

	/*其次，处理调度表（处理的情况是，数据发送完之前离开了当前的RSU），需要将该事件推送到System级别的RSU切换链表中*/
	for (cRSU &_RSU : m_RSUVec)
		_RSU.DRAProcessRSULevelScheduleInfoTable(m_ATTI, m_VeUEVec, m_EventVec, m_DRA_RSUSwitchEventIdList);

	/*然后，处理RSU级别的等待链表*/
	for (cRSU &_RSU : m_RSUVec)
		_RSU.DRAProcessRSULevelWaitingVeUEIdList(m_ATTI, m_VeUEVec, m_EventVec, m_DRA_RSUSwitchEventIdList);


	/*最后，处理System级别的RSU切换链表*/
	for (cRSU &_RSU : m_RSUVec)
		_RSU.DRAProcessSystemLevelRSUSwitchList(m_ATTI, m_VeUEVec, m_EventVec, m_DRA_RSUSwitchEventIdList);


	/*注意，这里再次处理一遍等待链表，因为RSU切换链表会将切换的事件压入等待链表，或者接纳链表*/
	for (cRSU &_RSU : m_RSUVec)
		_RSU.DRAProcessRSULevelWaitingVeUEIdList(m_ATTI, m_VeUEVec, m_EventVec, m_DRA_RSUSwitchEventIdList);
}

void cSystem::DRASelectBasedOnP13() {

}

void cSystem::DRASelectBasedOnP23() {

}

void cSystem::DRASelectBasedOnP123() {
	for (cRSU &_RSU : m_RSUVec)
		_RSU.DRASelectBasedOnP123(m_ATTI,m_VeUEVec, m_EventVec);
}




void cSystem::DRAConflictListener() {
	for (cRSU &_RSU : m_RSUVec) {
		_RSU.DRAConflictListener(m_ATTI);
	}

	g_OutDRAProcessInfo << "    处理完冲突表后的等待链表：" << endl;
	for (cRSU &_RSU : m_RSUVec) {
		_RSU.DRAWriteProcessInfo(g_OutDRAProcessInfo, 1);
	}
	g_OutDRAProcessInfo << endl;
}
