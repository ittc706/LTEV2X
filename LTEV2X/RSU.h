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

	};

	struct RRMRR {

	};

	struct WT {

	};

	struct TMAC {

	};
};


