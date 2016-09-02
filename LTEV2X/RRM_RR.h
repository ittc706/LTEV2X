#pragma once
#include<vector>
#include<list>
#include"RRM.h"
#include"Exception.h"
#include"Global.h"

/*===========================================
*              ģ�鳣������
* ==========================================*/
const int gc_RRNumRBPerPattern = 5;//ÿ��Pattern��RB����
const int gc_RRPatternNum = gc_TotalBandwidth / gc_BandwidthOfRB / gc_RRNumRBPerPattern;//�ܵ�Pattern����
const int gc_RRBitNumPerPattern = gc_RRNumRBPerPattern*gc_BitNumPerRB;//ÿ��Pattern��λTTI�����bit����

/*===========================================
*            ������Ϣ���ݽṹ
* ==========================================*/
struct sRRScheduleInfo {
	int eventId;//�¼����
	eMessageType messageType;//�¼�����
	int VeUEId;//�������
	int RSUId;//RSU���
	int patternIdx;//Ƶ�����
	int occupiedNumTTI;//��ǰVeUE���д����ʵ��TTI���䣨�����䣩

	sRRScheduleInfo() {}
	sRRScheduleInfo(int eventId, eMessageType messageType, int VeUEId, int RSUId, int patternIdx, const int &occupiedNumTTI) {
		this->eventId = eventId;
		this->messageType = messageType;
		this->VeUEId = VeUEId;
		this->RSUId = RSUId;
		this->patternIdx = patternIdx;
		this->occupiedNumTTI = occupiedNumTTI;
	}

	std::string toLogString(int n);

	/*
	* ���ɱ�ʾ������Ϣ��string����
	* �����¼���Id��������Id���Լ�Ҫ������¼���ռ�õ�TTI����
	*/
	std::string toScheduleString(int n);
};



/*===========================================
*                RSU������
* ==========================================*/
class RSUAdapterRR {
public:
	cRSU& m_HoldObj;//�����������е�ԭRSU����
	RSUAdapterRR() = delete;
	RSUAdapterRR(cRSU& _RSU);

	/*------------------���ݳ�Ա------------------*/
	

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
};


/*===========================================
*                VeUE������
* ==========================================*/
class VeUEAdapterRR {
public:
	cVeUE& m_HoldObj;//�����������е�ԭVeUE����
	VeUEAdapterRR() = delete;
	VeUEAdapterRR(cVeUE& _VeUE) :m_HoldObj(_VeUE) {}
};



class RRM_RR :public RRM_Basic {
public:
	RRM_RR() = delete;
	RRM_RR(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList);

	std::vector<RSUAdapterRR> m_RSUAdapterVec;
	std::vector<VeUEAdapterRR> m_VeUEAdapterVec;

	/*------------------���ݳ�Ա------------------*/
	std::list<int> m_RRSwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������

	int m_NewCount = 0;//��¼��̬�����Ķ���Ĵ���

	int m_DeleteCount = 0;//��¼ɾ����̬��������Ĵ���

	/*------------------��Ա����------------------*/
	void schedule() override;//DRA�����ܿأ����ǻ�����麯��

	void RRInformationClean();//��Դ������Ϣ���

	void RRUpdateAdmitEventIdList(bool clusterFlag);//���½����
	void RRProcessEventList();
	void RRProcessWaitEventIdListWhenLocationUpdate();
	void RRProcessSwitchListWhenLocationUpdate();
	void RRProcessWaitEventIdList();

	void RRTransimitBegin();//��ʼ����(���Ǹ���ScheduleTable)

	void RRWriteScheduleInfo(std::ofstream& out);//��¼������Ϣ��־
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx);//��ʱ��Ϊ��λ��¼��־
	void RRDelaystatistics();//ʱ��ͳ��

	void RRTransimitEnd();//�������(���Ǹ���ScheduleTable)
};



inline
void RSUAdapterRR::RRPushToAdmitEventIdList(int eventId) {
	m_RRAdmitEventIdList.push_back(eventId);
}

inline
void RSUAdapterRR::RRPushToWaitEventIdList(int eventId, eMessageType messageType) {
	if (messageType == EMERGENCY) {
		m_RRWaitEventIdList.insert(m_RRWaitEventIdList.begin(), eventId);
	}
	else if (messageType == PERIOD) {
		m_RRWaitEventIdList.push_back(eventId);
	}
}

inline
void RSUAdapterRR::RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}