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

class cRSU {
public:
	//����Ƕ�׽ṹ��ǰ������
	struct GTAT;
	struct GTATUrban;
	struct GTATHigh;
	struct RRM;
	struct RRMDRA;
	struct RRMRR;
	struct WT;
	struct TMAC;


	//���ڽṹ��ָ�룬ֻ����ָ����ʽ����Ϊ����ǰ�У��ṹ��Ķ�����δ���֣�ֻ�ܶ��岻��������
	GTAT* m_GTAT = nullptr;//���ڴ洢������ģ��ʹ�õĲ���
	GTATUrban* m_GTATUrban = nullptr;//���ڴ洢���򳡾����ض�����
	GTATHigh* m_GTATHigh = nullptr;//���ڴ洢���ٳ������ض�����
	RRM* m_RRM = nullptr;
	RRMDRA* m_RRMDRA = nullptr;//���ڴ洢DRAģʽ���ض�����
	RRMRR* m_RRMRR = nullptr;//���ڴ洢RRģʽ���ض�����
	WT* m_WT = nullptr;
	TMAC* m_TMAC = nullptr;


	cRSU();
	void initializeUrban(sRSUConfigure &t_RSUConfigure);
	void initializeHigh(sRSUConfigure &t_RSUConfigure);
	void initializeElse();
	~cRSU();

	//�������ݽṹ����
	struct GTAT {
		int m_RSUId;
		std::list<int> m_VeUEIdList;//��ǰRSU��Χ�ڵ�VeUEId�������,RRM_DRAģ����Ҫ
		int m_DRAClusterNum;//һ��RSU���Ƿ�Χ�ڵĴصĸ���,RRM_DRAģ����Ҫ
		std::vector<std::list<int>> m_DRAClusterVeUEIdList;//���ÿ���ص�VeUE��Id������,�±����صı��
	};

	struct GTATUrban {
		double m_fAbsX;
		double m_fAbsY;
		cIMTA *imta;
		double m_fantennaAngle;
	};

	struct GTATHigh {
		double m_fAbsX;
		double m_fAbsY;
		cIMTA *imta;
		double m_fantennaAngle;
	};

	struct RRM {

	};

	struct RRMDRA {

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


		GTAT* m_GTAT;


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


		RRMDRA(GTAT* t_GTAT);
	};

	struct RRMRR {

	};

	struct WT {

	};

	struct TMAC {

	};

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



inline
int cRSU::DRAGetClusterIdxOfVeUE(int VeUEId) {
	int dex = -1;
	for (int clusterIdx = 0; clusterIdx < m_GTAT->m_DRAClusterNum; clusterIdx++) {
		for (int Id : m_GTAT->m_DRAClusterVeUEIdList[clusterIdx])
			if (Id == VeUEId) return clusterIdx;
	}
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)���ó����ڵ�ǰRSU��");
}


inline
void cRSU::DRAPushToAdmitEventIdList(int eventId) {
	m_RRMDRA->m_DRAAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToEmergencyAdmitEventIdList(int eventId) {
	m_RRMDRA->m_DRAEmergencyAdmitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToWaitEventIdList(int eventId) {
	m_RRMDRA->m_DRAWaitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToEmergencyWaitEventIdList(int eventId) {
	m_RRMDRA->m_DRAEmergencyWaitEventIdList.push_back(eventId);
}

inline
void cRSU::DRAPushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
}

inline
void cRSU::DRAPushToTransmitScheduleInfoList(RRMDRA::sDRAScheduleInfo* p, int patternIdx) {
	m_RRMDRA->m_DRATransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void cRSU::DRAPushToEmergencyTransmitScheduleInfoList(RRMDRA::sDRAScheduleInfo* p, int patternIdx) {
	m_RRMDRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(p);
}

inline
void cRSU::DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, RRMDRA::sDRAScheduleInfo*p) {
	m_RRMDRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx] = p;
}

inline
void cRSU::DRAPushToEmergencyScheduleInfoTable(int patternIdx, RRMDRA::sDRAScheduleInfo*p) {
	m_RRMDRA->m_DRAEmergencyScheduleInfoTable[patternIdx] = p;
}

inline
void cRSU::DRAPullFromScheduleInfoTable(int TTI) {
	int clusterIdx = DRAGetClusterIdx(TTI);
	/*�����ڵ��ȱ��е�ǰ���Դ������Ϣѹ��m_DRATransimitEventIdList*/
	for (int patternIdx = 0; patternIdx < gc_DRATotalPatternNum; patternIdx++) {
		if (m_RRMDRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
			m_RRMDRA->m_DRATransimitScheduleInfoList[patternIdx].push_back(m_RRMDRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx]);
			m_RRMDRA->m_DRAScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
		}
	}
}

inline
void cRSU::DRAPullFromEmergencyScheduleInfoTable() {
	for (int patternIdx = 0; patternIdx < gc_DRAEmergencyTotalPatternNum; patternIdx++) {
		if (m_RRMDRA->m_DRAEmergencyScheduleInfoTable[patternIdx] != nullptr) {
			m_RRMDRA->m_DRAEmergencyTransimitScheduleInfoList[patternIdx].push_back(m_RRMDRA->m_DRAEmergencyScheduleInfoTable[patternIdx]);
			m_RRMDRA->m_DRAEmergencyScheduleInfoTable[patternIdx] = nullptr;
		}
	}
}

