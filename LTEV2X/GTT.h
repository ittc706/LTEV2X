#pragma once
#include<random>
#include<vector>
#include<list>
#include<tuple>
#include"Config.h"

// <GTT>: Geographical Topology and Transport

class VeUE;
class GTT_Urban_VeUE;
class GTT_HighSpeed_VeUE;
class IMTA;

class GTT_VeUE {
	/*------------------静态------------------*/
public:
	/*
	* 车辆计数
	*/
	static int s_VeUECount;
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元VeUE数据交互的系统级VeUE对象
	*/
	VeUE* m_This=nullptr;

public:
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
	* 其他车辆，对当前车辆的干扰路径损耗，WT模块需要
	* 下标：VeUEId(会在一开始就开辟好所有车辆的槽位，该层的size不变)
	*/
	std::vector<double> m_InterferencePloss;

	/*
	* 其他车辆，对当前车辆的信道响应矩阵，WT模块需要
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
	* 构造函数
	*/
	GTT_VeUE() {}

	/*
	* 析构函数，释放指针
	*/
	~GTT_VeUE();

	/*
	* 用于取得指向实际类型的指针
	*/
	virtual GTT_Urban_VeUE  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_VeUE  *const getHighSpeedPoint() = 0;

	/*
	* 取得系统级System的VeUE的指针
	*/
	VeUE* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的VeUE的指针
	*/
	void setSystemPoint(VeUE* t_Point) { m_This = t_Point; }
};

class RSU;
class GTT_Urban_RSU;
class GTT_HighSpeed_RSU;

class GTT_RSU {
	/*------------------静态------------------*/
public:
	/*
	* 车辆计数
	*/
	static int s_RSUCount;
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元RSU数据交互的系统级RSU对象
	*/
	RSU* m_This = nullptr;

public:
	/*
	* RSUId
	*/
	int m_RSUId = s_RSUCount++;

	/*
	* 绝对横坐标，纵坐标
	*/
	double m_AbsX;
	double m_AbsY;

	/*
	* <?>
	*/
	IMTA *m_IMTA = nullptr;

	/*
	* <?>
	*/
	double m_FantennaAngle;

	/*
	* 当前RSU范围内的VeUEId编号容器,RRM模块需要
	*/
	std::list<int> m_VeUEIdList;

	/*
	* 一个RSU覆盖范围内的簇的个数,RRM模块需要
	*/
	int m_ClusterNum;

	/*
	* 存放每个簇的VeUE的Id的容器,下标代表簇的编号
	*/
	std::vector<std::list<int>> m_ClusterVeUEIdList;

	/*------------------方法------------------*/
	/*
	* 析构函数，释放指针
	*/
	~GTT_RSU();

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 用于取得指向实际类型的指针
	*/
	virtual GTT_Urban_RSU  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_RSU  *const getHighSpeedPoint() = 0;

	/*
	* 取得系统级System的RSU的指针
	*/
	RSU* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的RSU的指针
	*/
	void setSystemPoint(RSU* t_Point) { m_This = t_Point; }
};

class eNB;
class GTT_Urban_eNB;
class GTT_HighSpeed_eNB;

class GTT_eNB {
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元eNB数据交互的系统级eNB对象
	*/
	eNB* m_This = nullptr;
public:
	/*
	* 所在道路Id
	*/
	int m_RoadId;

	/*
	* 基站Id
	*/
	int m_eNBId;

	/*
	* 基站相对横纵坐标
	*/
	double m_X, m_Y;

	/*
	* 基站绝对横纵坐标
	*/
	double m_AbsX, m_AbsY;

	/*
	* 该基站中的RSU容器(存储RSU的Id)
	*/
	std::list<int> m_RSUIdList;

	/*
	* 该基站中的VeUE容器(存储VeUE的Id)
	*/
	std::list<int> m_VeUEIdList;


	/*------------------方法------------------*/
public:

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 初始化方法
	* 不用构造函数的原因是构造的时刻其依赖项还没创建完毕
	*/
	virtual void initialize(eNBConfig &t_eNBConfig) = 0;

	/*
	* 用于取得指向实际类型的指针
	*/
	virtual GTT_Urban_eNB  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_eNB  *const getHighSpeedPoint() = 0;

	/*
	* 取得系统级System的eNB的指针
	*/
	eNB* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的eNB的指针
	*/
	void setSystemPoint(eNB* t_Point) { m_This = t_Point; }
};

class Road;
class GTT_Urban_Road;
class GTT_HighSpeed_Road;

class GTT_Road {
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元Road数据交互的系统级Road对象
	*/
	Road* m_This = nullptr;
public:
	/*
	* 道路Id
	*/
	int m_RoadId;

	/*
	* 绝对横坐标与纵坐标
	*/
	double m_AbsX;
	double m_AbsY;

	/*
	* <?>
	*/
	int  m_upr;

	/*------------------方法------------------*/
public:
	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 用于取得指向实际类型的指针
	*/
	virtual GTT_Urban_Road  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_Road  *const getHighSpeedPoint() = 0;

	/*
	* 取得系统级System的Road的指针
	*/
	Road* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的Road的指针
	*/
	void setSystemPoint(Road* t_Point) { m_This = t_Point; }
};


class System;
class GTT {
	/*------------------域------------------*/
private:
	/*
	* 指向系统的指针
	*/
	System* m_Context;
public:
	/*
	* GTT单元视图下的基站容器
	*/
	GTT_eNB** m_eNBAry;

	/*
	* GTT单元视图下的道路容器
	*/
	GTT_Road** m_RoadAry;

	/*
	* GTT单元视图下的RSU容器
	*/
	GTT_RSU** m_RSUAry;

	/*
	* VeUE容器
	* 第一维度的指针指向数组，该数组存放指向GTT_VeUE实体的指针
	* 为什么数组存的是指针，因为需要实现多态
	*/
	GTT_VeUE** m_VeUEAry;

	/*------------------接口------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	GTT() = delete;

	/*
	* 构造函数
	*/
	GTT(System* t_Context) : m_Context(t_Context) {}

	/*
	* 析构函数
	*/
	~GTT();

	/*
	* 获取系统类的指针
	*/
	System* getContext() { return m_Context; }

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
