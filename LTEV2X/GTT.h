#pragma once
#include<random>
#include<set>
#include<tuple>
#include"RSU.h"
#include"VUE.h"
#include"eNB.h"
#include"Road.h"
#include"Config.h"

// <GTT>: Geographical Topology and Transport

class GTT_Urban_VeUE;
class GTT_HighSpeed_VeUE;

class GTT_VeUE {
	/*------------------静态------------------*/
public:
	/*
	* 车辆计数
	*/
	static int s_VeUECount;
	/*------------------域------------------*/
public:
	VeUE* m_This=nullptr;

	/*
	* 车辆ID
	*/
	const int m_VeUEId = s_VeUECount++;

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
	double* m_H = nullptr;

	/*
	* 车辆与所有RSU之间的距离
	*/
	double* m_Distance = nullptr;

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

	/*
	* 地理位置更新日志信息
	*/
	std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;

	/*------------------方法------------------*/
public:
	/*
	* 析构函数，释放指针
	*/
	~GTT_VeUE();
	virtual void initialize(VeUEConfig &t_VeUEConfig) = 0;

	virtual GTT_Urban_VeUE  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_VeUE  *const getHighSpeedPoint() = 0;
};


class GTT_Basic {
	/*------------------域------------------*/
public:
	/*
	* 当前的TTI时刻,指向系统的该参数
	*/
	int& m_TTI;

	/*
	* 系统配置参数,指向系统的该参数
	*/
	SystemConfig& m_Config;

	/*
	* 基站容器,指向系统的该参数
	* 这里为什么必须是引用类型，因为系统的这些数组指针必须靠该模块来初始化，因此不能传入拷贝
	*/
	eNB* &m_eNBAry;

	/*
	* 道路容器,指向系统的该参数
	*/
	Road* &m_RoadAry;

	/*
	* RSU容器,指向系统的该参数
	*/
	RSU* &m_RSUAry;

	/*
	* VeUE容器,指向系统的该参数
	*/
	GTT_VeUE** m_VeUEAry;

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	GTT_Basic() = delete;

	/*
	* 构造函数
	* 这里指针都是引用类型，因为需要初始化系统的各个实体数组
	* 该构造函数也定义了该模块的视图
	*/
	GTT_Basic(int &t_TTI, SystemConfig& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry) :
		m_TTI(t_TTI), m_Config(t_Config), m_eNBAry(t_eNBAry), m_RoadAry(t_RoadAry), m_RSUAry(t_RSUAry) {}

	/*
	* 析构函数
	*/
	~GTT_Basic();

	/*
	* 模块参数配置
	*/
	virtual void configure() = 0;

	/*
	* 当发生位置更新时，清除缓存的调度相关信息
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* 初始化各个实体数组
	*/
	virtual void initialize() = 0;

	/*
	* 信道产生
	*/
	virtual void channelGeneration() = 0;

	/*
	* 信道刷新
	*/
	virtual void freshLoc() = 0;

	/*
	* 写入地理位置更新日志
	*/
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &t_File1, std::ofstream &t_File2) = 0;

	/*
	* 计算干扰矩阵
	* 传入的参数解释
	*		外层下标为车辆编号
	*		内层下标为Pattern编号
	*		最内层list为该车辆在该Pattern下的干扰列表
	* 目前仅有簇间干扰，因为RSU间干扰太小，几乎可以忽略
	*/
	virtual void calculateInterference(const std::vector<std::vector<std::list<int>>>& t_RRMInterferenceVec) = 0;
};
