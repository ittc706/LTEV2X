#pragma once
#include <vector>
#include "RRMBasic.h"
#include "RSU.h"
#include "VUE.h"
#include "Enumeration.h"
#include "Exception.h"


/*===========================================
*              ģ�鳣������
* ==========================================*/
const int gc_DRA_NTTI = 25; //���дؽ���һ��DRA��ռ�õ�TTI������(NTTI:Number of TTI)
const int gc_DRAEmergencyTotalPatternNum = 5;//��������¼���Pattern����
const int gc_DRAEmergencyFBNumPerPattern = 1;//ÿ�������¼���Patternռ�õ�FB����
const int gc_DRAPatternTypeNum = 2;//�ǽ����¼���Pattern����������
const int gc_DRAPatternNumPerPatternType[gc_DRAPatternTypeNum] = { 25,5 };//��ȫƵ��ÿ��Pattern�����Ӧ��Pattern����
const int gc_DRA_FBNumPerPatternType[gc_DRAPatternTypeNum] = { 1,5 };//ÿ��Pattern������ռ��FB����
const std::vector<int> gc_DRAPatternIdxTable[gc_DRAPatternTypeNum] = { //ÿ��Pattern�����Ӧ��Pattern Idx���б�
	makeContinuousSequence(0,gc_DRAPatternNumPerPatternType[0] - 1),
	makeContinuousSequence(gc_DRAPatternNumPerPatternType[0],gc_DRAPatternNumPerPatternType[0] + gc_DRAPatternNumPerPatternType[1] - 1)
};

const int gc_DRATotalPatternNum = [&]() {
	int res = 0;
	for (int num : gc_DRAPatternNumPerPatternType)
		res += num;
	return res;
}();//���з�EmergencyPattern���͵�Pattern�����ܺ�



struct sDRAScheduleInfo {
	int eventId;//�¼����
	int VeUEId;//�������
	int RSUId;//RSU���
	int patternIdx;//Ƶ�����
	std::list<std::tuple<int, int>> occupiedIntervalList;//��ǰVeUE���д����ʵ��TTI���䣨�����䣩

	sDRAScheduleInfo() {}
	sDRAScheduleInfo(int eventId, int VeUEId, int RSUId, int patternIdx, const std::list<std::tuple<int, int>> &occupiedIntervalList) {
		this->eventId = eventId;
		this->VeUEId = VeUEId;
		this->RSUId = RSUId;
		this->patternIdx = patternIdx;
		this->occupiedIntervalList = occupiedIntervalList;
	}

	std::string toLogString(int n);

	/*
	* ���ɱ�ʾ������Ϣ��string����
	* �����¼���Id��������Id���Լ�Ҫ������¼���ռ�õ�TTI����
	*/
	std::string toScheduleString(int n);
};



class RSUAdapterDRA {
public:
	cRSU& m_HoldObj;
	RSUAdapterDRA() = delete;
	RSUAdapterDRA(cRSU& _RSU);

	//���ݳ�Ա


	/*
	* TDR:Time Domain Resource
	* �±����ر��
	* tuple�洢�ı����ĺ�������Ϊ���洢ÿ����������ʱ�������������˵㣬�Ҷ˵��Լ����䳤��
	*/
	std::vector<std::tuple<int, int, int>> m_DRAClusterTDRInfo;


	/*
	* Pattern�����Ƿ���õı��
	* ����±����ر��
	* �ڲ��±����Pattern���
	* ��"m_DRAPatternIsAvailable[i][j]==true"�����i��Pattern��j����
	*/
	std::vector<std::vector<bool>> m_DRAPatternIsAvailable;

	/*
	* ���ڴ�ŵ�ǰTTI�Ľ����¼�����
	*/
	std::list<int> m_DRAAdmitEventIdList;

	/*
	* RSU����ĵȴ��б�
	* ��ŵ���VeUEId
	* ����Դ�У�
	*		1���ִغ���System�����л�����ת���RSU����ĵȴ�����
	*		2���¼������е�ǰ���¼���������VeUEδ�ڿɷ�����Ϣ��ʱ�Σ�ת��ȴ�����
	*		3��VeUE�ڴ�����Ϣ�󣬷�����ͻ�������ͻ��ת��ȴ�����
	*/
	std::list<int> m_DRAWaitEventIdList;

	/*
	* ��ŵ��ȵ�����Ϣ
	* ����±����ر��
	* �ڲ��±����Pattern���
	*/
	std::vector<std::vector<sDRAScheduleInfo*>> m_DRAScheduleInfoTable;

	/*
	* ��ǰʱ�̵�ǰRSU�ڴ��ڴ���״̬�ĵ�����Ϣ����
	* ����±����Pattern���
	* �ڲ���list���ڴ����ͻ
	*/
	std::vector<std::list<sDRAScheduleInfo*>> m_DRATransimitScheduleInfoList;


	/*
	* Pattern�����Ƿ���õı��
	* �±����Pattern���
	* ��"m_DRAPatternIsAvailable[j]==true"����Pattern��j����
	*/
	std::vector<bool> m_DRAEmergencyPatternIsAvailable;

	/*
	* ��ǰʱ�̵ȴ�����Ľ����¼��б�
	*/
	std::list<int> m_DRAEmergencyAdmitEventIdList;

	/*
	* ��ǰʱ�̴��ڵȴ�����״̬�Ľ����¼��б�
	*/
	std::list<int> m_DRAEmergencyWaitEventIdList;

	/*
	* ��ǰʱ�̴��ڴ���״̬�Ľ����¼�������Ϣ�б�
	* ����±����pattern���
	*/
	std::vector<std::list<sDRAScheduleInfo*>> m_DRAEmergencyTransimitScheduleInfoList;

	/*
	* ��ǰʱ�̴��ڵ���״̬�Ľ����¼�������Ϣ�б�
	*/
	std::vector<sDRAScheduleInfo*> m_DRAEmergencyScheduleInfoTable;

	//��Ա����
	std::string toString(int n);

	int DRAGetClusterIdx(int TTI);

	/*
	* ���ڲ���ָ��VeUEId�����Ĵر��
	* ��Ҫ������������ж�VeUE�Ƿ����ڸ�RSU��ֱ����VeUE�����RSU������жϼ��ɣ�
	* ���������ʹ��ǰ���ǣ���֪�������ڴ���
	*/
	int DRAGetClusterIdxOfVeUE(int VeUEId);

	/*
	* ��AdmitEventIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void DRAPushToAdmitEventIdList(int eventId);
	void DRAPushToEmergencyAdmitEventIdList(int eventId);

	/*
	* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void DRAPushToWaitEventIdList(int eventId);
	void DRAPushToEmergencyWaitEventIdList(int eventId);

	/*
	* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void DRAPushToSwitchEventIdList(int eventId, std::list<int>& systemDRASwitchVeUEIdList);

	/*
	* ��TransimitScheduleInfo����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void DRAPushToTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx);
	void DRAPushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx);

	/*
	* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, sDRAScheduleInfo*p);
	void DRAPushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p);

	/*
	* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
	*/
	void DRAPullFromScheduleInfoTable(int TTI);
	void DRAPullFromEmergencyScheduleInfoTable();
};

class VeUEAdapter {
public:
	cVeUE& m_HoldObj;
	VeUEAdapter() = delete;
	VeUEAdapter(cVeUE& _VeUE) :m_HoldObj(_VeUE) {}

	std::tuple<int, int> m_ScheduleInterval;//��VeUE�ڵ�ǰ���ڵ�ǰһ�ֵ�������

	int RBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType);
	int RBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx);

	std::string toString(int n);//���ڴ�ӡVeUE��Ϣ
};



class RRM_DRA :public RRM_Basic {
public:
	RRM_DRA() = delete;
	RRM_DRA(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList,eDRAMode m_DRAMode);

	std::vector<RSUAdapterDRA> m_RSUAdapterVec;
	std::vector<VeUEAdapter> m_VeUEAdapterVec;


	//���ݳ�Ա
	eDRAMode m_DRAMode;
	std::list<int> m_DRASwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������

	int newCount = 0;//��¼��̬�����Ķ���Ĵ���

	int deleteCount = 0;//��¼ɾ����̬��������Ĵ���

	//��Ա����
public:
	/*�ӿں���*/
	void schedule() override;//DRA�����ܿأ����ǻ��۵��麯��

private:
	/*ʵ�ֺ���*/
	void DRAInformationClean();//��Դ������Ϣ���
	void DRAGroupSizeBasedTDM(bool clusterFlag);//���ڴش�С��ʱ�ָ���

	void DRAUpdateAdmitEventIdList(bool clusterFlag);//���½�������
	void DRAProcessEventList();
	void DRAProcessScheduleInfoTableWhenLocationUpdate();
	void DRAProcessWaitEventIdListWhenLocationUpdate();
	void DRAProcessSwitchListWhenLocationUpdate();
	void DRAProcessWaitEventIdList();


	void DRASelectBasedOnP13();//����P1��P3����Դ����
	void DRASelectBasedOnP23();//����P2��P3����Դ����
	void DRASelectBasedOnP123();//����P1��P2��P3����Դ����

	void DRAWriteScheduleInfo();//��¼������Ϣ��־
	void DRADelaystatistics();//ʱ��ͳ��
	void DRAConflictListener();//֡����ͻ

	//��־��¼����
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int clusterIdx, int patternIdx);

	void writeClusterPerformInfo(std::ofstream &out);//д��ִ���Ϣ����־


	//���ߺ���
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);
	std::list<std::tuple<int, int>> DRABuildScheduleIntervalList(int TTI, const sEvent& event, std::tuple<int, int, int>ClasterTTI);
	std::list<std::tuple<int, int>> DRABuildEmergencyScheduleInterval(int TTI, const sEvent& event);

};



inline
int VeUEAdapter::RBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType) {
	int size = static_cast<int>(curAvaliablePatternIdx[messageType].size());
	if (size == 0) return -1;
	return curAvaliablePatternIdx[messageType][rand() % size];
}

inline
int VeUEAdapter::RBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx) {
	int size = static_cast<int>(curAvaliableEmergencyPatternIdx.size());
	if (size == 0) return -1;
	return curAvaliableEmergencyPatternIdx[rand() % size];
}


inline
int RSUAdapterDRA::DRAGetClusterIdxOfVeUE(int VeUEId) {
	int dex = -1;
	for (int clusterIdx = 0; clusterIdx < m_HoldObj.m_DRAClusterNum; clusterIdx++) {
		for (int Id : m_HoldObj.m_DRAClusterVeUEIdList[clusterIdx])
			if (Id == VeUEId) return clusterIdx;
	}
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)���ó����ڵ�ǰRSU��");
}


inline
void RSUAdapterDRA::DRAPushToAdmitEventIdList(int eventId) {
	m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void RSUAdapterDRA::DRAPushToEmergencyAdmitEventIdList(int eventId) {
	m_DRAEmergencyAdmitEventIdList.push_back(eventId);
}

inline
void RSUAdapterDRA::DRAPushToWaitEventIdList(int eventId) {
	m_DRAWaitEventIdList.push_back(eventId);
}

inline
void RSUAdapterDRA::DRAPushToEmergencyWaitEventIdList(int eventId) {
	m_DRAEmergencyWaitEventIdList.push_back(eventId);
}

inline
void RSUAdapterDRA::DRAPushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void RSUAdapterDRA::DRAPushToTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx) {
	m_DRATransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void RSUAdapterDRA::DRAPushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx) {
	m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void RSUAdapterDRA::DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, sDRAScheduleInfo*p) {
	m_DRAScheduleInfoTable[clusterIdx][patternIdx] = p;
}

inline
void RSUAdapterDRA::DRAPushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p) {
	m_DRAEmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void RSUAdapterDRA::DRAPullFromScheduleInfoTable(int TTI) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	/*�����ڵ��ȱ��е�ǰ���Դ������Ϣѹ��m_DRATransimitEventIdList*/
	for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
		if (m_DRAScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
			m_DRATransimitScheduleInfoList[patternIdx].push_back(m_DRAScheduleInfoTable[clusterIdx][patternIdx]);
			m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
		}
	}
}

inline
void RSUAdapterDRA::DRAPullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
		if (m_DRAEmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(m_DRAEmergencyScheduleInfoTable[patternIdx]);
			m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}