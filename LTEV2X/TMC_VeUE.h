#pragma once
#include<random>
#include<vector>

class VeUE;

class TMC_VeUE {
	/*------------------静态------------------*/
public:
	/*
	* 随机数引擎，该类共享
	*/
	static std::default_random_engine s_Engine;
	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元VeUE数据交互的系统级VeUE对象
	*/
	VeUE* m_This;

public:
	/*
	* 默认构造函数
	*/
	TMC_VeUE();

	/*
	* 对应拥塞等级下，车辆下次周期事件触发的时刻
	*/
	std::vector<int> m_NextPeriodEventTriggerTTI;

	/*------------------方法------------------*/
public:
	/*
	* 取得系统级System的VeUE的指针
	*/
	VeUE* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的VeUE的指针
	*/
	void setSystemPoint(VeUE* t_Point) { m_This = t_Point; }
};
