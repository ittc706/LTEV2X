#pragma once
#include<stdexcept>
#include"GTT.h"

// <GTT_Urban>: Geographical Topology and Transport Urban

class GTT_Urban_VeUE :public GTT_VeUE {
	/*------------------域------------------*/
public:
	/*
	* <?>
	*/
	int m_LocationId;
	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	GTT_Urban_VeUE() = delete;
	GTT_Urban_VeUE(VeUEConfig &t_VeUEConfig);

	/*
	* 用于取得指向实际类型的指针
	*/
	GTT_Urban_VeUE  *const getUrbanPoint()override { return this; }
	GTT_HighSpeed_VeUE  *const getHighSpeedPoint()override { throw std::logic_error("RuntimeException"); }
};


class GTT_Urban_RSU :public GTT_RSU {
public:
	/*
	* 构造函数
	*/
	GTT_Urban_RSU();

	/*
	* 用于取得指向实际类型的指针
	*/
	GTT_Urban_RSU  *const getUrbanPoint()override { return this; }
	GTT_HighSpeed_RSU  *const getHighSpeedPoint()override { throw std::logic_error("RuntimeException"); }
};


class GTT_Urban_eNB :public GTT_eNB {
public:
	/*
	* 初始化方法
	* 不用构造函数的原因是构造的时刻其依赖项还没创建完毕
	*/
	void initialize(eNBConfig &t_eNBConfig)override;

	/*
	* 用于取得指向实际类型的指针
	*/
	GTT_Urban_eNB  *const getUrbanPoint()override { return this; }
	GTT_HighSpeed_eNB  *const getHighSpeedPoint()override { throw std::logic_error("RuntimeException"); }
};


class GTT_Urban_Road :public GTT_Road {
	/*------------------域------------------*/
public:
	/*
	* 基站数目
	*/
	int m_eNBNum;

	/*
	* 基站Id
	*/
	int m_eNBId;

	/*
	* 持有指向其所在基站对象的指针
	*/
	GTT_eNB *m_eNB;

public:
	/*
	* 初始化方法
	* 不用构造函数的原因是构造的时刻其依赖项还没创建完毕
	*/
	GTT_Urban_Road(UrbanRoadConfig &t_RoadConfig);

	/*
	* 用于取得指向实际类型的指针
	*/
	GTT_Urban_Road  *const getUrbanPoint()override { return this; }
	GTT_HighSpeed_Road  *const getHighSpeedPoint()override { throw std::logic_error("RuntimeException"); }
};


class GTT_Urban :public GTT {
	/*------------------静态------------------*/
public:
	/*
	* 随机数引擎，该类共享
	*/
	static std::default_random_engine s_Engine;

	/*
	* 基站数量
	*/
	static const int s_eNB_NUM = 7;

	/*
	* 道路数量
	*/
	static const int s_ROAD_NUM = 14;

	/*
	* RSU数量
	*/
	static const int s_RSU_NUM = 24;

	/*
	* 南北向路长
	*/
	static int s_ROAD_LENGTH_SN;

	/*
	* 东西向路长
	*/
	static int s_ROAD_LENGTH_EW;

	/*
	* 路宽
	*/
	static double s_ROAD_WIDTH;

	/*
	* 车速
	*/
	static double s_SPEED;

	/*
	* ???
	*/
	static const double s_ROAD_TOPO_RATIO[s_ROAD_NUM * 2];

	/*
	* ???
	*/
	static const int s_WRAP_AROUND_ROAD[s_ROAD_NUM][9];

	/*
	* ???
	*/
	static const int s_RSU_CLUSTER_NUM[s_RSU_NUM];

	/*
	* ???
	*/
	static const double s_RSU_TOPO_RATIO[s_RSU_NUM * 2];

	/*
	* ???
	*/
	static const int s_RSU_IN_ROAD[s_ROAD_NUM][4];

	/*
	* 加载城镇场景配置参数
	*/
	static void loadConfig(Platform t_Platform);
	/*------------------域------------------*/
private:
	/*
	* ueTopo x轴 格数
	*/
	int m_xNum;

	/*
	* ueTopo y轴 格数
	*/
	int m_yNum;

	/*
	* ？
	*/
	int m_ueTopoNum;

	/*
	* 一个Road中ue相对坐标
	*/
	double *m_pueTopo;

	/*
	* user per road array
	*/
	int* m_pupr;

	/*
	* 车辆车速
	*/
	double m_Speed;

public:
	/*
	* 统计每个RSU下的车辆数目
	* 外层下标代表第几次位置更新(从0开始)，内层下标代表RSU编号
	*/
	std::vector<std::vector<int>> m_VeUENumPerRSU;

	/*------------------方法------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	GTT_Urban() = delete;

	/*
	* 构造函数
	* 这里指针都是引用类型，因为需要初始化系统的各个实体数组
	* 该构造函数也定义了该模块的视图
	*/
	GTT_Urban(System* t_Context);

	/*
	* 参数配置
	*/
	void configure()override;

	/*
	* 当发生位置更新时，清除缓存的调度相关信息
	*/
	void cleanWhenLocationUpdate()override;

	/*
	* 初始化各个实体数组
	*/
	void initialize()override;

	/*
	* 信道产生
	*/
	void channelGeneration()override;

	/*
	* 信道刷新
	*/
	void freshLoc() override;

	/*
	* 写入地理位置更新日志
	*/
	void writeVeUELocationUpdateLogInfo(std::ofstream &t_File1, std::ofstream &t_File2) override;

	/*
	* 计算干扰矩阵
	* 传入的参数解释
	*		外层下标为车辆编号
	*		内层下标为Pattern编号
	*		最内层list为该车辆在该Pattern下的干扰列表
	* 目前仅有簇间干扰，因为RSU间干扰太小，几乎可以忽略
	*/
	void calculateInterference(const std::vector<std::vector<std::list<int>>>& t_RRMInterferenceVec) override;
};