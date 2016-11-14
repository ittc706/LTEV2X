#pragma once
#include"GTT.h"

// <GTT_Urban>: Geographical Topology and Transport Urban

class GTT_Urban :public GTT_Basic {
	/*------------------静态------------------*/
public:
	/*
	* 随机数引擎，该类共享
	*/
	static std::default_random_engine s_Engine;
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
	* 城镇Road总数
	*/
	int m_UrbanRoadNum;

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
	*/
	GTT_Urban(int &t_TTI, SystemConfig& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry, VeUE* &t_VeUEAry);

	/*
	* 参数配置
	*/
	void configure()override;

	/*
	* 当发生位置更新时，清除缓存的调度相关信息
	*/
	void cleanWhenLocationUpdate()override;

	/*
	* 初始化
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
	void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) override;

	/*
	* 计算干扰矩阵
	*/
	void calculateInterference(const std::vector<std::vector<std::list<int>>>& RRMInterferenceVec) override;
};