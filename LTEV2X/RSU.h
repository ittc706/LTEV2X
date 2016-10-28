#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include<tuple>
#include"Global.h"
#include"VUE.h"
#include"Event.h"
#include"IMTA.h"
#include"Exception.h"

class RSU {
public:
	//����Ƕ�׽ṹ��ǰ������
	struct GTAT;
	struct GTAT_Urban;
	struct GTAT_HighSpeed;
	struct RRM;
	struct RRM_DRA;
	struct RRM_RR;
	struct WT;
	struct TMAC;


	//���ڽṹ��ָ�룬ֻ����ָ����ʽ����Ϊ����ǰ�У��ṹ��Ķ�����δ���֣�ֻ�ܶ��岻��������
	GTAT* m_GTAT = nullptr;//���ڴ洢������ģ��ʹ�õĲ���
	GTAT_Urban* m_GTAT_Urban = nullptr;//���ڴ洢���򳡾����ض�����
	GTAT_HighSpeed* m_GTAT_HighSpeed = nullptr;//���ڴ洢���ٳ������ض�����
	RRM* m_RRM = nullptr;
	RRM_DRA* m_RRM_DRA = nullptr;//���ڴ洢DRAģʽ���ض�����
	RRM_RR* m_RRM_RR = nullptr;//���ڴ洢RRģʽ���ض�����
	WT* m_WT = nullptr;
	TMAC* m_TMAC = nullptr;


	RSU();
	void initializeUrban(RSUConfigure &t_RSUConfigure);
	void initializeHighSpeed(RSUConfigure &t_RSUConfigure);
	void initializeElse();//��������ģ��ĳ�Ա��ʼ������������GTATģ�飬��˳�ʼ��GTAT��Ϻ��ٵ��øú���
	~RSU();

	//�������ݽṹ����
	struct GTAT {
		int m_RSUId;
		std::list<int> m_VeUEIdList;//��ǰRSU��Χ�ڵ�VeUEId�������,RRM_DRAģ����Ҫ
		int m_DRAClusterNum;//һ��RSU���Ƿ�Χ�ڵĴصĸ���,RRM_DRAģ����Ҫ
		std::vector<std::list<int>> m_DRAClusterVeUEIdList;//���ÿ���ص�VeUE��Id������,�±����صı��
	};

	struct GTAT_Urban {
		double m_AbsX;
		double m_AbsY;
		IMTA *m_IMTA;
		double m_FantennaAngle;
	};

	struct GTAT_HighSpeed {
		double m_AbsX;
		double m_AbsY;
		IMTA *m_IMTA;
		double m_FantennaAngle;
	};

	struct RRM {

	};

	struct RRM_DRA {
		/*===========================================
		*            ������Ϣ���ݽṹ
		* ==========================================*/
		struct DRAScheduleInfo {
			int eventId;//�¼����
			int VeUEId;//�������
			int RSUId;//RSU���
			int patternIdx;//Ƶ�����
			int remainBitNum = -1;//ʣ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
			int transimitBitNum = -1;//��ǰ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
			int occupiedTTINum = -1;//�����껹��ռ�õ�TTI����

			DRAScheduleInfo() {}
			DRAScheduleInfo(int eventId, int VeUEId, int RSUId, int patternIdx) {
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


		RSU* m_this;//RRM_DRA���õ�GTAT����ز�������C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ


		/*
		* TDR:Time Domain Resource
		* �±����ر��
		* tuple�洢�ı����ĺ�������Ϊ���洢ÿ����������ʱ�������������˵㣬�Ҷ˵��Լ����䳤��
		*/
		std::vector<std::tuple<int, int, int>> m_DRAClusterTDRInfo;


		/*
		* Pattern�����Ƿ���õı��
		* ����±����ر��
		* �ڲ��±����Pattern���(��Ա�ţ���ȥEmergency��Pattern����)
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
		* �ڲ��±����Pattern���(��Ա�ţ���ȥEmergency��Pattern����)
		*/
		std::vector<std::vector<DRAScheduleInfo*>> m_DRAScheduleInfoTable;

		/*
		* ��ǰʱ�̵�ǰRSU�ڴ��ڴ���״̬�ĵ�����Ϣ����
		* ����±����Pattern���(��Ա�ţ���ȥEmergency��Pattern����)
		* �ڲ���list���ڴ����ͻ
		*/
		std::vector<std::list<DRAScheduleInfo*>> m_DRATransimitScheduleInfoList;


		/*
		* Pattern�����Ƿ���õı��
		* �±����Pattern���(���Ա��)
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
		* ����±����pattern���(���Ա��)
		*/
		std::vector<std::list<DRAScheduleInfo*>> m_DRAEmergencyTransimitScheduleInfoList;

		/*
		* ��ǰʱ�̴��ڵ���״̬�Ľ����¼�������Ϣ�б�
		* ����±����pattern���(���Ա��)
		*/
		std::vector<DRAScheduleInfo*> m_DRAEmergencyScheduleInfoTable;

		RRM_DRA(RSU* t_this);//���캯��

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
		void DRAPushToTransmitScheduleInfoList(RRM_DRA::DRAScheduleInfo* p, int patternIdx);
		void DRAPushToEmergencyTransmitScheduleInfoList(RRM_DRA::DRAScheduleInfo* p, int patternIdx);

		/*
		* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, RRM_DRA::DRAScheduleInfo*p);
		void DRAPushToEmergencyScheduleInfoTable(int patternIdx, RRM_DRA::DRAScheduleInfo*p);

		/*
		* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
		*/
		void DRAPullFromScheduleInfoTable(int TTI);
		void DRAPullFromEmergencyScheduleInfoTable();
	};

	struct RRM_RR {
		/*===========================================
		*            ������Ϣ���ݽṹ
		* ==========================================*/
		struct RRScheduleInfo {
			int eventId;//�¼����
			MessageType messageType;//�¼�����
			int VeUEId;//�������
			int RSUId;//RSU���
			int patternIdx;//Ƶ�����
			int occupiedNumTTI;//��ǰVeUE���д����ʵ��TTI���䣨�����䣩

			RRScheduleInfo() {}
			RRScheduleInfo(int eventId, MessageType messageType, int VeUEId, int RSUId, int patternIdx, const int &occupiedNumTTI) {
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
		std::vector<RRScheduleInfo*> m_RRScheduleInfoTable;


		/*------------------��Ա����------------------*/

		/*
		* ��AdmitEventIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void RRPushToAdmitEventIdList(int eventId);

		/*
		* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void RRPushToWaitEventIdList(int eventId, MessageType messageType);

		/*
		* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList);

		RRM_RR();
	};

	struct WT {

	};

	struct TMAC {

	};

};



inline
int RSU::RRM_DRA::DRAGetClusterIdxOfVeUE(int VeUEId) {
	int dex = -1;
	for (int clusterIdx = 0; clusterIdx < m_this->m_GTAT->m_DRAClusterNum; clusterIdx++) {
		for (int Id : m_this->m_GTAT->m_DRAClusterVeUEIdList[clusterIdx])
			if (Id == VeUEId) return clusterIdx;
	}
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)���ó����ڵ�ǰRSU��");
}


inline
void RSU::RRM_DRA::DRAPushToAdmitEventIdList(int eventId) {
	m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_DRA::DRAPushToEmergencyAdmitEventIdList(int eventId) {
	m_DRAEmergencyAdmitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_DRA::DRAPushToWaitEventIdList(int eventId) {
	m_DRAWaitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_DRA::DRAPushToEmergencyWaitEventIdList(int eventId) {
	m_DRAEmergencyWaitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_DRA::DRAPushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void RSU::RRM_DRA::DRAPushToTransmitScheduleInfoList(RRM_DRA::DRAScheduleInfo* p, int patternIdx) {
	int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
	m_DRATransimitScheduleInfoList[relativePatternIdx].push_back(p);
}

inline
void RSU::RRM_DRA::DRAPushToEmergencyTransmitScheduleInfoList(RRM_DRA::DRAScheduleInfo* p, int patternIdx) {
	m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void RSU::RRM_DRA::DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, RRM_DRA::DRAScheduleInfo*p) {
	int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
	m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx] = p;
}

inline
void RSU::RRM_DRA::DRAPushToEmergencyScheduleInfoTable(int patternIdx, RRM_DRA::DRAScheduleInfo*p) {
	m_DRAEmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void RSU::RRM_DRA::DRAPullFromScheduleInfoTable(int TTI) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	/*�����ڵ��ȱ��е�ǰ���Դ������Ϣѹ��m_DRATransimitEventIdList*/
	for (int patternIdx = gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
		int relativePatternIdx = patternIdx - gc_DRAPatternNumPerPatternType[EMERGENCY];
		if (m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx] != nullptr) {
			m_DRATransimitScheduleInfoList[relativePatternIdx].push_back(m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx]);
			m_DRAScheduleInfoTable[clusterIdx][relativePatternIdx] = nullptr;
		}
	}
}

inline
void RSU::RRM_DRA::DRAPullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0; patternIdx < gc_DRAPatternNumPerPatternType[EMERGENCY]; patternIdx++) {
		if (m_DRAEmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(m_DRAEmergencyScheduleInfoTable[patternIdx]);
			m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}











inline
void RSU::RRM_RR::RRPushToAdmitEventIdList(int eventId) {
	m_RRAdmitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_RR::RRPushToWaitEventIdList(int eventId, MessageType messageType) {
	if (messageType == EMERGENCY) {
		m_RRWaitEventIdList.insert(m_RRWaitEventIdList.begin(), eventId);
	}
	else {
		m_RRWaitEventIdList.push_back(eventId);
	}
}

inline
void RSU::RRM_RR::RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}