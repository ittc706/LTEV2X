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
	struct GTATUrban;
	struct GTATHighSpeed;
	struct RRM;
	struct RRMDRA;
	struct RRMRR;
	struct WT;
	struct TMAC;


	//���ڽṹ��ָ�룬ֻ����ָ����ʽ����Ϊ����ǰ�У��ṹ��Ķ�����δ���֣�ֻ�ܶ��岻��������
	GTAT* m_GTAT = nullptr;//���ڴ洢������ģ��ʹ�õĲ���
	GTATUrban* m_GTATUrban = nullptr;//���ڴ洢���򳡾����ض�����
	GTATHighSpeed* m_GTATHighSpeed = nullptr;//���ڴ洢���ٳ������ض�����
	RRM* m_RRM = nullptr;
	RRMDRA* m_RRMDRA = nullptr;//���ڴ洢DRAģʽ���ض�����
	RRMRR* m_RRMRR = nullptr;//���ڴ洢RRģʽ���ض�����
	WT* m_WT = nullptr;
	TMAC* m_TMAC = nullptr;


	RSU();
	void initializeUrban(sRSUConfigure &t_RSUConfigure);
	void initializeHighSpeed(sRSUConfigure &t_RSUConfigure);
	void initializeElse();//��������ģ��ĳ�Ա��ʼ������������GTATģ�飬��˳�ʼ��GTAT��Ϻ��ٵ��øú���
	~RSU();

	//�������ݽṹ����
	struct GTAT {
		int m_RSUId;
		std::list<int> m_VeUEIdList;//��ǰRSU��Χ�ڵ�VeUEId�������,RRM_DRAģ����Ҫ
		int m_DRAClusterNum;//һ��RSU���Ƿ�Χ�ڵĴصĸ���,RRM_DRAģ����Ҫ
		std::vector<std::list<int>> m_DRAClusterVeUEIdList;//���ÿ���ص�VeUE��Id������,�±����صı��
	};

	struct GTATUrban {
		double m_AbsX;
		double m_AbsY;
		cIMTA *m_IMTA;
		double m_FantennaAngle;
	};

	struct GTATHighSpeed {
		double m_AbsX;
		double m_AbsY;
		cIMTA *m_IMTA;
		double m_FantennaAngle;
	};

	struct RRM {

	};

	struct RRMDRA {
		/*===========================================
		*            ������Ϣ���ݽṹ
		* ==========================================*/
		struct sDRAScheduleInfo {
			int eventId;//�¼����
			int VeUEId;//�������
			int RSUId;//RSU���
			int patternIdx;//Ƶ�����
			int remainBitNum = -1;//ʣ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
			int transimitBitNum = -1;//��ǰ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
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


		RSU* m_this;//RRMDRA���õ�GTAT����ز�������C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ


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

		RRMDRA(RSU* t_this);//���캯��

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
		void DRAPushToTransmitScheduleInfoList(RRMDRA::sDRAScheduleInfo* p, int patternIdx);
		void DRAPushToEmergencyTransmitScheduleInfoList(RRMDRA::sDRAScheduleInfo* p, int patternIdx);

		/*
		* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, RRMDRA::sDRAScheduleInfo*p);
		void DRAPushToEmergencyScheduleInfoTable(int patternIdx, RRMDRA::sDRAScheduleInfo*p);

		/*
		* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
		*/
		void DRAPullFromScheduleInfoTable(int TTI);
		void DRAPullFromEmergencyScheduleInfoTable();
	};

	struct RRMRR {
		/*===========================================
		*            ������Ϣ���ݽṹ
		* ==========================================*/
		struct sRRScheduleInfo {
			int eventId;//�¼����
			MessageType messageType;//�¼�����
			int VeUEId;//�������
			int RSUId;//RSU���
			int patternIdx;//Ƶ�����
			int occupiedNumTTI;//��ǰVeUE���д����ʵ��TTI���䣨�����䣩

			sRRScheduleInfo() {}
			sRRScheduleInfo(int eventId, MessageType messageType, int VeUEId, int RSUId, int patternIdx, const int &occupiedNumTTI) {
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
		std::vector<sRRScheduleInfo*> m_RRScheduleInfoTable;


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

		RRMRR();
	};

	struct WT {

	};

	struct TMAC {

	};

};



inline
int RSU::RRMDRA::DRAGetClusterIdxOfVeUE(int VeUEId) {
	int dex = -1;
	for (int clusterIdx = 0; clusterIdx < m_this->m_GTAT->m_DRAClusterNum; clusterIdx++) {
		for (int Id : m_this->m_GTAT->m_DRAClusterVeUEIdList[clusterIdx])
			if (Id == VeUEId) return clusterIdx;
	}
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)���ó����ڵ�ǰRSU��");
}


inline
void RSU::RRMDRA::DRAPushToAdmitEventIdList(int eventId) {
	m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void RSU::RRMDRA::DRAPushToEmergencyAdmitEventIdList(int eventId) {
	m_DRAEmergencyAdmitEventIdList.push_back(eventId);
}

inline
void RSU::RRMDRA::DRAPushToWaitEventIdList(int eventId) {
	m_DRAWaitEventIdList.push_back(eventId);
}

inline
void RSU::RRMDRA::DRAPushToEmergencyWaitEventIdList(int eventId) {
	m_DRAEmergencyWaitEventIdList.push_back(eventId);
}

inline
void RSU::RRMDRA::DRAPushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void RSU::RRMDRA::DRAPushToTransmitScheduleInfoList(RRMDRA::sDRAScheduleInfo* p, int patternIdx) {
	m_DRATransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void RSU::RRMDRA::DRAPushToEmergencyTransmitScheduleInfoList(RRMDRA::sDRAScheduleInfo* p, int patternIdx) {
	m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void RSU::RRMDRA::DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, RRMDRA::sDRAScheduleInfo*p) {
	m_DRAScheduleInfoTable[clusterIdx][patternIdx] = p;
}

inline
void RSU::RRMDRA::DRAPushToEmergencyScheduleInfoTable(int patternIdx, RRMDRA::sDRAScheduleInfo*p) {
	m_DRAEmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void RSU::RRMDRA::DRAPullFromScheduleInfoTable(int TTI) {
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
void RSU::RRMDRA::DRAPullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
		if (m_DRAEmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(m_DRAEmergencyScheduleInfoTable[patternIdx]);
			m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}











inline
void RSU::RRMRR::RRPushToAdmitEventIdList(int eventId) {
	m_RRAdmitEventIdList.push_back(eventId);
}

inline
void RSU::RRMRR::RRPushToWaitEventIdList(int eventId, MessageType messageType) {
	if (messageType == EMERGENCY) {
		m_RRWaitEventIdList.insert(m_RRWaitEventIdList.begin(), eventId);
	}
	else {
		m_RRWaitEventIdList.push_back(eventId);
	}
}

inline
void RSU::RRMRR::RRPushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}