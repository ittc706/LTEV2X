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
		~GTT_Urban();
		double m_AbsX;
		double m_AbsY;
		IMTA *m_IMTA = nullptr;
		double m_FantennaAngle;
	};

	struct GTT_HighSpeed {
		~GTT_HighSpeed();
		double m_AbsX;
		double m_AbsY;
		IMTA *m_IMTA = nullptr;
		double m_FantennaAngle;
	};

	struct RRM {
		/*===========================================
		*            ������Ϣ���ݽṹ
		* ==========================================*/
		class ScheduleInfo {
		public:
			int eventId;//�¼����
			int VeUEId;//�������
			int RSUId;//RSU���
			int clusterIdx;//�ر��
			int patternIdx;//Ƶ�����
			int currentPackageIdx = -1;//��ǰ��������ݰ��ı��
			int remainBitNum = -1;//ʣ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
			int transimitBitNum = -1;//��ǰ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)

			ScheduleInfo() {}
			ScheduleInfo(int t_EventId, int t_VeUEId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx) {
				this->eventId = t_EventId;
				this->VeUEId = t_VeUEId;
				this->RSUId = t_RSUId;
				this->clusterIdx = t_ClusterIdx;
				this->patternIdx = t_PatternIdx;
			}

			std::string toLogString();

			/*
			* ���ɱ�ʾ������Ϣ��string����
			* �����¼���Id��������Id���Լ�Ҫ������¼���ռ�õ�TTI����
			*/
			std::string toScheduleString(int t_NumTab);
		};
	};

	struct RRM_TDM_DRA {
		

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
		std::vector<std::vector<RSU::RRM::ScheduleInfo*>> m_ScheduleInfoTable;

		/*
		* ��ǰʱ�̵�ǰRSU�ڴ��ڴ���״̬�ĵ�����Ϣ����
		* ����±����Pattern���(��Ա�ţ���ȥEmergency��Pattern����)
		* �ڲ���list���ڴ����ͻ
		*/
		std::vector<std::list<RSU::RRM::ScheduleInfo*>> m_TransimitScheduleInfoList;


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
		std::vector<std::list<RSU::RRM::ScheduleInfo*>> m_EmergencyTransimitScheduleInfoList;

		/*
		* ��ǰʱ�̴��ڵ���״̬�Ľ����¼�������Ϣ�б�
		* ����±����pattern���(���Ա��)
		*/
		std::vector<RSU::RRM::ScheduleInfo*> m_EmergencyScheduleInfoTable;

		RRM_TDM_DRA(RSU* t_This);//���캯��

		/*------------------��Ա����------------------*/

		std::string toString(int t_NumTab);

		int getClusterIdx(int t_TTI);


		/*
		* ��AdmitEventIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToAdmitEventIdList(int t_EventId);
		void pushToEmergencyAdmitEventIdList(int t_EventId);

		/*
		* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToWaitEventIdList(int t_EventId);
		void pushToEmergencyWaitEventIdList(int t_EventId);

		/*
		* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList);

		/*
		* ��TransimitScheduleInfo����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToTransmitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info, int t_PatternIdx);
		void pushToEmergencyTransmitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info, int t_PatternIdx);

		/*
		* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToScheduleInfoTable(int t_ClusterIdx, int t_PatternIdx, RSU::RRM::ScheduleInfo* t_Info);
		void pushToEmergencyScheduleInfoTable(int t_PatternIdx, RSU::RRM::ScheduleInfo* t_Info);

		/*
		* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
		*/
		void pullFromScheduleInfoTable(int t_TTI);
		void pullFromEmergencyScheduleInfoTable();
	};

	struct RRM_ICC_DRA {
		RSU* m_This;

		RRM_ICC_DRA(RSU* t_This);//���캯��

		
		/*
		* RSU����ĵȴ��б�
		* ����±�Ϊ�ر��
		* �ڲ�list��ŵ��Ƕ�Ӧ�ص�VeUEId
		* ����Դ�У�
		*		1���ִغ���System�����л�����ת���RSU����ĵȴ�����
		*		2���¼������е�ǰ���¼�����
		*		3��VeUE�ڴ�����Ϣ�󣬷�����ͻ�������ͻ��ת��ȴ�����
		*/
		std::vector<std::list<int>> m_WaitEventIdList;

		/*
		* Pattern�����Ƿ���õı��
		* ����±����ر��
		* �ڲ��±����Pattern���
		* ��"m_PatternIsAvailable[i][j]==true"�����i��Pattern��j����
		*/
		std::vector<std::vector<bool>> m_PatternIsAvailable;


		/*
		* ��ŵ��ȵ�����Ϣ
		* ����±����ر��
		* �ڲ��±����Pattern���
		*/
		std::vector<std::vector<RSU::RRM::ScheduleInfo*>> m_ScheduleInfoTable;


		/*
		* ��ǰʱ�̵�ǰRSU�ڴ��ڴ���״̬�ĵ�����Ϣ����
		* ����±����ر��
		* �ڲ��±����Pattern���
		* ���ڲ���list���ڴ����ͻ
		*/
		std::vector<std::vector<std::list<RSU::RRM::ScheduleInfo*>>> m_TransimitScheduleInfoList;


		std::string toString(int t_NumTab);
		/*
		* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToWaitEventIdList(int t_ClusterIdx, int t_EventId);

		/*
		* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

		/*
		* ��TransimitScheduleInfo����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToTransmitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info);
	};

	struct RRM_RR {
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
		std::vector<std::vector<RSU::RRM::ScheduleInfo*>> m_ScheduleInfoTable;


		RRM_RR(RSU* t_This);//���캯��
		/*------------------��Ա����------------------*/

		/*
		* ��AdmitEventIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToAdmitEventIdList(int t_ClusterIdx, int t_EventId);

		/*
		* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToWaitEventIdList(int t_ClusterIdx, int t_EventId, MessageType t_MessageType);

		/*
		* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList);
	};

	struct WT {

	};

	struct TMC {

	};

};


inline
void RSU::RRM_TDM_DRA::pushToAdmitEventIdList(int t_EventId) {
	m_AdmitEventIdList.push_back(t_EventId);
}

inline
void RSU::RRM_TDM_DRA::pushToEmergencyAdmitEventIdList(int t_EventId) {
	m_EmergencyAdmitEventIdList.push_back(t_EventId);
}

inline
void RSU::RRM_TDM_DRA::pushToWaitEventIdList(int t_EventId) {
	m_WaitEventIdList.push_back(t_EventId);
}

inline
void RSU::RRM_TDM_DRA::pushToEmergencyWaitEventIdList(int t_EventId) {
	m_EmergencyWaitEventIdList.push_back(t_EventId);
}

inline
void RSU::RRM_TDM_DRA::pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}

inline
void RSU::RRM_TDM_DRA::pushToTransmitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info, int t_PatternIdx) {
	int relativePatternIdx = t_PatternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
	m_TransimitScheduleInfoList[relativePatternIdx].push_back(t_Info);
}

inline
void RSU::RRM_TDM_DRA::pushToEmergencyTransmitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info, int t_PatternIdx) {
	m_EmergencyTransimitScheduleInfoList[t_PatternIdx].push_back(t_Info);
}

inline
void RSU::RRM_TDM_DRA::pushToScheduleInfoTable(int t_ClusterIdx, int t_PatternIdx, RSU::RRM::ScheduleInfo* t_Info) {
	int relativePatternIdx = t_PatternIdx - ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY];
	m_ScheduleInfoTable[t_ClusterIdx][relativePatternIdx] = t_Info;
}

inline
void RSU::RRM_TDM_DRA::pushToEmergencyScheduleInfoTable(int t_PatternIdx, RSU::RRM::ScheduleInfo* t_Info) {
	m_EmergencyScheduleInfoTable[t_PatternIdx] = t_Info;
}

inline
void RSU::RRM_TDM_DRA::pullFromScheduleInfoTable(int t_TTI) {
	int clusterIdx = getClusterIdx(t_TTI);
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
void RSU::RRM_ICC_DRA::pushToWaitEventIdList(int t_ClusterIdx, int t_EventId) {
	m_WaitEventIdList[t_ClusterIdx].push_back(t_EventId);
}

inline
void RSU::RRM_ICC_DRA::pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}

inline
void RSU::RRM_ICC_DRA::pushToTransmitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info) {
	m_TransimitScheduleInfoList[t_Info->clusterIdx][t_Info->patternIdx].push_back(t_Info);
}



inline
void RSU::RRM_RR::pushToAdmitEventIdList(int t_ClusterIdx, int t_EventId) {
	m_AdmitEventIdList[t_ClusterIdx].push_back(t_EventId);
}

inline
void RSU::RRM_RR::pushToWaitEventIdList(int t_ClusterIdx, int t_EventId, MessageType t_MessageType) {
	if (t_MessageType == EMERGENCY) {
		m_WaitEventIdList[t_ClusterIdx].insert(m_WaitEventIdList[t_ClusterIdx].begin(), t_EventId);
	}
	else {
		m_WaitEventIdList[t_ClusterIdx].push_back(t_EventId);
	}
}

inline
void RSU::RRM_RR::pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}