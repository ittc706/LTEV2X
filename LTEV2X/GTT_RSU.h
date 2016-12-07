#pragma once
#include<vector>
#include<list>
#include<string>

class RSU;
class GTT_Urban_RSU;
class GTT_HighSpeed_RSU;
class IMTA;

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

	/*
	* 一个RSU覆盖范围内的拥塞区域的个数
	*/
	int m_ZoneNum;

	/*
	* 存放每个簇的拥塞区域Zone的Id的容器,下标代表Zone的编号
	*/
	std::vector<std::list<int>> m_ZoneVeUEIdList;

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



class  GTT_HighSpeed_RSU :public GTT_RSU {
public:
	/*
	* 构造函数
	*/
	GTT_HighSpeed_RSU();

	/*
	* 用于取得指向实际类型的指针
	*/
	GTT_Urban_RSU  *const getUrbanPoint()override { throw std::logic_error("RuntimeException"); }
	GTT_HighSpeed_RSU  *const getHighSpeedPoint()override { return this; }
};
