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
#include"Global.h"

using namespace std;

//<UNDONE>
void cSystem::RRSchedule() {//RR调度总控
	//调度前清理工作
	RRInformationClean();
}

//<UNDONE>
void cSystem::RRInformationClean() {//资源分配信息清空
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRInformationClean();
	}
}

//<UNDONE>
void cSystem::RRUpdateAdmitEventIdList(bool clusterFlag) {
	//首先，处理System级别的事件触发链表
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRProcessEventList(m_TTI, m_VeUEAry, m_EventVec, m_EventTTIList);
	}

	//其次，如果当前TTI进行了分簇，需要处理调度表
	if (clusterFlag) {
		if (m_RRSwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");
		//处理RSU级别的调度链表
		for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.RRProcessScheduleInfoWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_RRSwitchEventIdList);
		}
		//处理RSU级别的等待链表
		for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.RRProcessWaitEventIdListWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_RRSwitchEventIdList);
		}
		//处理System级别的切换链表
		for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.RRProcessSwitchListWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_RRSwitchEventIdList);
		}
		if (m_RRSwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");
	}

	//然后，处理RSU级别的等待链表
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRProcessWaitEventIdList(m_TTI, m_VeUEAry, m_EventVec, m_RRSwitchEventIdList);
	}
}

//<UNDONE>
void cSystem::RRWriteScheduleInfo() {//记录调度信息日志
	g_OutRRScheduleInfo << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	g_OutRRScheduleInfo << "{" << endl;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRWriteScheduleInfo(g_OutRRScheduleInfo, m_TTI);
	}
	g_OutRRScheduleInfo << "}" << endl;
	g_OutRRScheduleInfo << "\n\n" << endl;
}

//<UNDONE>
void cSystem::RRDelaystatistics() {//时延统计
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRDelaystatistics(m_TTI, m_EventVec);
	}
}




void cSystem::PFSchedule() {
	//清除上一次调度信息
	PFInformationClean();

	schedulePF_RP_CSI_UL();
	
}


void cSystem::PFInformationClean() {
	for (int eNBId = 0;eNBId < m_Config.eNBNum;eNBId++) {
		ceNB &_eNB = m_eNBAry[eNBId];
		_eNB.m_vecScheduleInfo.clear();
	}	
}


void cSystem::schedulePF_RP_CSI_UL() {
	for (int eNBId = 0;eNBId < m_Config.eNBNum;eNBId++) {
		ceNB &_eNB = m_eNBAry[eNBId];
		int k = static_cast<int>(_eNB.m_RSUIdList.size());
		vector<vector<bool>> SPU(k, vector<bool>(gc_RBNum));//每个RSU用一个vector<bool>来管理其SPU，true代表已选如该子带
		vector<int> S;//未分配的子带集合(存储子带的Id）

					  //初始化子带集合S（这里需要不需要？每次调度前应该没有子带被占用吧）
		for (int subbandId = 0; subbandId < gc_RBNum; subbandId++)
			if (_eNB.m_UnassignedSubband)S.push_back(subbandId);


		//计算每个RSU对应不同子带的PF因子
		vector<sPFInfo> F;//存储PF因子的容器
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int subbandId = 0; subbandId < gc_RBNum; subbandId++) {
				if (_eNB.m_UnassignedSubband[subbandId] == false) continue;//该子带已被分配
				double t_FactorPF = log10(1 + m_RSUAry[RSUId].m_SINR[subbandId]) / m_RSUAry[RSUId].m_AccumulateThroughput;
				F.push_back(sPFInfo(RSUId, subbandId, t_FactorPF));
			}
		}

		//开始排序算法
		int p = 1;
		while (S.size() != 0) {
			sPFInfo pPFInfo = selectKthPF(F, p, 0, static_cast<int>(F.size()) - 1);
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
	bool clusterFlag = m_TTI  % m_Config.locationUpdateNTTI == 0;

	//资源分配信息清空:包括每个RSU内的接入链表等
	DRAInformationClean();

	//  WRONG
	//根据簇大小进行时域资源的划分
	DRAGroupSizeBasedTDM(clusterFlag);
	//  WRONG

	//建立接纳链表，遍历RSU内的m_VecVUE，生成m_CallList
	DRAUpdateAdmitEventIdList(clusterFlag);
	
	//当前m_TTI的DRA算法
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
	//写入调度信息
	DRAWriteScheduleInfo();

	//统计时延信息
	DRADelaystatistics();

	//帧听冲突
	DRAConflictListener();
}


void cSystem::DRAInformationClean() {
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRAInformationClean();
	}		
}


void cSystem::DRAGroupSizeBasedTDM(bool clusterFlag) {
	if (!clusterFlag)return;
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRAGroupSizeBasedTDM(m_VeUEAry);
	}

	writeClusterPerformInfo(g_OutClasterPerformInfo);
}


void cSystem::DRAUpdateAdmitEventIdList(bool clusterFlag) {
	//首先，处理System级别的事件触发链表
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRAProcessEventList(m_TTI, m_VeUEAry, m_EventVec, m_EventTTIList);
	}

	//其次，如果当前TTI进行了分簇，需要处理调度表
	if (clusterFlag) {
		if (m_DRASwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
		//处理RSU级别的调度链表
		for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.DRAProcessScheduleInfoTableWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_DRASwitchEventIdList);
		}
	    //处理RSU级别的等待链表
		for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.DRAProcessWaitEventIdListWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_DRASwitchEventIdList);
		}
		//处理System级别的切换链表
		for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.DRAProcessSwitchListWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_DRASwitchEventIdList);
		}
		if (m_DRASwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
	}

	//然后，处理RSU级别的等待链表
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRAProcessWaitEventIdList(m_TTI, m_VeUEAry, m_EventVec, m_DRASwitchEventIdList);
	}
}

void cSystem::DRASelectBasedOnP13() {

}

void cSystem::DRASelectBasedOnP23() {

}

void cSystem::DRASelectBasedOnP123() {
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRASelectBasedOnP123(m_TTI, m_VeUEAry, m_EventVec);
	}
}


void cSystem::DRAWriteScheduleInfo() {
	g_OutDRAScheduleInfo << "[ TTI = " << left << setw(3) << m_TTI << "]" << endl;
	g_OutDRAScheduleInfo << "{" << endl;
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRAWriteScheduleInfo(g_OutDRAScheduleInfo, m_TTI);
	}
	g_OutDRAScheduleInfo << "}" << endl;
	g_OutDRAScheduleInfo << "\n\n" << endl;
}


void cSystem::DRADelaystatistics() {
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRADelaystatistics(m_TTI, m_EventVec);
	}
}


void cSystem::DRAConflictListener() {
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRAConflictListener(m_TTI,m_EventVec);
	}
}


