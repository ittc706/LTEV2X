#pragma once
#include<vector>
#include<string>
#include<list>
#include"Global.h"
#include"Config.h"

class eNB{
	/*------------------域------------------*/
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
	
    //<UNDONE>
	/*
	* 该基站中的RSU容器(存储RSU的Id)
	*/
	std::list<int> m_RSUIdList;

	//<UNDONE>
	/*
	* 该基站中的VeUE容器(存储VeUE的Id)
	*/
	std::list<int> m_VeUEIdList;


	/*------------------方法------------------*/
public:
	/*
	* 城镇场景下的初始化方法
	*/
	void initializeUrban(eNBConfig &t_eNBConfig);

	/*
	* 高速场景下的初始化方法
	*/
	void initializeHighSpeed(eNBConfig &t_eNBConfig);

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);
};
