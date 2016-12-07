#pragma once
#include<list>
#include<vector>
#include<tuple>

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
	VeUE* m_This = nullptr;

public:
	/*
	* 车辆ID
	*/
	const int m_VeUEId = s_VeUECount++;

	/*
	* 所在道路的RoadId
	*/
	int m_RoadId = -1;

	/*
	* 相对横坐标，纵坐标
	*/
	double m_X = -1;
	double m_Y = -1;

	/*
	* 绝对横坐标，纵坐标
	*/
	double m_AbsX = -1;
	double m_AbsY = -1;

	/*
	* 车辆速度
	*/
	double m_V = -1;

	/*
	* <?>
	*/
	double m_VAngle = -1;

	/*
	* <?>
	*/
	double m_FantennaAngle = -1;

	/*
	* <?>
	*/
	IMTA *m_IMTA = nullptr;

	/*
	* 车辆所在的RSUId
	*/
	int m_RSUId = -1;

	/*
	* 车辆所在簇编号
	*/
	int m_ClusterIdx = -1;

	/*
	* 车辆所在拥塞区域编号
	*/
	int m_ZoneIdx = -1;

	/*
	* 拥塞等级
	*/
	int m_CongestionLevel = 0;

	/*
	* 发送天线数目
	*/
	int m_Nt = -1;

	/*
	* 接收天线数目
	*/
	int m_Nr = -1;

	/*
	* 路径损耗
	*/
	double m_Ploss = -1;

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


//VeUE配置参数
struct VeUEConfig {
	int roadId;
	double X;
	double Y;
	double AbsX;
	double AbsY;
	double V;
	int VeUENum;
	double angle;
};


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



class GTT_HighSpeed_VeUE :public GTT_VeUE {
	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	GTT_HighSpeed_VeUE() = delete;
	GTT_HighSpeed_VeUE(VeUEConfig &t_VeUEConfig);

	/*
	* 用于取得指向实际类型的指针
	*/
	GTT_Urban_VeUE  *const getUrbanPoint()override { throw std::logic_error("RuntimeException"); }
	GTT_HighSpeed_VeUE  *const getHighSpeedPoint()override { return this; }
};