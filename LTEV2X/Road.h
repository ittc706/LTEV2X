#pragma once

#include<iostream>
#include<fstream>
#include<string>
#include<list>
#include<sstream>
#include"Enumeration.h"
#include"Config.h"
#include"Global.h"
#include"eNB.h"


class Road{
	/*---------------前置声明---------------*/
public:
	class GTT;
	class GTT_Urban;
	class GTT_HighSpeed;
	/*------------------域------------------*/
public:
	/*
	* GTT内部类指针
	* 用于存储两个场景公共的参数
	*/
	GTT* m_GTT = nullptr;

	/*
	* GTT_Urban内部类指针
	* 用于存储城镇场景的特定参数
	*/
	GTT_Urban* m_GTT_Urban = nullptr;

	/*
	* GTT_HighSpeed内部类指针
	* 用于存储高速场景的特定参数
	*/
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;

	/*------------------方法------------------*/
public:
	/*
	* 默认构造函数
	* 初始化内部类指针
	*/
	Road();

	/*
	* 默认构造函数
	* 析构内部类指针
	*/
	~Road();

	/*
	* 城镇场景下，道路对象初始化方法
	* 在GTT模块初始化时调用
	*/
	void initializeUrban(UrbanRoadConfig &t_RoadConfig);

	/*
	* 高速场景下，道路对象初始化方法
	* 在GTT模块初始化时调用
	*/
	void initializeHighSpeed(HighSpeedRodeConfig &t_RoadHighSpeedConfig);

	/*-----------------内部类-----------------*/
	class GTT{
		/*--------域--------*/
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

		/*-------方法-------*/
	public:
		/*
		* 生成格式化字符串
		*/
		std::string toString(int t_NumTab);
	};

	class GTT_Urban {
		/*--------域--------*/
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
		eNB *m_eNB;
	};

	class GTT_HighSpeed {
	public:

	};
};