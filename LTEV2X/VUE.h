#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include<random>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"


class cVeUE {
public:
	static int m_VeUECount;

	cVeUE();
	~cVeUE();
	void initializeUrban(sUEConfigure &t_UEConfigure);
	void initializeHighSpeed(sUEConfigure &t_UEConfigure);
	void initializeElse();//由于其他模块的成员初始化可能依赖于GTAT模块，因此初始化GTAT完毕后，再调用该函数

	//类内嵌套结构体前置声明
	struct GTAT;
	struct GTATUrban;
	struct GTATHighSpeed;
	struct RRM;
	struct RRMDRA;
	struct RRMRR;
	struct WT;
	struct TMAC;

	//类内结构体指针，只能是指针形式，因为到当前行，结构体的定义尚未出现，只能定义不完整类型
	GTAT* m_GTAT = nullptr;//用于存储供其他模块使用的参数
	GTATUrban* m_GTATUrban = nullptr;//用于存储城镇场景的特定参数
	GTATHighSpeed* m_GTATHighSpeed = nullptr;//用于存储高速场景的特定参数
	RRM* m_RRM = nullptr;
	RRMDRA* m_RRMDRA = nullptr;//用于存储DRA模式的特定参数
	RRMRR* m_RRMRR = nullptr;//用于存储RR模式的特定参数
	WT* m_WT = nullptr;
	TMAC* m_TMAC = nullptr;
	

	const int m_VeUEId = m_VeUECount++;//车辆ID
	

	//类内数据结构定义
	struct GTAT {
		int m_RSUId;//RRM_DRA模块需要
		int m_ClusterIdx;//RRM_DRA模块需要

		int m_Nt;//发送天线数目，WT_B模块需要
		int m_Nr;//接收天线数目，WT_B模块需要
		double m_Ploss;//路径损耗，WT_B模块需要
		double *m_H=nullptr;//信道响应矩阵，WT_B模块需要

		std::vector<double> m_InterferencePloss;//干扰路径损耗，WT_B模块需要
		double *m_InterferenceH = nullptr;//干扰信道响应矩阵，WT_B模块需要
	};

	struct GTATUrban {
		int m_RoadId;
		int m_LocationId;
		double m_X;//相对横坐标
		double m_Y;
		double m_AbsX;//绝对横坐标
		double m_AbsY;
		double m_V;//速度
		double m_VAngle;
		double m_FantennaAngle;
		cIMTA *m_IMTA=nullptr;
	};

	struct GTATHighSpeed {
		int m_RoadId;
		double m_X;//相对横坐标
		double m_Y;
		double m_AbsX;//绝对横坐标
		double m_AbsY;
		double m_V;//速度
		double m_VAngle;
		double m_FantennaAngle;
		cIMTA *m_IMTA=nullptr;
	};

	struct RRM {
		int m_InterferenceVeUENum;//同频干扰数量
		std::vector<int> m_InterferenceVeUEVec;//同频干扰车辆ID，不包含当前车辆
		int  m_PreModulation = 4;//上一次的调制方式，WT_B模块需要
	};

	struct RRMDRA {
		static std::default_random_engine s_Engine;

		cVeUE* m_This;//RRMDRA会用到GTAT的相关参数，而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系
		
		std::tuple<int, int> m_ScheduleInterval;//该VeUE所在簇的当前一轮调度区间

		RRMDRA(cVeUE* t_this);
		//成员函数
		int DRARBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, MessageType messageType);
		int DRARBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx);

		std::string toString(int n);//用于打印VeUE信息
	};

	struct RRMRR {

	};

	struct WT {

	};

	struct TMAC {
		std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//地理位置更新日志信息
	};
};


inline
int cVeUE::RRMDRA::DRARBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, MessageType messageType) {
	int size = static_cast<int>(curAvaliablePatternIdx[messageType].size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return curAvaliablePatternIdx[messageType][u(s_Engine)];
}

inline
int cVeUE::RRMDRA::DRARBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx) {
	int size = static_cast<int>(curAvaliableEmergencyPatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return curAvaliableEmergencyPatternIdx[u(s_Engine)];
}


