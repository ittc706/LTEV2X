/*
* =====================================================================================
*
*       Filename:  Scheduling.cpp
*
*    Description:  ����ģ��
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
void cSystem::RRSchedule() {//RR�����ܿ�
	//����ǰ������
	RRInformationClean();
}

//<UNDONE>
void cSystem::RRInformationClean() {//��Դ������Ϣ���
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRInformationClean();
	}
}

//<UNDONE>
void cSystem::RRUpdateAdmitEventIdList(bool clusterFlag) {
	//���ȣ�����System������¼���������
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRProcessEventList(m_TTI, m_VeUEAry, m_EventVec, m_EventTTIList);
	}

	//��Σ������ǰTTI�����˷ִأ���Ҫ������ȱ�
	if (clusterFlag) {
		if (m_RRSwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");
		//����RSU����ĵ�������
		for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.RRProcessScheduleInfoWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_RRSwitchEventIdList);
		}
		//����RSU����ĵȴ�����
		for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.RRProcessWaitEventIdListWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_RRSwitchEventIdList);
		}
		//����System������л�����
		for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.RRProcessSwitchListWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_RRSwitchEventIdList);
		}
		if (m_RRSwitchEventIdList.size() != 0) throw Exp("cSystem::RRUpdateAdmitEventIdList");
	}

	//Ȼ�󣬴���RSU����ĵȴ�����
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRProcessWaitEventIdList(m_TTI, m_VeUEAry, m_EventVec, m_RRSwitchEventIdList);
	}
}

//<UNDONE>
void cSystem::RRWriteScheduleInfo() {//��¼������Ϣ��־
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
void cSystem::RRDelaystatistics() {//ʱ��ͳ��
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.RRDelaystatistics(m_TTI, m_EventVec);
	}
}




void cSystem::PFSchedule() {
	//�����һ�ε�����Ϣ
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
		vector<vector<bool>> SPU(k, vector<bool>(gc_RBNum));//ÿ��RSU��һ��vector<bool>��������SPU��true������ѡ����Ӵ�
		vector<int> S;//δ������Ӵ�����(�洢�Ӵ���Id��

					  //��ʼ���Ӵ�����S��������Ҫ����Ҫ��ÿ�ε���ǰӦ��û���Ӵ���ռ�ðɣ�
		for (int subbandId = 0; subbandId < gc_RBNum; subbandId++)
			if (_eNB.m_UnassignedSubband)S.push_back(subbandId);


		//����ÿ��RSU��Ӧ��ͬ�Ӵ���PF����
		vector<sPFInfo> F;//�洢PF���ӵ�����
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int subbandId = 0; subbandId < gc_RBNum; subbandId++) {
				if (_eNB.m_UnassignedSubband[subbandId] == false) continue;//���Ӵ��ѱ�����
				double t_FactorPF = log10(1 + m_RSUAry[RSUId].m_SINR[subbandId]) / m_RSUAry[RSUId].m_AccumulateThroughput;
				F.push_back(sPFInfo(RSUId, subbandId, t_FactorPF));
			}
		}

		//��ʼ�����㷨
		int p = 1;
		while (S.size() != 0) {
			sPFInfo pPFInfo = selectKthPF(F, p, 0, static_cast<int>(F.size()) - 1);
			int u = pPFInfo.RSUId;
			int v = pPFInfo.SubbandId;

			if (SPU[u].size() == 0 || (v>0 && SPU[u][v - 1] || v < gc_RBNum - 1 && SPU[u][v + 1])) {
				S.erase(S.begin() + v);
				SPU[u][v] = true;
				F.erase(F.begin() + p - 1);//����ע�⣬p�������˳��������1��ʼ��
			}
			else {
				p++;
			}
		}

		//д����Ƚ����(�ٴ���
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

	//��Դ������Ϣ���:����ÿ��RSU�ڵĽ��������
	DRAInformationClean();

	//  WRONG
	//���ݴش�С����ʱ����Դ�Ļ���
	DRAGroupSizeBasedTDM(clusterFlag);
	//  WRONG

	//����������������RSU�ڵ�m_VecVUE������m_CallList
	DRAUpdateAdmitEventIdList(clusterFlag);
	
	//��ǰm_TTI��DRA�㷨
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
	//д�������Ϣ
	DRAWriteScheduleInfo();

	//ͳ��ʱ����Ϣ
	DRADelaystatistics();

	//֡����ͻ
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
	//���ȣ�����System������¼���������
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.DRAProcessEventList(m_TTI, m_VeUEAry, m_EventVec, m_EventTTIList);
	}

	//��Σ������ǰTTI�����˷ִأ���Ҫ������ȱ�
	if (clusterFlag) {
		if (m_DRASwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
		//����RSU����ĵ�������
		for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.DRAProcessScheduleInfoTableWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_DRASwitchEventIdList);
		}
	    //����RSU����ĵȴ�����
		for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.DRAProcessWaitEventIdListWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_DRASwitchEventIdList);
		}
		//����System������л�����
		for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
			cRSU &_RSU = m_RSUAry[RSUId];
			_RSU.DRAProcessSwitchListWhenLocationUpdate(m_TTI, m_VeUEAry, m_EventVec, m_DRASwitchEventIdList);
		}
		if (m_DRASwitchEventIdList.size() != 0) throw Exp("cSystem::DRAUpdateAdmitEventIdList");
	}

	//Ȼ�󣬴���RSU����ĵȴ�����
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


