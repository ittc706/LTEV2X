#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include<random>
#include<map>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"


class VeUE {
public:
	//类内嵌套结构体前置声明
	struct GTT;
	struct GTT_Urban;
	struct GTT_HighSpeed;
	struct RRM;
	struct RRM_TDM_DRA;
	struct RRM_ICC_DRA;
	struct RRM_RR;
	struct WT;
	struct TMC;

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

	
	~VeUE();
	//每个类内结构体均由相应模块来完成初始化
	void initializeGTT_Urban(VeUEConfigure &t_UEConfigure);
	void initializeGTT_HighSpeed(VeUEConfigure &t_UEConfigure);
	void initializeRRM_TDM_DRA();
	void initializeRRM_ICC_DRA();
	void initializeRRM_RR();
	void initializeWT();
	void initializeTMC();

	static int m_VeUECount;

	const int m_VeUEId = m_VeUECount++;//车辆ID
	

	//类内数据结构定义
	struct GTT {
		~GTT();

		int m_RSUId;//车辆所在的RSUId
		int m_ClusterIdx;//车辆所在簇编号

		int m_Nt;//发送天线数目，WT_B模块需要
		int m_Nr;//接收天线数目，WT_B模块需要
		double m_Ploss;//路径损耗，WT_B模块需要
		double* m_H;//信道响应矩阵，WT_B模块需要

		/*
		* 其他车辆，对当前车辆的干扰路径损耗，WT_B模块需要
		* 下标：VeUEId(会在一开始就开辟好所有车辆的槽位，该层的size不变)
		*/
		std::vector<double> m_InterferencePloss;
		/*
		* 其他车辆，对当前车辆的信道响应矩阵，WT_B模块需要
		* 下标：VeUEId：VeUEId(会在一开始就开辟好所有车辆的槽位，该层的size不变)
		*/
		std::vector<double*> m_InterferenceH;//下标对应的Pattern下，干扰信道响应矩阵，WT_B模块需要
	};

	struct GTT_Urban {
		~GTT_Urban();
		int m_RoadId;
		int m_LocationId;
		double m_X;//相对横坐标
		double m_Y;
		double m_AbsX;//绝对横坐标
		double m_AbsY;
		double m_V;//速度
		double m_VAngle;
		double m_FantennaAngle;
		IMTA *m_IMTA = nullptr;
	};

	struct GTT_HighSpeed {
		~GTT_HighSpeed();
		int m_RoadId;
		double m_X;//相对横坐标
		double m_Y;
		double m_AbsX;//绝对横坐标
		double m_AbsY;
		double m_V;//速度
		double m_VAngle;
		double m_FantennaAngle;
		IMTA *m_IMTA = nullptr;
	};

	struct RRM {
		std::vector<int> m_InterferenceVeUENum;//下标对应的Pattern下，同频干扰数量
		std::vector<std::vector<int>> m_InterferenceVeUEIdVec;//下标对应的Pattern下，同频干扰车辆ID，不包含当前车辆，每个类型对应的总数就是m_InterferenceVeUENum[patternIdx]
		std::vector<std::vector<int>> m_PreInterferenceVeUEIdVec;//含义同上，上一次的干扰车辆，用于判断是否相同
		/*
		* 下标为Pattern编号
		* tuple第一个元素：调制方式对应的2的指数，如QPSK为2，16QAM为4 64QAM位6，WT模块需要
		* tuple第二个元素：调制方式对应的每符号的比特数目(实部或虚部)
		* tuple第三个元素：编码码率
		*/
		std::vector<std::tuple<ModulationType,int,double>> m_WTInfo;
		std::vector<bool> m_isWTCached;//标记是否缓存过WT结果，下标为patternIdx,需要在位置更新后清空

		bool isNeedRecalculateSINR(int patternIdx);
	};

	struct RRM_TDM_DRA {
		static std::default_random_engine s_Engine;

		VeUE* m_This;//RRM_TDM_DRA会用到GTT的相关参数，而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系
		
		std::tuple<int, int> m_ScheduleInterval;//该VeUE所在簇的当前一轮调度区间

		RRM_TDM_DRA(VeUE* t_This) :m_This(t_This) {}
		//成员函数
		int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

		std::string toString(int t_NumTab);//用于打印VeUE信息
	};

	struct RRM_ICC_DRA {
		static std::default_random_engine s_Engine;

		VeUE* m_This;//RRM_ICC_DRA会用到GTT的相关参数，而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系

		RRM_ICC_DRA(VeUE* t_This) :m_This(t_This) {}

		int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);//随机选择资源块

		std::string toString(int t_NumTab);//用于打印VeUE信息
	};

	struct RRM_RR {

	};

	struct WT {

	};

	struct TMC {
		std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//地理位置更新日志信息
	};
};


inline
int VeUE::RRM_TDM_DRA::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}


inline
int VeUE::RRM_ICC_DRA::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}
