#pragma once
#include<vector>
#include<random>
#include"RRM.h"
#include"RSU.h"
#include"VUE.h"
#include"Enumeration.h"
#include"Exception.h"
#include"WT.h"

//RRM_DRA:Radio Resource Management Distributed Resource Allocation

/*===========================================
*              ģ�鳣������
* ==========================================*/
const int gc_DRA_NTTI = 100; //���дؽ���һ��DRA��ռ�õ�TTI������(NTTI:Number of TTI)

const int gc_DRAEmergencyTotalPatternNum = 3;//��������¼���Pattern����
const int gc_DRAEmergencyRBNumPerPattern = 2;//ÿ�������¼���Patternռ�õ�RB����

const int gc_DRAPatternTypeNum = 2;//�ǽ����¼���Pattern����������
const int gc_DRA_RBNumPerPatternType[gc_DRAPatternTypeNum] = { 5,10 };//ÿ��Pattern������ռ��RB����
const int gc_DRAPatternNumPerPatternType[gc_DRAPatternTypeNum] = { 6,2 };//��ȫƵ��ÿ��Pattern�����Ӧ��Pattern����
const int gc_DRAPatternTypePatternIdxInterval[gc_DRAPatternTypeNum][2] = { {0,5},{6,7} };

const int gc_DRATotalPatternNum = [&]() {
	int res = 0;
	for (int num : gc_DRAPatternNumPerPatternType)
		res += num;
	return res;
}();//���з�EmergencyPattern���͵�Pattern�����ܺ�



/*===========================================
*        ���ڸ�ģ���ö�����Ͷ���
* ==========================================*/
enum eDRAMode {
	//P1:  Collision avoidance based on sensing
	//P2:  Enhanced random resource selection
	//P3:  Location-based resource selection
	P13,               //Combination of P1 and P3
	P23,               //Combination of P2 and P3
	P123               //Combination of P1 and P2 and P3
};



/*===========================================
*            ������Ϣ���ݽṹ
* ==========================================*/
struct sDRAScheduleInfo {
	int eventId;//�¼����
	int VeUEId;//�������
	int RSUId;//RSU���
	int patternIdx;//Ƶ�����
	int remainBitNum=-1;//ʣ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
	int transimitBitNum=-1;//��ǰ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
	int occupiedTTINum = -1;//�����껹��ռ�õ�TTI����

	sDRAScheduleInfo() {}
	sDRAScheduleInfo(int eventId, int VeUEId, int RSUId, int patternIdx) {
		this->eventId = eventId;
		this->VeUEId = VeUEId;
		this->RSUId = RSUId;
		this->patternIdx = patternIdx;
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
class RSUAdapterDRA {
public:
	cRSU& m_HoldObj;//�����������е�ԭRSU����
	RSUAdapterDRA() = delete;
	RSUAdapterDRA(cRSU& _RSU);

	/*------���ݳ�Ա(��������ģ�����ݳ�Ա)--------*/

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


	/*------------------��Ա����------------------*/

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


/*===========================================
*                VeUE������
* ==========================================*/
class VeUEAdapterDRA {
public:
	static std::default_random_engine s_Engine;

	cVeUE& m_HoldObj;//�����������е�ԭVeUE����
	VeUEAdapterDRA() = delete;
	VeUEAdapterDRA(cVeUE& _VeUE) :m_HoldObj(_VeUE) {}

	/*------���ݳ�Ա(��������ģ�����ݳ�Ա)--------*/

	std::tuple<int, int> m_ScheduleInterval;//��VeUE���ڴصĵ�ǰһ�ֵ�������


	/*------------------��Ա����------------------*/

	int DRARBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType);
	int DRARBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx);

	std::string toString(int n);//���ڴ�ӡVeUE��Ϣ
};




/*===========================================
*                DRAģ��
* ==========================================*/
class RRM_DRA :public RRM_Basic {
public:
	RRM_DRA() = delete;
	RRM_DRA(int &systemTTI, 
		sConfigure& systemConfig, 
		cRSU* systemRSUAry, 
		cVeUE* systemVeUEAry, 
		std::vector<sEvent>& systemEventVec, 
		std::vector<std::list<int>>& systemEventTTIList,
		std::vector<std::vector<int>>& systemTTIRSUThroughput,
		eDRAMode systemDRAMode,
		WT_Basic* systemWTPoint);

	std::vector<RSUAdapterDRA> m_RSUAdapterVec;
	std::vector<VeUEAdapterDRA> m_VeUEAdapterVec;


	/*------------------���ݳ�Ա------------------*/

	eDRAMode m_DRAMode;//��Դ��ѡ��Ĳ���
	WT_Basic* m_WTPoint;
	std::list<int> m_DRASwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������

	int m_NewCount = 0;//��¼��̬�����Ķ���Ĵ���

	int m_DeleteCount = 0;//��¼ɾ����̬��������Ĵ���

	/*------------------��Ա����------------------*/

public:
	/*�ӿں���*/
	void schedule() override;//DRA�����ܿأ����ǻ�����麯��

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


	void DRADelaystatistics();//ʱ��ͳ��
	void DRAConflictListener();//֡����ͻ

	void DRATransimitStart();//ģ�⴫�俪ʼ�����µ�����Ϣ
	void DRATransimitEnd();//ģ�⴫���������ͳ��������

	//��־��¼����
	void DRAWriteScheduleInfo(std::ofstream& out);//��¼������Ϣ��־
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, eEventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx);
	void DRAWriteClusterPerformInfo(std::ofstream &out);//д��ִ���Ϣ����־


	//���ߺ���
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);
	int getPatternType(int patternIdx);
	std::pair<int, int> DRAGetOccupiedRBRange(eMessageType messageType, int patternIdx);
};



inline
int VeUEAdapterDRA::DRARBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType) {
	int size = static_cast<int>(curAvaliablePatternIdx[messageType].size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return curAvaliablePatternIdx[messageType][u(s_Engine)];
}

inline
int VeUEAdapterDRA::DRARBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx) {
	int size = static_cast<int>(curAvaliableEmergencyPatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return curAvaliableEmergencyPatternIdx[u(s_Engine)];
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