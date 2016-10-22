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
	//类内嵌套结构体前置声明
	struct GTAT;
	struct GTATUrban;
	struct GTATHigh;
	struct RRM;
	struct RRMDRA;
	struct RRMRR;
	struct WT;
	struct TMAC;


	//类内结构体指针，只能是指针形式，因为到当前行，结构体的定义尚未出现，只能定义不完整类型
	GTAT* m_GTAT = nullptr;//用于存储供其他模块使用的参数
	GTATUrban* m_GTATUrban = nullptr;//用于存储城镇场景的特定参数
	GTATHigh* m_GTATHigh = nullptr;//用于存储高速场景的特定参数
	RRM* m_RRM = nullptr;
	RRMDRA* m_RRMDRA = nullptr;//用于存储DRA模式的特定参数
	RRMRR* m_RRMRR = nullptr;//用于存储RR模式的特定参数
	WT* m_WT = nullptr;
	TMAC* m_TMAC = nullptr;


	cRSU();
	void initializeUrban(sRSUConfigure &t_RSUConfigure);
	void initializeHigh(sRSUConfigure &t_RSUConfigure);
	void initializeElse();
	~cRSU();

	//类内数据结构定义
	struct GTAT {
		int m_RSUId;
		std::list<int> m_VeUEIdList;//当前RSU范围内的VeUEId编号容器,RRM_DRA模块需要
		int m_DRAClusterNum;//一个RSU覆盖范围内的簇的个数,RRM_DRA模块需要
		std::vector<std::list<int>> m_DRAClusterVeUEIdList;//存放每个簇的VeUE的Id的容器,下标代表簇的编号
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
			int eventId;//事件编号
			int VeUEId;//车辆编号
			int RSUId;//RSU编号
			int patternIdx;//频域块编号
			int remainBitNum = -1;//剩余待传bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
			int transimitBitNum = -1;//当前传输的bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
			int occupiedTTINum = -1;//传输完还需占用的TTI数量

			sDRAScheduleInfo() {}
			sDRAScheduleInfo(int eventId, int VeUEId, int RSUId, int patternIdx) {
				this->eventId = eventId;
				this->VeUEId = VeUEId;
				this->RSUId = RSUId;
				this->patternIdx = patternIdx;
			}

			std::string toLogString(int n);

			/*
			* 生成表示调度信息的string对象
			* 包括事件的Id，车辆的Id，以及要传输该事件所占用的TTI区间
			*/
			std::string toScheduleString(int n);
		};


		GTAT* m_GTAT;


		/*
		* TDR:Time Domain Resource
		* 下标代表簇编号
		* tuple存储的变量的含义依次为：存储每个簇所分配时间数量区间的左端点，右端点以及区间长度
		*/
		std::vector<std::tuple<int, int, int>> m_DRAClusterTDRInfo;


		/*
		* Pattern块释是否可用的标记
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号
		* 若"m_DRAPatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
		*/
		std::vector<std::vector<bool>> m_DRAPatternIsAvailable;

		/*
		* 用于存放当前TTI的接纳事件链表
		*/
		std::list<int> m_DRAAdmitEventIdList;

		/*
		* RSU级别的等待列表
		* 存放的是VeUEId
		* 其来源有：
		*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
		*		2、事件链表中当前的事件触发，但VeUE未在可发送消息的时段，转入等待链表
		*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
		*/
		std::list<int> m_DRAWaitEventIdList;

		/*
		* 存放调度调度信息
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号
		*/
		std::vector<std::vector<sDRAScheduleInfo*>> m_DRAScheduleInfoTable;

		/*
		* 当前时刻当前RSU内处于传输状态的调度信息链表
		* 外层下标代表Pattern编号
		* 内层用list用于处理冲突
		*/
		std::vector<std::list<sDRAScheduleInfo*>> m_DRATransimitScheduleInfoList;


		/*
		* Pattern块释是否可用的标记
		* 下标代表Pattern编号
		* 若"m_DRAPatternIsAvailable[j]==true"代表Pattern块j可用
		*/
		std::vector<bool> m_DRAEmergencyPatternIsAvailable;

		/*
		* 当前时刻等待接入的紧急事件列表
		*/
		std::list<int> m_DRAEmergencyAdmitEventIdList;

		/*
		* 当前时刻处于等待接入状态的紧急事件列表
		*/
		std::list<int> m_DRAEmergencyWaitEventIdList;

		/*
		* 当前时刻处于传输状态的紧急事件调度信息列表
		* 外层下标代表pattern编号
		*/
		std::vector<std::list<sDRAScheduleInfo*>> m_DRAEmergencyTransimitScheduleInfoList;

		/*
		* 当前时刻处于调度状态的紧急事件调度信息列表
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

	/*------------------成员函数------------------*/

	std::string toString(int n);

	int DRAGetClusterIdx(int TTI);

	/*
	* 用于查找指定VeUEId所属的簇编号
	* 不要用这个函数来判断VeUE是否属于该RSU（直接用VeUE对象的RSU编号来判断即可）
	* 这个函数的使用前提是，已知车辆已在簇中
	*/
	int DRAGetClusterIdxOfVeUE(int VeUEId);

	/*
	* 将AdmitEventIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToAdmitEventIdList(int eventId);
	void DRAPushToEmergencyAdmitEventIdList(int eventId);

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToWaitEventIdList(int eventId);
	void DRAPushToEmergencyWaitEventIdList(int eventId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToSwitchEventIdList(int eventId, std::list<int>& systemDRASwitchVeUEIdList);

	/*
	* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToTransmitScheduleInfoList(RRMDRA::sDRAScheduleInfo* p, int patternIdx);
	void DRAPushToEmergencyTransmitScheduleInfoList(RRMDRA::sDRAScheduleInfo* p, int patternIdx);

	/*
	* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
	*/
	void DRAPushToScheduleInfoTable(int clusterIdx, int patternIdx, RRMDRA::sDRAScheduleInfo*p);
	void DRAPushToEmergencyScheduleInfoTable(int patternIdx, RRMDRA::sDRAScheduleInfo*p);

	/*
	* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
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
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)：该车不在当前RSU中");
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
	/*将处于调度表中当前可以传输的信息压入m_DRATransimitEventIdList*/
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

