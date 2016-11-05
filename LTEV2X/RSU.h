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
	struct GTT;
	struct GTT_Urban;
	struct GTT_HighSpeed;
	struct RRM;
	struct RRM_TDM_DRA;
	struct RRM_ICC_DRA;
	struct RRM_RR;
	struct WT;
	struct TMC;


	//���ڽṹ��ָ�룬ֻ����ָ����ʽ����Ϊ����ǰ�У��ṹ��Ķ�����δ���֣�ֻ�ܶ��岻��������
	GTT* m_GTT = nullptr;//���ڴ洢������ģ��ʹ�õĲ���
	GTT_Urban* m_GTT_Urban = nullptr;//���ڴ洢���򳡾����ض�����
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;//���ڴ洢���ٳ������ض�����
	RRM* m_RRM = nullptr;
	RRM_TDM_DRA* m_RRM_TDM_DRA = nullptr;//���ڴ洢RRM_TDM_DRAģʽ���ض�����
	RRM_ICC_DRA* m_RRM_ICC_DRA = nullptr;//���ڴ洢RRM_ICC_DRAģʽ���ض�����
	RRM_RR* m_RRM_RR = nullptr;//���ڴ洢RRģʽ���ض�����
	WT* m_WT = nullptr;
	TMC* m_TMC = nullptr;


	RSU();
	void initializeGTT_Urban(RSUConfigure &t_RSUConfigure);
	void initializeGTT_HighSpeed(RSUConfigure &t_RSUConfigure);
	void initializeRRM_TDM_DRA();
	void initializeRRM_ICC_DRA();
	void initializeRRM_RR();
	void initializeWT();
	void initializeTMC();
	~RSU();

	//�������ݽṹ����
	struct GTT {
		int m_RSUId;
		std::list<int> m_VeUEIdList;//��ǰRSU��Χ�ڵ�VeUEId�������,RRM_TDM_DRAģ����Ҫ
		int m_ClusterNum;//һ��RSU���Ƿ�Χ�ڵĴصĸ���,RRM_TDM_DRAģ����Ҫ
		std::vector<std::list<int>> m_ClusterVeUEIdList;//���ÿ���ص�VeUE��Id������,�±����صı��
	};

	struct GTT_Urban {
		double m_AbsX;
		double m_AbsY;
		IMTA *m_IMTA;
		double m_FantennaAngle;
	};

	struct GTT_HighSpeed {
		double m_AbsX;
		double m_AbsY;
		IMTA *m_IMTA;
		double m_FantennaAngle;
	};

	struct RRM {

	};

	struct RRM_TDM_DRA {
		/*===========================================
		*            ������Ϣ���ݽṹ
		* ==========================================*/
		struct ScheduleInfo {
			int eventId;//�¼����
			int VeUEId;//�������
			int RSUId;//RSU���
			int patternIdx;//Ƶ�����
			int currentPackageIdx = -1;//��ǰ��������ݰ��ı��
			int remainBitNum = -1;//ʣ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
			int transimitBitNum = -1;//��ǰ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)

			ScheduleInfo() {}
			ScheduleInfo(int eventId, int VeUEId, int RSUId, int patternIdx) {
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

		RSU* m_This;//RRM_TDM_DRA���õ�GTT����ز�������C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ

		/*
		* TDR:Time Domain Resource
		* �±����ر��
		* tuple�洢�ı����ĺ�������Ϊ���洢ÿ����������ʱ�������������˵㣬�Ҷ˵��Լ����䳤��
		*/
		std::vector<std::tuple<int, int, int>> m_ClusterTDRInfo;


		/*
		* Pattern�����Ƿ���õı��
		* ����±����ر��
		* �ڲ��±����Pattern���(��Ա�ţ���ȥEmergency��Pattern����)
		* ��"m_PatternIsAvailable[i][j]==true"�����i��Pattern��j����
		*/
		std::vector<std::vector<bool>> m_PatternIsAvailable;

		/*
		* ���ڴ�ŵ�ǰTTI�Ľ����¼�����
		*/
		std::list<int> m_AdmitEventIdList;

		/*
		* RSU����ĵȴ��б�
		* ��ŵ���VeUEId
		* ����Դ�У�
		*		1���ִغ���System�����л�����ת���RSU����ĵȴ�����
		*		2���¼������е�ǰ���¼���������VeUEδ�ڿɷ�����Ϣ��ʱ�Σ�ת��ȴ�����
		*		3��VeUE�ڴ�����Ϣ�󣬷�����ͻ�������ͻ��ת��ȴ�����
		*/
		std::list<int> m_WaitEventIdList;

		/*
		* ��ŵ��ȵ�����Ϣ
		* ����±����ر��
		* �ڲ��±����Pattern���(��Ա�ţ���ȥEmergency��Pattern����)
		*/
		std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;

		/*
		* ��ǰʱ�̵�ǰRSU�ڴ��ڴ���״̬�ĵ�����Ϣ����
		* ����±����Pattern���(��Ա�ţ���ȥEmergency��Pattern����)
		* �ڲ���list���ڴ����ͻ
		*/
		std::vector<std::list<ScheduleInfo*>> m_TransimitScheduleInfoList;


		/*
		* Pattern�����Ƿ���õı��
		* �±����Pattern���(���Ա��)
		* ��"m_EmergencyPatternIsAvailable[j]==true"����Pattern��j����
		*/
		std::vector<bool> m_EmergencyPatternIsAvailable;

		/*
		* ��ǰʱ�̵ȴ�����Ľ����¼��б�
		*/
		std::list<int> m_EmergencyAdmitEventIdList;

		/*
		* ��ǰʱ�̴��ڵȴ�����״̬�Ľ����¼��б�
		*/
		std::list<int> m_EmergencyWaitEventIdList;

		/*
		* ��ǰʱ�̴��ڴ���״̬�Ľ����¼�������Ϣ�б�
		* ����±����pattern���(���Ա��)
		*/
		std::vector<std::list<ScheduleInfo*>> m_EmergencyTransimitScheduleInfoList;

		/*
		* ��ǰʱ�̴��ڵ���״̬�Ľ����¼�������Ϣ�б�
		* ����±����pattern���(���Ա��)
		*/
		std::vector<ScheduleInfo*> m_EmergencyScheduleInfoTable;

		RRM_TDM_DRA(RSU* t_This);//���캯��

		/*------------------��Ա����------------------*/

		std::string toString(int n);

		int getClusterIdx(int TTI);

		/*
		* ���ڲ���ָ��VeUEId�����Ĵر��
		* ��Ҫ������������ж�VeUE�Ƿ����ڸ�RSU��ֱ����VeUE�����RSU������жϼ��ɣ�
		* ���������ʹ��ǰ���ǣ���֪�������ڴ���
		*/
		int getClusterIdxOfVeUE(int VeUEId);

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
		void pushToSwitchEventIdList(int eventId, std::list<int>& t_SwitchVeUEIdList);

		/*
		* ��TransimitScheduleInfo����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToTransmitScheduleInfoList(RRM_TDM_DRA::ScheduleInfo* p, int patternIdx);
		void pushToEmergencyTransmitScheduleInfoList(RRM_TDM_DRA::ScheduleInfo* p, int patternIdx);

		/*
		* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToScheduleInfoTable(int clusterIdx, int patternIdx, RRM_TDM_DRA::ScheduleInfo*p);
		void pushToEmergencyScheduleInfoTable(int patternIdx, RRM_TDM_DRA::ScheduleInfo*p);

		/*
		* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
		*/
		void pullFromScheduleInfoTable(int TTI);
		void pullFromEmergencyScheduleInfoTable();
	};

	struct RRM_ICC_DRA {
		RSU* m_This;

		RRM_ICC_DRA(RSU* t_This);//���캯��

		/*
		* ���ڴ�ŵ�ǰTTI�Ľ����¼�����
		* ����±��Ǵر��
		*/
		std::vector<std::list<int>> m_AdmitEventIdList;

	};

	struct RRM_RR {
		/*===========================================
		*            ������Ϣ���ݽṹ
		* ==========================================*/
		struct ScheduleInfo {
			int eventId;//�¼����
			MessageType messageType;//�¼�����
			int VeUEId;//�������
			int RSUId;//RSU���
			int patternIdx;//Ƶ�����
			int currentPackageIdx = -1;//��ǰ��������ݰ��ı��
			int remainBitNum = -1;//ʣ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
			int transimitBitNum = -1;//��ǰ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)


			ScheduleInfo() {}
			ScheduleInfo(int eventId, MessageType messageType, int VeUEId, int RSUId, int patternIdx) {
				this->eventId = eventId;
				this->messageType = messageType;
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

		RSU* m_This;//RRM_TDM_DRA���õ�GTT����ز�������C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ

		/*
		* ��ǰTTI�����б�
		* ����±�Ϊ�ر��
		* �ڲ�list��󳤶Ȳ�����Pattern����
		*/
		std::vector<std::vector<int>> m_AdmitEventIdList;

		/*
		* RSU����ĵȴ��б�
		* ����±�Ϊ�ر��
		* �ڲ�list��ŵ��Ǵ��ڵȴ�����״̬��VeUEId
		* ����Դ�У�
		*		1���ִغ���System�����л�����ת���RSU����ĵȴ�����
		*		2���¼������е�ǰ���¼�������ת��ȴ�����
		*/
		std::vector<std::list<int>> m_WaitEventIdList;

		/*
		* ��ŵ��ȵ�����Ϣ�����ν��д���
		* ������Pattern��
		*/
		std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;


		RRM_RR(RSU* t_this);//���캯��
		/*------------------��Ա����------------------*/

		/*
		* ��AdmitEventIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToAdmitEventIdList(int clusterIdx, int eventId);

		/*
		* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToWaitEventIdList(int clusterIdx, int eventId, MessageType messageType);

		/*
		* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToSwitchEventIdList(int eventId, std::list<int>& t_SwitchVeUEIdList);
	};

	struct WT {

	};

	struct TMC {

	};

};



inline
int RSU::RRM_TDM_DRA::getClusterIdxOfVeUE(int VeUEId) {
	int dex = -1;
	for (int clusterIdx = 0; clusterIdx < m_This->m_GTT->m_ClusterNum; clusterIdx++) {
		for (int Id : m_This->m_GTT->m_ClusterVeUEIdList[clusterIdx])
			if (Id == VeUEId) return clusterIdx;
	}
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)���ó����ڵ�ǰRSU��");
}


inline
void RSU::RRM_TDM_DRA::pushToAdmitEventIdList(int eventId) {
	m_AdmitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_TDM_DRA::pushToEmergencyAdmitEventIdList(int eventId) {
	m_EmergencyAdmitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_TDM_DRA::pushToWaitEventIdList(int eventId) {
	m_WaitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_TDM_DRA::pushToEmergencyWaitEventIdList(int eventId) {
	m_EmergencyWaitEventIdList.push_back(eventId);
}

inline
void RSU::RRM_TDM_DRA::pushToSwitchEventIdList(int VeUEId, std::list<int>& t_SwitchVeUEIdList) {
	t_SwitchVeUEIdList.push_back(VeUEId);
}

inline
void RSU::RRM_TDM_DRA::pushToTransmitScheduleInfoList(RRM_TDM_DRA::ScheduleInfo* p, int patternIdx) {
	int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
	m_TransimitScheduleInfoList[relativePatternIdx].push_back(p);
}

inline
void RSU::RRM_TDM_DRA::pushToEmergencyTransmitScheduleInfoList(RRM_TDM_DRA::ScheduleInfo* p, int patternIdx) {
	m_EmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void RSU::RRM_TDM_DRA::pushToScheduleInfoTable(int clusterIdx, int patternIdx, RRM_TDM_DRA::ScheduleInfo*p) {
	int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
	m_ScheduleInfoTable[clusterIdx][relativePatternIdx] = p;
}

inline
void RSU::RRM_TDM_DRA::pushToEmergencyScheduleInfoTable(int patternIdx, RRM_TDM_DRA::ScheduleInfo*p) {
	m_EmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void RSU::RRM_TDM_DRA::pullFromScheduleInfoTable(int TTI) {
	int clusterIdx = getClusterIdx(TTI);
	/*�����ڵ��ȱ��е�ǰ���Դ������Ϣѹ��m_TransimitEventIdList*/
	for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
		int relativePatternIdx = patternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
		if (m_ScheduleInfoTable[clusterIdx][relativePatternIdx] != nullptr) {
			m_TransimitScheduleInfoList[relativePatternIdx].push_back(m_ScheduleInfoTable[clusterIdx][relativePatternIdx]);
			m_ScheduleInfoTable[clusterIdx][relativePatternIdx] = nullptr;
		}
	}
}

inline
void RSU::RRM_TDM_DRA::pullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
		if (m_EmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_EmergencyTransimitScheduleInfoList[patternIdx].push_back(m_EmergencyScheduleInfoTable[patternIdx]);
			m_EmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}











inline
void RSU::RRM_RR::pushToAdmitEventIdList(int clusterIdx, int eventId) {
	m_AdmitEventIdList[clusterIdx].push_back(eventId);
}

inline
void RSU::RRM_RR::pushToWaitEventIdList(int clusterIdx, int eventId, MessageType messageType) {
	if (messageType == EMERGENCY) {
		m_WaitEventIdList[clusterIdx].insert(m_WaitEventIdList[clusterIdx].begin(), eventId);
	}
	else {
		m_WaitEventIdList[clusterIdx].push_back(eventId);
	}
}

inline
void RSU::RRM_RR::pushToSwitchEventIdList(int eventId, std::list<int>& t_SwitchVeUEIdList) {
	t_SwitchVeUEIdList.push_back(eventId);
}