#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include<tuple>
#include"Schedule.h"
#include"Global.h"
#include"VUE.h"
#include"Traffic.h"
#include "IMTA.h"

class cRSU {
public:
	cRSU() {}
	/*--------------------------------------------------------------
	*                      �������˵�Ԫ
	* -------------------------------------------------------------*/

	/*------------------���ݳ�Ա------------------*/
	int m_RSUId;
	std::list<int> m_VeUEIdList;//��ǰRSU��Χ�ڵ�VeUEId�������
	float m_fAbsX;
	float m_fAbsY;
	//cChannelModel *channelModel;
	cIMTA *imta;
	float m_fantennaAngle;

	/*------------------��Ա����------------------*/
	void initialize(sRSUConfigure &t_RSUConfigure);

	/*--------------------------------------------------------------
	*                      ������Դ����Ԫ
	* -------------------------------------------------------------*/

	/*----------------------------------------------------
	*                      RR����
	* ---------------------------------------------------*/
	/*------------------���ݳ�Ա------------------*/

	/*
	* RSU����ĵȴ��б�
	* ��ŵ���VeUEId
	* ����Դ�У�
	*		1���ִغ���System�����л�����ת���RSU����ĵȴ�����
	*		2���¼������е�ǰ���¼���������VeUEδ�ڿɷ�����Ϣ��ʱ�Σ�ת��ȴ�����
	*		3��VeUE�ڴ�����Ϣ�󣬷�����ͻ�������ͻ��ת��ȴ�����
	*/
	std::list<int> m_RRWaitEventIdList;

	/*
	* ��ŵ��ȵ�����Ϣ����������ѯ�����ÿ��ʱ�̽���һ���û�
	*/
	sRRScheduleInfo* m_RRScheduleInfo;


	/*
	* ��ǰʱ�̴��ڵȴ�����״̬�Ľ����¼��б�
	*/
	std::list<int> m_RREmergencyWaitEventIdList;

	/*------------------��Ա����------------------*/
public:
	/*�ӿں���*/
	void RRInformationClean();//��Դ������Ϣ���

	/*
	* ���ڴ���System������¼��������¼�ת����RSU����ĵȴ�������
	*/
	void RRProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList);


	/*
	* ������λ�ø��º󣬴�����ȱ�
	* ��������RSU�л��ĵ��¼����͵�System�����RSU�л������У����Ҫ������RRProcessSystemLevelSwitchList�ĵ���
	* ��������RSU��С���л����¼����͵�RSU����ĵȴ������У����Ҫ������RRProcessRSULevelWaitVeUEIdList�ĵ���
	*/
	void RRProcessScheduleInfoWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList);


	/*
	* ������λ�ø��º󣬴���ȴ�����
	*/
	void RRProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList);

	/*
	* ���ڴ���System�����RSU�л��������¼�ת������Ӧ�������У�RSU����Ľ����������RSU����ĵȴ�����
	* ������Ϻ󣬸�����Ĵ�СΪ0
	*/
	void RRProcessSwitchListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemRRSwitchEventIdList);

	/*
	* ���ڴ���RSU����ĵȴ�����
	* ��������RSU�л����¼����͵�System�����RSU�л�������
	*/
	void RRProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* ��������Ϣд���ļ��У������ã�
	*/
	void RRWriteScheduleInfo(std::ofstream& out, int TTI);

	/*
	* ����TTIΪ�ؼ��ֵ���־��Ϣд���ļ��У������ã�
	*/
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int clusterIdx, int patternIdx);


private:
	/*ʵ�ֺ���*/

	/*
	* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void RRPushToWaitEventIdList(int eventId);
	void RRPushToEmergencyWaitEventIdList(int eventId);

	/*
	* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList);

	/*----------------------------------------------------
	*                      PF����
	* ---------------------------------------------------*/
	/*------------------���ݳ�Ա------------------*/
public:
	double m_AccumulateThroughput;   //�ۼ�������
	bool m_IsScheduledUL;    //UpLink�Ƿ��ڱ�����
	bool m_IsScheduledDL;    //DownLink�Ƿ��ڱ�����
	double m_FactorPF[gc_RBNum];
	double m_SINR[gc_RBNum];
	sFeedbackInfo m_FeedbackUL;//��Ҫ���͸���վ�˵ķ�����Ϣ


    /*----------------------------------------------------
    *                   �ֲ�ʽ��Դ����
    *          DRA:Distributed Resource Allocation
    * ---------------------------------------------------*/

	/*------------------���ݳ�Ա------------------*/
public:
	/*
	* һ��RSU���Ƿ�Χ�ڵĴصĸ�������ֵ���Ա���RSU�����ͣ�������ʮ��·�ڻ��ǵ�·�м�
	* ��RSU������"m_RSUType"�й�
	*/
	int m_DRAClusterNum;

	/*
	* TDR:Time Domain Resource
	* �±����ر��
	* tuple�洢�ı����ĺ�������Ϊ���洢ÿ����������ʱ�������������˵㣬�Ҷ˵��Լ����䳤��
	*/
	std::vector<std::tuple<int,int,int>> m_DRAClusterTDRInfo;

	/*
	* ���ÿ���ص�VeUE��Id������
	* �±����صı��
	*/
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;  

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




	/*------------------��Ա����------------------*/
public:
	/*�ӿں���*/

	/*
	* ���ݴ˿̵�TTI���ص�ǰ������Դ����Ĵصı��
	*/
	int DRAGetClusterIdx(int TTI);

	/*
	* ��Դ������Ϣ���
	*/
	void DRAInformationClean();

	/*
	* ���ڴش�С��ʱ�ָ���
	* ÿ�������ٷ���һ��ʱ϶
	* ʣ��ʱ϶���������з���
	*/
	void DRAGroupSizeBasedTDM(cVeUE *systemVeUEVec);

	/*
	* ���ڴ���System������¼��������¼�ת������Ӧ�������У�RSU����Ľ����������RSU����ĵȴ�����
	*/
	void DRAProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList);

	/*
	* ���ڴ���ȴ�����Ľ���
	*/
	void DRAProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* ���ڴ���RSU����ĵ��ȱ�
	* ��������RSU�л��ĵ��¼����͵�System�����RSU�л������У����Ҫ������DRAProcessSystemLevelSwitchList�ĵ���
	* ��������RSU��С���л����¼����͵�RSU����ĵȴ������У����Ҫ������DRAProcessRSULevelWaitVeUEIdList�ĵ���
	*/
	void DRAProcessScheduleInfoTableWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	
	/*
	* ���ڴ���RSU����ĵȴ�����
	* ��������RSU�л����¼����͵�System�����RSU�л�������
	*/
	void DRAProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* ���ڴ���System�����RSU�л��������¼�ת������Ӧ�������У�RSU����Ľ����������RSU����ĵȴ�����
	* ������Ϻ󣬸�����Ĵ�СΪ0
	*/
	void DRAProcessSwitchListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* ����P1��P3����Դ����
	*/
	void DRASelectBasedOnP13(int TTI,cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* ����P2��P3����Դ����
	*/
	void DRASelectBasedOnP23(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* ����P1��P2��P3����Դ����
	*/
	void DRASelectBasedOnP123(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* ֡����ͻ
	*/
	void DRAConflictListener(int TTI, std::vector<sEvent>& systemEventVec);

	/*
	* ʱ��ͳ��
	* ͳ����Դռ�õ��ۼ�TTI
	* ͳ�Ƶȴ��ۼ�TTI
	* ��DRASelectBasedOnP..֮��������
	* ��ʱ�����Ѵ������¼�������WaitEventIdList�У����ߴ�����TransimitScheduleInfoList�У�����ScheduleInfoTable��
	*/
	void DRADelaystatistics(int TTI,std::vector<sEvent>& systemEventVec);

	/*
	* ��������Ϣд���ļ��У������ã�
	*/
	void DRAWriteScheduleInfo(std::ofstream& out,int TTI);

	/*
	* ����TTIΪ�ؼ��ֵ���־��Ϣд���ļ��У������ã�
	*/
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int clusterIdx, int patternIdx);

	/*
	* ���ɰ���RSU��Ϣ��string
	*/
	std::string toString(int n);


private:
	/*ʵ�ֺ���*/

	/*
	* ��DRAGroupSizeBasedTDM(...)�ڲ�������
	* �������vector�������ֵ������
	*/
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);

	/*
	* ���ڲ���ָ��VeUEId�����Ĵر��
	* ��Ҫ������������ж�VeUE�Ƿ����ڸ�RSU��ֱ����VeUE�����RSU������жϼ��ɣ�
	* ���������ʹ��ǰ���ǣ���֪�������ڴ���
	*/
	int DRAGetClusterIdxOfVeUE(int VeUEId);

	/*
	* ���㵱ǰ�¼�����Ӧ�ĵ�������
	* �ں���DRASelectBasedOnP123(...)�ڲ�������
    */
	std::list<std::tuple<int, int>> DRABuildScheduleIntervalList(int TTI,const sEvent& event, std::tuple<int, int, int>ClasterTTI);
	std::list<std::tuple<int, int>> DRABuildEmergencyScheduleInterval(int TTI, const sEvent& event);


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
	void DRAPushToTransmitScheduleInfoList(sDRAScheduleInfo* p,int patternIdx);
	void DRAPushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx);

	/* 
	* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void DRAPushToScheduleInfoTable(int clusterIdx,int patternIdx, sDRAScheduleInfo*p);
	void DRAPushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p);

	/*
	* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
	*/
	void DRAPullFromScheduleInfoTable(int TTI);
	void DRAPullFromEmergencyScheduleInfoTable();

};

/*----------------------------------------------------
*                      RR����
* ---------------------------------------------------*/

inline
void cRSU::RRPushToWaitEventIdList(int eventId) {
	m_RRWaitEventIdList.push_back(eventId);
}

inline
void cRSU::RRPushToEmergencyWaitEventIdList(int eventId) {
	m_RREmergencyWaitEventIdList.push_back(eventId);
}

inline
void cRSU::RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}


/*----------------------------------------------------
*                   �ֲ�ʽ��Դ����
*          DRA:Distributed Resource Allocation
* ---------------------------------------------------*/
inline
void cRSU::DRAPushToAdmitEventIdList(int eventId) {
	m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToEmergencyAdmitEventIdList(int eventId) {
	m_DRAEmergencyAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToWaitEventIdList(int eventId) {
	m_DRAWaitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToEmergencyWaitEventIdList(int eventId) {
	m_DRAEmergencyWaitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void cRSU::DRAPushToTransmitScheduleInfoList(sDRAScheduleInfo* p,int patternIdx) {
	m_DRATransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void cRSU::DRAPushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx) {
	m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void cRSU::DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, sDRAScheduleInfo*p) {
	m_DRAScheduleInfoTable[clusterIdx][patternIdx] = p;
}

inline
void cRSU::DRAPushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p) {
	m_DRAEmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void cRSU::DRAPullFromScheduleInfoTable(int TTI) {
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
void cRSU::DRAPullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0;patternIdx < gc_DRAEmergencyTotalPatternNum;patternIdx++) {
		if (m_DRAEmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(m_DRAEmergencyScheduleInfoTable[patternIdx]);
			m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}