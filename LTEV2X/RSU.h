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
	*                      ����ʽ��Դ����Ԫ
	* -------------------------------------------------------------*/
	/*------------------���ݳ�Ա------------------*/
public:
	double m_AccumulateThroughput;   //�ۼ�������
	bool m_IsScheduledUL;    //UpLink�Ƿ��ڱ�����
	bool m_IsScheduledDL;    //DownLink�Ƿ��ڱ�����
	double m_FactorPF[gc_RBNum];
	double m_SINR[gc_RBNum];
	sFeedbackInfo m_FeedbackUL;//��Ҫ���͸���վ�˵ķ�����Ϣ


	/*--------------------------------------------------------------
	*                      �ֲ�ʽ��Դ����
	*            DRA:Distributed Resource Allocation
	* -------------------------------------------------------------*/

	/*------------------���ݳ�Ա------------------*/
public:
	/*
	* RSU�����ͣ�
	* 1������ʮ��·�ڵ�RSU
	* 2���ֲ��ڵ�·�м��RSU
	*/
	eRSUType m_RSUType; 

	/*
	* һ��RSU���Ƿ�Χ�ڵĴصĸ���
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
	* ��"m_DRAPatternIsAvailable[i][j]==true"�����i��Pattern��j����
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
	* ��System����ĺ����ڲ�������
	* ���ڴ���System������¼��������¼�ת������Ӧ�������У�RSU����Ľ����������RSU����ĵȴ�����
	*/
	void DRAProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList);

	/*
	* ��System����ĺ����ڲ�������
	* ���ڴ���RSU����ĵ��ȱ�
	* ��������RSU�л��ĵ��¼����͵�System�����RSU�л������У����Ҫ������DRAProcessSystemLevelSwitchList�ĵ���
	* ��������RSU��С���л����¼����͵�RSU����ĵȴ������У����Ҫ������DRAProcessRSULevelWaitVeUEIdList�ĵ���
	*/
	void DRAProcessScheduleInfoTableWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* ��System����ĺ����ڲ�������
	* ���ڴ���RSU����ĵȴ�����
	* ��������RSU�л����¼����͵�System�����RSU�л�������
	*/
	void DRAProcessWaitEventIdList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);


	void DRAProcessWaitEventIdListWhenLocationUpdate(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, std::list<int> &systemDRASwitchEventIdList);

	/*
	* ��System����ĺ����ڲ�������
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
	int getMaxIndex(const std::vector<double>&clusterSize);

	/*
	* ���ڲ���ָ��VeUEId�����Ĵر��
	* ��Ҫ������������ж�VeUE�Ƿ����ڸ�RSU��ֱ����VeUE�����RSU������жϼ��ɣ�
	* ���������ʹ��ǰ���ǣ���֪�������ڴ���
	*/
	int getClusterIdxOfVeUE(int VeUEId);

	/*
	* ���㵱ǰ�¼�����Ӧ�ĵ�������
	* �ں���DRASelectBasedOnP123(...)�ڲ�������
    */
	std::list<std::tuple<int, int>> buildScheduleIntervalList(int TTI,const sEvent& event, std::tuple<int, int, int>ClasterTTI);
	std::list<std::tuple<int, int>> buildEmergencyScheduleInterval(int TTI, const sEvent& event);


	/*
	* ��AdmitEventIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToAdmitEventIdList(int eventId);
	void pushToEmergencyAdmitEventIdList(int eventId);

	/*
	* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToWaitEventIdList(int eventId);
	void pushToEmergencyWaitEventIdList(int eventId);

	/*
	* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToSwitchEventIdList(int eventId, std::list<int>& systemDRASwitchVeUEIdList);

	/*
	* ��TransimitScheduleInfo����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToTransmitScheduleInfoList(sDRAScheduleInfo* p,int patternIdx);
	void pushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx);

	/* 
	* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToScheduleInfoTable(int clusterIdx,int patternIdx, sDRAScheduleInfo*p);
	void pushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p);

	/*
	* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
	*/
	void pullFromScheduleInfoTable(int TTI);
	void pullFromEmergencyScheduleInfoTable();

};


inline
void cRSU::pushToAdmitEventIdList(int eventId) {
	m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::pushToEmergencyAdmitEventIdList(int eventId) {
	m_DRAEmergencyAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::pushToWaitEventIdList(int eventId) {
	m_DRAWaitEventIdList.push_back(eventId);
}

inline
void cRSU::pushToEmergencyWaitEventIdList(int eventId) {
	m_DRAEmergencyWaitEventIdList.push_back(eventId);
}

inline
void cRSU::pushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void cRSU::pushToTransmitScheduleInfoList(sDRAScheduleInfo* p,int patternIdx) {
	m_DRATransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void cRSU::pushToEmergencyTransmitScheduleInfoList(sDRAScheduleInfo* p, int patternIdx) {
	m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void cRSU::pushToScheduleInfoTable(int clusterIdx, int patternIdx, sDRAScheduleInfo*p) {
	m_DRAScheduleInfoTable[clusterIdx][patternIdx] = p;
}

inline
void cRSU::pushToEmergencyScheduleInfoTable(int patternIdx, sDRAScheduleInfo*p) {
	m_DRAEmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void cRSU::pullFromScheduleInfoTable(int TTI) {
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
void cRSU::pullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0;patternIdx < gc_DRAEmergencyTotalPatternNum;patternIdx++) {
		if (m_DRAEmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(m_DRAEmergencyScheduleInfoTable[patternIdx]);
			m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}