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
	/*------------------静态------------------*/
public:
	/*
	* 车辆计数
	*/
	static int m_VeUECount;

	/*---------------前置声明---------------*/
public:
	class GTT;
	class GTT_Urban;
	class GTT_HighSpeed;
	class RRM;
	class RRM_TDM_DRA;
	class RRM_ICC_DRA;
	class RRM_RR;
	class WT;
	class TMC;

	/*------------------域------------------*/
public:
	/*
	* 车辆ID
	*/
	const int m_VeUEId = m_VeUECount++;

	/*
	* GTT内部类指针
	* 用于存储两个场景公共的参数
	*/
	GTT* m_GTT = nullptr;

	/*
	* GTT_Urban内部类指针
	* 用于存储城镇场景的特定参数
	*/
	GTT_Urban* m_GTT_Urban = nullptr;

	/*
	* GTT_HighSpeed内部类指针
	* 用于存储高速场景的特定参数
	*/
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;

	/*
	* RRM内部类指针
	* 用于存储不同实现的共同参数
	*/
	RRM* m_RRM = nullptr;

	/*
	* RRM_TDM_DRA内部类指针
	* 用于存储RRM_TDM_DRA模式的特定参数
	*/
	RRM_TDM_DRA* m_RRM_TDM_DRA = nullptr;

	/*
	* RRM_ICC_DRA内部类指针
	* 用于存储RRM_ICC_DRA模式的特定参数
	*/
	RRM_ICC_DRA* m_RRM_ICC_DRA = nullptr;

	/*
	* RRM_RR内部类指针
	* 用于存储RR模式的特定参数
	*/
	RRM_RR* m_RRM_RR = nullptr;

	/*
	* WT内部类指针
	* 用于存储不同实现的共同参数
	*/
	WT* m_WT = nullptr;

	/*
	* TMC内部类指针
	* 用于存储不同实现的共同参数
	*/
	TMC* m_TMC = nullptr;

	/*------------------方法------------------*/
public:
	/*
	* 析构函数
	* 析构内部类指针
	*/
	~VeUE();
	
	/*
	* 城镇场景下，RSU对象初始化方法
	* 在GTT模块初始化时调用
	*/
	void initializeGTT_Urban(VeUEConfig &t_VeUEConfig);

	/*
	* 高速场景下，RSU对象初始化方法
	* 在GTT模块初始化时调用
	*/
	void initializeGTT_HighSpeed(VeUEConfig &t_VeUEConfig);

	/*
	* TDM_DRA模式下，RSU对象初始化方法
	* 在RRM模块初始化时调用
	*/
	void initializeRRM_TDM_DRA();

	/*
	* ICC_DRA模式下，RSU对象初始化方法
	* 在RRM模块初始化时调用
	*/
	void initializeRRM_ICC_DRA();

	/*
	* RR模式下，RSU对象初始化方法
	* 在RRM模块初始化时调用
	*/
	void initializeRRM_RR();

	/*
	* WT模块视图中，RSU对象初始化方法
	* 在WT模块初始化时调用
	*/
	void initializeWT();

	/*
	* TMC模块视图中，RSU对象初始化方法
	* 在WT模块初始化时调用
	*/
	void initializeTMC();

	/*-----------------内部类-----------------*/
public:
	class GTT {
		/*--------域--------*/
	public:
		/*
		* 所在道路的RoadId
		*/
		int m_RoadId;

		/*
		* 相对横坐标，纵坐标
		*/
		double m_X;
		double m_Y;

		/*
		* 绝对横坐标，纵坐标
		*/
		double m_AbsX;
		double m_AbsY;

		/*
		* 车辆速度
		*/
		double m_V;

		/*
		* <?>
		*/
		double m_VAngle;

		/*
		* <?>
		*/
		double m_FantennaAngle;

		/*
		* <?>
		*/
		IMTA *m_IMTA = nullptr;

		/*
		* 车辆所在的RSUId
		*/
		int m_RSUId;

		/*
		* 车辆所在簇编号
		*/
		int m_ClusterIdx;

		/*
		* 发送天线数目
		*/
		int m_Nt;

		/*
		* 接收天线数目
		*/
		int m_Nr;

		/*
		* 路径损耗
		*/
		double m_Ploss;

		/*
		* 信道响应矩阵
		*/
		double* m_H;

		/*
		* 车辆与所有RSU之间的距离
		*/
		double* m_Distance;

		/*
		* 其他车辆，对当前车辆的干扰路径损耗，WT_B模块需要
		* 下标：VeUEId(会在一开始就开辟好所有车辆的槽位，该层的size不变)
		*/
		std::vector<double> m_InterferencePloss;

		/*
		* 其他车辆，对当前车辆的信道响应矩阵，WT_B模块需要
		* 下标：干扰车辆的VeUEId：VeUEId(会在一开始就开辟好所有车辆的槽位，该层的size不变)
		*/
		std::vector<double*> m_InterferenceH;

		/*-------方法-------*/
	public:
		/*
		* 析构函数，释放指针
		*/
		~GTT();
	};

	class GTT_Urban {
		/*--------域--------*/
	public:
		/*
		* <?>
		*/
		int m_LocationId;
	};

	class GTT_HighSpeed {
	public:

	};

	class RRM {
		/*--------域--------*/
	public:
		/*
		* 下标对应的Pattern下，同频干扰数量
		*/
		std::vector<int> m_InterferenceVeUENum;

		/*
		* 下标对应的Pattern下，同频干扰车辆ID
		* 不包含当前车辆
		*/
		std::vector<std::vector<int>> m_InterferenceVeUEIdVec;

		/*
		* 含义同上，上一次的干扰车辆
		* 用于判断是否相同，从而决定是否需要再算一次信道以及载干比
		*/
		std::vector<std::vector<int>> m_PreInterferenceVeUEIdVec;
		
		/*
		* 调制方式
		*/
		const ModulationType m_ModulationType = gc_ModulationType;

		/*
		* 信道编码码率
		*/
		const double m_CodeRate = gc_CodeRate;

		/*
		* 上次计算的载干比
		* 若为最小值((std::numeric_limits<double>::min)())则说明之前没有计算过
		*/
		std::vector<double> m_PreSINR;

		/*-------方法-------*/
	public:
		/*
		* 判断是否需要重新计算SINR
		* 取决于干扰列表是否相同
		*/
		bool isNeedRecalculateSINR(int t_PatternIdx);

		/*
		* 判断之前是否已经算过SINR
		*/
		bool isAlreadyCalculateSINR(int t_PatternIdx) { return m_PreSINR[t_PatternIdx] != (std::numeric_limits<double>::min)(); }
	};

	class RRM_TDM_DRA {
		/*-------静态-------*/
	public:
		static std::default_random_engine s_Engine;
		/*--------域--------*/
	public:
		/*
		* RRM_TDM_DRA会用到GTT的相关参数
		* 而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系
		*/
		VeUE* m_This;

		/*
		* 该VeUE所在簇的当前地理位置的相对调度区间
		*/
		std::tuple<int, int> m_ScheduleInterval;

		/*-------方法-------*/
	public:
		/*
		* 默认构造函数定义为删除
		*/
		RRM_TDM_DRA() = delete;

		/*
		* 构造函数
		*/
		RRM_TDM_DRA(VeUE* t_This) :m_This(t_This) {}
		
		/*
		* 随机选取可用资源快
		*/
		int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

		/*
		* 生成格式化字符串
		*/
		std::string toString(int t_NumTab);
	};

	class RRM_ICC_DRA {
		/*-------静态-------*/
	public:
		static std::default_random_engine s_Engine;
		/*--------域--------*/
	public:
		/*
		* RRM_ICC_DRA会用到GTT的相关参数
		* 而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系
		*/
		VeUE* m_This;

		/*-------方法-------*/
	public:
		/*
		* 默认构造函数定义为删除
		*/
		RRM_ICC_DRA() = delete;

		/*
		* 构造函数
		*/
		RRM_ICC_DRA(VeUE* t_This) :m_This(t_This) {}

		/*
		* 随机选择资源块
		*/
		int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

		/*
		* 生成格式化字符串
		*/
		std::string toString(int t_NumTab);
	};

	class RRM_RR {
		/*--------域--------*/
	public:
		/*
		* RRM_RR会用到GTT的相关参数
		* 而C++内部类是静态的，因此传入一个外围类实例的引用，建立联系
		*/
		VeUE* m_This;

		/*-------方法-------*/
	public:
		/*
		* 默认构造函数定义为删除
		*/
		RRM_RR() = delete;

		/*
		* 构造函数
		*/
		RRM_RR(VeUE* t_This) :m_This(t_This) {}

		/*
		* 生成格式化字符串
		*/
		std::string toString(int t_NumTab);
	};

	class WT {
	public:
	};

	class TMC {
		/*--------域--------*/
	public:
		/*
		* 地理位置更新日志信息
		*/
		std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;
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
