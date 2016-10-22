#pragma once
#include<vector>
#include<random>
#include"RRM.h"
#include"RSU.h"
#include"VUE.h"
#include"Enumeration.h"
#include"Exception.h"
#include"WT.h"
#include"GTAT.h"

//RRM_DRA:Radio Resource Management Distributed Resource Allocation





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
		WT_Basic* systemWTPoint,
		GTAT_Basic* systemGTATPoint
	);

	std::vector<VeUEAdapterDRA> m_VeUEAdapterVec;


	/*------------------���ݳ�Ա------------------*/

	eDRAMode m_DRAMode;//��Դ��ѡ��Ĳ���
	GTAT_Basic* m_GTATPoint;//�������˵�Ԫģ��ָ��
	WT_Basic* m_WTPoint;//���ߴ��䵥Ԫģ��ָ��
	std::list<int> m_DRASwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������

	/*
	* ���ڴ��ָ��Pattern�ĳ����ı���б�������Pattern���
	* ����±���Pattern��ע��ǽ����¼�Ҫ����ƫ��������Ϊ�ǽ����¼���PatternҲ�Ǵ�0��ʼ��ţ���������Ҫ�������Pattern��ŵľ���ֵ
	*/
	std::vector<std::list<int>> m_DRAInterferenceVec;

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

	void DRATransimitPreparation();//ͳ�Ƹ�����Ϣ
	void DRATransimitStart();//ģ�⴫�俪ʼ�����µ�����Ϣ
	void DRATransimitEnd();//ģ�⴫���������ͳ��������

	//��־��¼����
	void DRAWriteScheduleInfo(std::ofstream& out);//��¼������Ϣ��־
	void DRAWriteTTILogInfo(std::ofstream& out, int TTI, eEventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx);
	void DRAWriteClusterPerformInfo(std::ofstream &out);//д��ִ���Ϣ����־


	//���ߺ���
	int DRAGetMaxIndex(const std::vector<double>&clusterSize);
	int DRAGetPatternType(int patternIdx);
	std::pair<int, int> DRAGetOccupiedSubCarrierRange(eMessageType messageType, int patternIdx);
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


