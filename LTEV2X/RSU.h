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
	class GTT;
	class GTT_Urban;
	class GTT_HighSpeed;
	class RRM;
	class RRM_TDM_DRA;
	class RRM_ICC_DRA;
	class RRM_RR;
	class WT;
	class TMC;


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
	class GTT {
	public:
		int m_RSUId;

		double m_AbsX;
		double m_AbsY;
		IMTA *m_IMTA = nullptr;
		double m_FantennaAngle;

		std::list<int> m_VeUEIdList;//��ǰRSU��Χ�ڵ�VeUEId�������,RRMģ����Ҫ
		int m_ClusterNum;//һ��RSU���Ƿ�Χ�ڵĴصĸ���,RRMģ����Ҫ
		std::vector<std::list<int>> m_ClusterVeUEIdList;//���ÿ���ص�VeUE��Id������,�±����صı��

		~GTT();
		std::string toString(int t_NumTab);
	};

	class GTT_Urban {
	public:
	};

	class GTT_HighSpeed {
	public:
	};

	class RRM {
	public:
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

	class RRM_TDM_DRA {
	public:

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
		* �ڲ��±����Pattern���
		* ��"m_PatternIsAvailable[i][j]==true"�����i��Pattern��j����
		*/
		std::vector<std::vector<bool>> m_PatternIsAvailable;


		/*
		* �ȴ��б�
		* ��ŵ���VeUEId
		* ����±����ر��
		* �ڲ�first����Emergency��second�����Emergency
		*/
		std::vector<std::pair<std::list<int>,std::list<int>>> m_WaitEventIdList;

		/*
		* ��ŵ��ȵ�����Ϣ(�Ѿ��ɹ����룬����δ������ϣ����䴫�����֮ǰ��һֱռ�ø���Դ��)
		* ����±����ر��
		* �ڲ��±����Pattern���
		*/
		std::vector<std::vector<RSU::RRM::ScheduleInfo*>> m_ScheduleInfoTable;

		/*
		* ��ǰʱ�̵�ǰRSU�ڴ��ڴ���״̬�ĵ�����Ϣ����
		* ����±�Ϊ�ر��
		* �в��±����Pattern���
		* �ڲ���list���ڴ����ͻ������Ӧ�ض�ӦPattern�µĵ�ǰ���д�����¼��ĵ�����Ϣ
		* ���ڽ����¼������дض���Ч
		* ���ڷǽ����¼�������ǰʱ�̶�Ӧ�Ĵ���Ч
		*/
		std::vector<std::vector<std::list<RSU::RRM::ScheduleInfo*>>>  m_TransimitScheduleInfoList;



		RRM_TDM_DRA(RSU* t_This);//���캯��

		/*------------------��Ա����------------------*/

		std::string toString(int t_NumTab);

		int getClusterIdx(int t_TTI);



		/*
		* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToWaitEventIdList(bool isEmergency, int t_ClusterIdx, int t_EventId);

		/*
		* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

		/*
		* ��TransimitScheduleInfo����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToTransimitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info);

		/*
		* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToScheduleInfoTable(RSU::RRM::ScheduleInfo* t_Info);

		/*
		* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
		*/
		void pullFromScheduleInfoTable(int t_TTI);
	};

	class RRM_ICC_DRA {
	public:
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

		/*
		* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToScheduleInfoTable(RSU::RRM::ScheduleInfo* t_Info);

		/*
		* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
		*/
		void pullFromScheduleInfoTable(int t_TTI);
	};

	class RRM_RR {
	public:
		RSU* m_This;//RRM_RR���õ�GTT����ز�������C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ

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
		* ����±����ر��
		* �ڲ��±����Pattern���
		*/
		std::vector<std::vector<RSU::RRM::ScheduleInfo*>> m_TransimitScheduleInfoTable;

		/*------------------��Ա����------------------*/
		RRM_RR(RSU* t_This);//���캯��
		std::string toString(int t_NumTab);
		/*
		* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToWaitEventIdList(bool isEmergency, int t_ClusterIdx, int t_EventId);

		/*
		* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList);

		/*
		* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
		*/
		void pushToTransimitScheduleInfoTable(RSU::RRM::ScheduleInfo* t_Info);
	};

	class WT {
	public:
	};

	class TMC {
	public:
	};

};


inline
void RSU::RRM_TDM_DRA::pushToWaitEventIdList(bool isEmergency, int t_ClusterIdx, int t_EventId) {
	if (isEmergency)
		m_WaitEventIdList[t_ClusterIdx].first.push_back(t_EventId);
	else
		m_WaitEventIdList[t_ClusterIdx].second.push_back(t_EventId);
}


inline
void RSU::RRM_TDM_DRA::pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}

inline
void RSU::RRM_TDM_DRA::pushToTransimitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info) {
	m_TransimitScheduleInfoList[t_Info->clusterIdx][t_Info->patternIdx].push_back(t_Info);
}


inline
void RSU::RRM_TDM_DRA::pushToScheduleInfoTable(RSU::RRM::ScheduleInfo* t_Info) {
	m_ScheduleInfoTable[t_Info->clusterIdx][t_Info->patternIdx] = t_Info;
}


inline
void RSU::RRM_TDM_DRA::pullFromScheduleInfoTable(int t_TTI) {
	/*�����ڵ��ȱ��е�ǰ���Դ������Ϣѹ��m_TransimitEventIdList*/

	/*  EMERGENCY  */
	for (int clusterIdx = 0; clusterIdx < m_This->m_GTT->m_ClusterNum; clusterIdx++) {
		for (int patternIdx = 0; patternIdx < ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx++) {
			if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
				m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
				m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
			}
		}
	}
	/*  EMERGENCY  */

	int clusterIdx = getClusterIdx(t_TTI);
	for (int patternIdx = ns_RRM_TDM_DRA::gc_PatternNumPerPatternType[EMERGENCY]; patternIdx < ns_RRM_TDM_DRA::gc_TotalPatternNum; patternIdx++) {
		if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
			m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
			m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
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
void RSU::RRM_ICC_DRA::pushToScheduleInfoTable(RSU::RRM::ScheduleInfo* t_Info) {
	m_ScheduleInfoTable[t_Info->clusterIdx][t_Info->patternIdx] = t_Info;
}

inline
void RSU::RRM_ICC_DRA::pullFromScheduleInfoTable(int t_TTI) {
	for (int clusterIdx = 0; clusterIdx < m_This->m_GTT->m_ClusterNum; clusterIdx++) {
		for (int patternIdx = 0; patternIdx < ns_RRM_ICC_DRA::gc_TotalPatternNum; patternIdx++) {
			if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
				m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
				m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
			}
		}
	}
}




inline
void RSU::RRM_RR::pushToWaitEventIdList(bool isEmergency, int t_ClusterIdx, int t_EventId) {
	if (isEmergency)
		m_WaitEventIdList[t_ClusterIdx].insert(m_WaitEventIdList[t_ClusterIdx].begin(), t_EventId);
	else
		m_WaitEventIdList[t_ClusterIdx].push_back(t_EventId);
}


inline
void RSU::RRM_RR::pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}


inline
void RSU::RRM_RR::pushToTransimitScheduleInfoTable(RSU::RRM::ScheduleInfo* t_Info) {
	m_TransimitScheduleInfoTable[t_Info->clusterIdx][t_Info->patternIdx] = t_Info;
}