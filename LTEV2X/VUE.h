#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include<random>
#include<map>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"

class GTT_VeUE;
class RRM_VeUE;

class VeUE {
	
	

	/*---------------前置声明---------------*/
public:
	class TMC;

	/*------------------域------------------*/
public:

	GTT_VeUE * m_GTT = nullptr;
	RRM_VeUE * m_RRM = nullptr;


	/*
	* TMC内部类指针
	* 用于存储不同实现的共同参数
	*/
	TMC* m_TMC = nullptr;

	/*------------------方法------------------*/
public:
	/*
	* 析构函数
	* 析构内部类指针
	*/
	~VeUE();
	
	/*
	* TMC模块视图中，RSU对象初始化方法
	* 在WT模块初始化时调用
	*/
	void initializeTMC();

	/*-----------------内部类-----------------*/
public:

	class TMC {
		/*--------域--------*/
	public:
		/*
		* 地理位置更新日志信息
		*/
		std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;
	};
};






