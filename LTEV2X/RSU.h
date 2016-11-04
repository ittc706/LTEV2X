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
	//类内嵌套结构体前置声明
	struct GTT;
	struct GTT_Urban;
	struct GTT_HighSpeed;
	struct RRM;
	struct RRM_TDM_DRA;
	struct RRM_RR;
	struct WT;
	struct TMC;


	//类内结构体指针，只能是指针形式，因为到当前行，结构体的定义尚未出现，只能定义不完整类型
	GTT* m_GTT = nullptr;//用于存储供其他模块使用的参数
	GTT_Urban* m_GTT_Urban = nullptr;//用于存储城镇场景的特定参数
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;//用于存储高速场景的特定参数
	RRM* m_RRM = nullptr;
	RRM_TDM_DRA* m_RRM_TDM_DRA = nullptr;//用于存储RRM_TDM_DRA模式的特定参数
	RRM_RR* m_RRM_RR = nullptr;//用于存储RR模式的特定参数
	WT* m_WT = nullptr;
	TMC* m_TMC = nullptr;


	RSU();
	void initializeGTT_Urban(RSUConfigure &t_RSUConfigure);
	void initializeGTT_HighSpeed(RSUConfigure &t_RSUConfigure);
	void initializeRRM_TDM_DRA();
	void initializeRRM_RR();
	void initializeWT();
	void initializeTMC();
	~RSU();

	//类内数据结构定义
	struct GTT {
		int m_RSUId;
		std::list<int> m_VeUEIdList;//当前RSU范围内的VeUEId编号容器,RRM_TDM_DRA模块需要
		int m_ClusterNum;//一个RSU覆盖范围内的簇的个数,RRM_TDM_DRA模块需要
		std::vector<std::list<int>> m_ClusterVeUEIdList;//存放每个簇的VeUE的Id的容器,下标代表簇的编号
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
		*            调度信息数据结构
		* ==========================================*/
		struct ScheduleInfo {
			int eventId;//事件编号
			int VeUEId;//车辆编号
			int RSUId;//RSU编号
			int patternIdx;//频域块编号
			int currentPackageIdx = -1;//当前传输的数据包的编号
			int remainBitNum = -1;//剩余待传bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
			int transimitBitNum = -1;//当前传输的bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)

			ScheduleInfo() {}
			ScheduleInfo(int eventId, int VeUEId, int RSUId, int patternIdx) {
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


		RSU* m_This;//RRM_TDM_DRA会用到GTT的相关参数，而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系


		/*
		* TDR:Time Domain Resource
		* 下标代表簇编号
		* tuple存储的变量的含义依次为：存储每个簇所分配时间数量区间的左端点，右端点以及区间长度
		*/
		std::vector<std::tuple<int, int, int>> m_ClusterTDRInfo;


		/*
		* Pattern块释是否可用的标记
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号(相对编号，除去Emergency的Pattern总数)
		* 若"m_PatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
		*/
		std::vector<std::vector<bool>> m_PatternIsAvailable;

		/*
		* 用于存放当前TTI的接纳事件链表
		*/
		std::list<int> m_AdmitEventIdList;

		/*
		* RSU级别的等待列表
		* 存放的是VeUEId
		* 其来源有：
		*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
		*		2、事件链表中当前的事件触发，但VeUE未在可发送消息的时段，转入等待链表
		*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
		*/
		std::list<int> m_WaitEventIdList;

		/*
		* 存放调度调度信息
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号(相对编号，除去Emergency的Pattern总数)
		*/
		std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;

		/*
		* 当前时刻当前RSU内处于传输状态的调度信息链表
		* 外层下标代表Pattern编号(相对编号，除去Emergency的Pattern总数)
		* 内层用list用于处理冲突
		*/
		std::vector<std::list<ScheduleInfo*>> m_TransimitScheduleInfoList;


		/*
		* Pattern块释是否可用的标记
		* 下标代表Pattern编号(绝对编号)
		* 若"m_EmergencyPatternIsAvailable[j]==true"代表Pattern块j可用
		*/
		std::vector<bool> m_EmergencyPatternIsAvailable;

		/*
		* 当前时刻等待接入的紧急事件列表
		*/
		std::list<int> m_EmergencyAdmitEventIdList;

		/*
		* 当前时刻处于等待接入状态的紧急事件列表
		*/
		std::list<int> m_EmergencyWaitEventIdList;

		/*
		* 当前时刻处于传输状态的紧急事件调度信息列表
		* 外层下标代表pattern编号(绝对编号)
		*/
		std::vector<std::list<ScheduleInfo*>> m_EmergencyTransimitScheduleInfoList;

		/*
		* 当前时刻处于调度状态的紧急事件调度信息列表
		* 外层下标代表pattern编号(绝对编号)
		*/
		std::vector<ScheduleInfo*> m_EmergencyScheduleInfoTable;

		RRM_TDM_DRA(RSU* t_this);//构造函数

		/*------------------成员函数------------------*/

		std::string toString(int n);

		int getClusterIdx(int TTI);

		/*
		* 用于查找指定VeUEId所属的簇编号
		* 不要用这个函数来判断VeUE是否属于该RSU（直接用VeUE对象的RSU编号来判断即可）
		* 这个函数的使用前提是，已知车辆已在簇中
		*/
		int getClusterIdxOfVeUE(int VeUEId);

		/*
		* 将AdmitEventIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToAdmitEventIdList(int eventId);
		void pushToEmergencyAdmitEventIdList(int eventId);

		/*
		* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToWaitEventIdList(int eventId);
		void pushToEmergencyWaitEventIdList(int eventId);

		/*
		* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToSwitchEventIdList(int eventId, std::list<int>& systemDRASwitchVeUEIdList);

		/*
		* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToTransmitScheduleInfoList(RRM_TDM_DRA::ScheduleInfo* p, int patternIdx);
		void pushToEmergencyTransmitScheduleInfoList(RRM_TDM_DRA::ScheduleInfo* p, int patternIdx);

		/*
		* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToScheduleInfoTable(int clusterIdx, int patternIdx, RRM_TDM_DRA::ScheduleInfo*p);
		void pushToEmergencyScheduleInfoTable(int patternIdx, RRM_TDM_DRA::ScheduleInfo*p);

		/*
		* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
		*/
		void pullFromScheduleInfoTable(int TTI);
		void pullFromEmergencyScheduleInfoTable();
	};

	struct RRM_RR {
		/*===========================================
		*            调度信息数据结构
		* ==========================================*/
		struct ScheduleInfo {
			int eventId;//事件编号
			MessageType messageType;//事件类型
			int VeUEId;//车辆编号
			int RSUId;//RSU编号
			int patternIdx;//频域块编号
			int currentPackageIdx = -1;//当前传输的数据包的编号
			int remainBitNum = -1;//剩余待传bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
			int transimitBitNum = -1;//当前传输的bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)


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
			* 生成表示调度信息的string对象
			* 包括事件的Id，车辆的Id，以及要传输该事件所占用的TTI区间
			*/
			std::string toScheduleString(int n);
		};

		RSU* m_This;//RRM_TDM_DRA会用到GTT的相关参数，而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系

		/*
		* 当前TTI接入列表
		* 外层下标为簇编号
		* 内层list最大长度不超过Pattern数量
		*/
		std::vector<std::vector<int>> m_AdmitEventIdList;

		/*
		* RSU级别的等待列表
		* 外层下标为簇编号
		* 内层list存放的是处于等待接入状态的VeUEId
		* 其来源有：
		*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
		*		2、事件链表中当前的事件触发，转入等待链表
		*/
		std::vector<std::list<int>> m_WaitEventIdList;

		/*
		* 存放调度调度信息，本次进行传输
		* 外层代表Pattern号
		*/
		std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;


		RRM_RR(RSU* t_this);//构造函数
		/*------------------成员函数------------------*/

		/*
		* 将AdmitEventIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToAdmitEventIdList(int clusterIdx, int eventId);

		/*
		* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToWaitEventIdList(int clusterIdx, int eventId, MessageType messageType);

		/*
		* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList);
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
	throw Exp("cRSU::getClusterIdxOfVeUE(int VeUEId)：该车不在当前RSU中");
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
void RSU::RRM_TDM_DRA::pushToSwitchEventIdList(int VeUEId, std::list<int>& systemDRASwitchVeUEIdList) {
	systemDRASwitchVeUEIdList.push_back(VeUEId);
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
	/*将处于调度表中当前可以传输的信息压入m_TransimitEventIdList*/
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
void RSU::RRM_RR::pushToSwitchEventIdList(int eventId, std::list<int>& systemRRSwitchVeUEIdList) {
	systemRRSwitchVeUEIdList.push_back(eventId);
}