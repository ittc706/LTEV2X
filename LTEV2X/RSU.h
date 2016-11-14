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
	class GTT;
	class GTT_Urban;
	class GTT_HighSpeed;
	class RRM;
	class RRM_TDM_DRA;
	class RRM_ICC_DRA;
	class RRM_RR;
	class WT;
	class TMC;


	//类内结构体指针，只能是指针形式，因为到当前行，结构体的定义尚未出现，只能定义不完整类型
	GTT* m_GTT = nullptr;//用于存储供其他模块使用的参数
	GTT_Urban* m_GTT_Urban = nullptr;//用于存储城镇场景的特定参数
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;//用于存储高速场景的特定参数
	RRM* m_RRM = nullptr;
	RRM_TDM_DRA* m_RRM_TDM_DRA = nullptr;//用于存储RRM_TDM_DRA模式的特定参数
	RRM_ICC_DRA* m_RRM_ICC_DRA = nullptr;//用于存储RRM_ICC_DRA模式的特定参数
	RRM_RR* m_RRM_RR = nullptr;//用于存储RR模式的特定参数
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

	//类内数据结构定义
	class GTT {
	public:
		int m_RSUId;

		double m_AbsX;
		double m_AbsY;
		IMTA *m_IMTA = nullptr;
		double m_FantennaAngle;

		std::list<int> m_VeUEIdList;//当前RSU范围内的VeUEId编号容器,RRM模块需要
		int m_ClusterNum;//一个RSU覆盖范围内的簇的个数,RRM模块需要
		std::vector<std::list<int>> m_ClusterVeUEIdList;//存放每个簇的VeUE的Id的容器,下标代表簇的编号

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
		*            调度信息数据结构
		* ==========================================*/
		class ScheduleInfo {
		public:
			int eventId;//事件编号
			int VeUEId;//车辆编号
			int RSUId;//RSU编号
			int clusterIdx;//簇编号
			int patternIdx;//频域块编号
			int currentPackageIdx = -1;//当前传输的数据包的编号
			int remainBitNum = -1;//剩余待传bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
			int transimitBitNum = -1;//当前传输的bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)

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
			* 生成表示调度信息的string对象
			* 包括事件的Id，车辆的Id，以及要传输该事件所占用的TTI区间
			*/
			std::string toScheduleString(int t_NumTab);
		};
	};

	class RRM_TDM_DRA {
	public:

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
		* 内层下标代表Pattern编号
		* 若"m_PatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
		*/
		std::vector<std::vector<bool>> m_PatternIsAvailable;


		/*
		* 等待列表
		* 存放的是VeUEId
		* 外层下标代表簇编号
		* 内层first代表Emergency，second代表非Emergency
		*/
		std::vector<std::pair<std::list<int>,std::list<int>>> m_WaitEventIdList;

		/*
		* 存放调度调度信息(已经成功接入，但尚未传输完毕，在其传输完毕之前会一直占用该资源块)
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号
		*/
		std::vector<std::vector<RSU::RRM::ScheduleInfo*>> m_ScheduleInfoTable;

		/*
		* 当前时刻当前RSU内处于传输状态的调度信息链表
		* 外层下标为簇编号
		* 中层下标代表Pattern编号
		* 内层用list用于处理冲突，即对应簇对应Pattern下的当前进行传输的事件的调度信息
		* 对于紧急事件，所有簇都有效
		* 对于非紧急事件，仅当前时刻对应的簇有效
		*/
		std::vector<std::vector<std::list<RSU::RRM::ScheduleInfo*>>>  m_TransimitScheduleInfoList;



		RRM_TDM_DRA(RSU* t_This);//构造函数

		/*------------------成员函数------------------*/

		std::string toString(int t_NumTab);

		int getClusterIdx(int t_TTI);



		/*
		* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToWaitEventIdList(bool isEmergency, int t_ClusterIdx, int t_EventId);

		/*
		* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

		/*
		* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToTransimitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info);

		/*
		* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToScheduleInfoTable(RSU::RRM::ScheduleInfo* t_Info);

		/*
		* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
		*/
		void pullFromScheduleInfoTable(int t_TTI);
	};

	class RRM_ICC_DRA {
	public:
		RSU* m_This;

		RRM_ICC_DRA(RSU* t_This);//构造函数

		
		/*
		* RSU级别的等待列表
		* 外层下标为簇编号
		* 内层list存放的是对应簇的VeUEId
		* 其来源有：
		*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
		*		2、事件链表中当前的事件触发
		*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
		*/
		std::vector<std::list<int>> m_WaitEventIdList;

		/*
		* Pattern块释是否可用的标记
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号
		* 若"m_PatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
		*/
		std::vector<std::vector<bool>> m_PatternIsAvailable;


		/*
		* 存放调度调度信息
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号
		*/
		std::vector<std::vector<RSU::RRM::ScheduleInfo*>> m_ScheduleInfoTable;


		/*
		* 当前时刻当前RSU内处于传输状态的调度信息链表
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号
		* 最内层用list用于处理冲突
		*/
		std::vector<std::vector<std::list<RSU::RRM::ScheduleInfo*>>> m_TransimitScheduleInfoList;


		std::string toString(int t_NumTab);
		/*
		* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToWaitEventIdList(int t_ClusterIdx, int t_EventId);

		/*
		* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

		/*
		* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToTransmitScheduleInfoList(RSU::RRM::ScheduleInfo* t_Info);

		/*
		* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToScheduleInfoTable(RSU::RRM::ScheduleInfo* t_Info);

		/*
		* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
		*/
		void pullFromScheduleInfoTable(int t_TTI);
	};

	class RRM_RR {
	public:
		RSU* m_This;//RRM_RR会用到GTT的相关参数，而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系

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
		* 外层下标代表簇编号
		* 内层下标代表Pattern编号
		*/
		std::vector<std::vector<RSU::RRM::ScheduleInfo*>> m_TransimitScheduleInfoTable;

		/*------------------成员函数------------------*/
		RRM_RR(RSU* t_This);//构造函数
		std::string toString(int t_NumTab);
		/*
		* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToWaitEventIdList(bool isEmergency, int t_ClusterIdx, int t_EventId);

		/*
		* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
		*/
		void pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList);

		/*
		* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
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
	/*将处于调度表中当前可以传输的信息压入m_TransimitEventIdList*/

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