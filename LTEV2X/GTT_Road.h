#pragma once
#include<string>
#include"Config.h"

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


class GTT_eNB;

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


class GTT_HighSpeed_Road :public GTT_Road {
public:
	/*
	* 构造函数
	*/
	GTT_HighSpeed_Road(HighSpeedRodeConfig &t_RoadHighSpeedConfig);

	/*
	* 用于取得指向实际类型的指针
	*/
	GTT_Urban_Road  *const getUrbanPoint()override { throw std::logic_error("RuntimeException"); }
	GTT_HighSpeed_Road  *const getHighSpeedPoint()override { return this; }
};