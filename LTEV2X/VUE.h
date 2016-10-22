#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
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
	void initializeHigh(sUEConfigure &t_UEConfigure);

	//类内嵌套结构体前置声明
	struct GTAT;
	struct GTATUrban;
	struct GTATHigh;
	struct RRM;
	struct RRMDRA;
	struct RRMRR;

	//类内结构体指针，只能是指针形式，因为到当前行，结构体的定义尚未出现
	GTAT* m_GTAT = nullptr;//用于存储供其他模块使用的参数
	GTATUrban* m_GTATUrban = nullptr;//用于存储城镇场景的特定参数
	GTATHigh* m_GTATHigh = nullptr;//用于存储高速场景的特定参数
	RRM* m_RRM = nullptr;
	RRMDRA* m_RRMDRA = nullptr;//用于存储DRA模式的特定参数
	RRMRR* m_RRMRR = nullptr;//用于存储RR模式的特定参数
	

	const int m_VeUEId = m_VeUECount++;//车辆ID
	
	
	int m_InterVeUENum;//同频干扰数量
	std::vector<int> m_InterVeUEVec;//同频干扰车辆ID，不包含当前车辆
	
	int  m_PreModulation;//上一次的调制方式，WT_B模块需要
	std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//地理位置更新日志信息


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
		int m_wRoadID;
		int m_locationID;
		double m_fX;
		double m_fY;
		double m_fAbsX;
		double m_fAbsY;
		double m_fv;
		double m_fvAngle;
		double m_fantennaAngle;
		cIMTA *imta=nullptr;
	};

	struct GTATHigh {
		int m_wLaneID;
		double m_fX;
		double m_fY;
		double m_fAbsX;
		double m_fAbsY;
		double m_fv;
		double m_fvAngle;
		double m_fantennaAngle;
		cIMTA *imta=nullptr;
	};

	struct RRM {

	};

	struct RRMDRA {

	};

	struct RRMRR {

	};
};



