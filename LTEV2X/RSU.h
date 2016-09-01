#pragma once

#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <tuple>
#include "Schedule.h"
#include "Global.h"
#include "VUE.h"
#include "Traffic.h"
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
	int m_DRAClusterNum;//һ��RSU���Ƿ�Χ�ڵĴصĸ���
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;//���ÿ���ص�VeUE��Id������,�±����صı��
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
	int m_RRNumRBPerPattern = 5;
	int m_RRPatternNum = gc_DRATotalBandwidth / gc_BandwidthOfRB / m_RRNumRBPerPattern;

	std::vector<int> m_RRAdmitEventIdList;//��ǰTTI�����б���󳤶Ȳ�����Pattern����

	/*
	* RSU����ĵȴ��б�
	* ��ŵ���VeUEId
	* ����Դ�У�
	*		1���ִغ���System�����л�����ת���RSU����ĵȴ�����
	*		2���¼������е�ǰ���¼�������ת��ȴ�����
	*/
	std::list<int> m_RRWaitEventIdList;

	/*
	* ��ŵ��ȵ�����Ϣ
	* ������Pattern��
	*/
	std::vector<sRRScheduleInfo*> m_RRScheduleInfoTable;


	/*------------------��Ա����------------------*/
public:
	/*�ӿں���*/
	void RRInformationClean();//��Դ������Ϣ���

	/*
	* ���ڴ���System������¼��������¼�ת����RSU����ĵȴ�������
	*/
	void RRProcessEventList(int TTI, const cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec, const std::vector<std::list<int>>& systemEventTTIList);


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


	void RRProcessTransimit1(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* ʱ��ͳ��
	* ͳ����Դռ�õ��ۼ�TTI
	* ͳ�Ƶȴ��ۼ�TTI
	* ��DRASelectBasedOnP..֮��������
	* ��ʱ�����Ѵ������¼�������WaitEventIdList�У����ߴ�����TransimitScheduleInfoList�У�����ScheduleInfoTable��
	*/
	void RRDelaystatistics(int TTI, std::vector<sEvent>& systemEventVec);

	void RRProcessTransimit2(int TTI, cVeUE *systemVeUEVec, std::vector<sEvent>& systemEventVec);

	/*
	* ��������Ϣд���ļ��У������ã�
	*/
	void RRWriteScheduleInfo(std::ofstream& out, int TTI);

	/*
	* ����TTIΪ�ؼ��ֵ���־��Ϣд���ļ��У������ã�
	*/
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx);


private:
	/*ʵ�ֺ���*/


	/*
	* ��AdmitEventIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void RRPushToAdmitEventIdList(int eventId);

	/*
	* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void RRPushToWaitEventIdList(int eventId, eMessageType messageType);

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

};

/*----------------------------------------------------
*                      RR����
* ---------------------------------------------------*/

inline
void cRSU::RRPushToAdmitEventIdList(int eventId) {
	m_RRAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::RRPushToWaitEventIdList(int eventId, eMessageType messageType) {
	if (messageType == EMERGENCY) {
		m_RRWaitEventIdList.insert(m_RRWaitEventIdList.begin(), eventId);
	}
	else if(messageType==PERIOD){
		m_RRWaitEventIdList.push_back(eventId);
	}
}


inline
void cRSU::RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}


/*----------------------------------------------------
*                   �ֲ�ʽ��Դ����
*          DRA:Distributed Resource Allocation
* ---------------------------------------------------*/
