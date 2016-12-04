#pragma once
#include<vector>
#include<list>
#include<string>
#include"Config.h"

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


class  GTT_HighSpeed_eNB :public GTT_eNB {
public:
	/*
	* 初始化方法
	* 不用构造函数的原因是构造的时刻其依赖项还没创建完毕
	*/
	void initialize(eNBConfig &t_eNBConfig)override;

	/*
	* 用于取得指向实际类型的指针
	*/
	GTT_Urban_eNB  *const getUrbanPoint()override { throw std::logic_error("RuntimeException"); }
	GTT_HighSpeed_eNB  *const getHighSpeedPoint()override { return this; }
};
