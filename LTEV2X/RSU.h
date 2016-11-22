#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include<tuple>
#include"Global.h"
#include"VUE.h"
#include"Event.h"
#include"IMTA.h"
#include"Exception.h"

/*
* 前置声明
*/
class GTT_RSU;
class RRM_RSU;
class WT_RSU;
class TMC_RSU;

/*
* 每一个单元都有该单元对应的RSU视图
* 该类的作用就是提供不同单元之间RSU实体的信息交互
*/

class RSU {
	friend class System;
	/*------------------域------------------*/
private:
	/*
	* 指向GTT视图下的VeUE实体对象
	*/
	GTT_RSU * m_GTT = nullptr;

	/*
	* 指向RRM视图下的VeUE实体对象
	*/
	RRM_RSU * m_RRM = nullptr;

	/*
	* 指向WT视图下的VeUE实体对象
	*/
	WT_RSU * m_WT = nullptr;

	/*
	* 指向TMC视图下的VeUE实体对象
	*/
	TMC_RSU * m_TMC = nullptr;

	/*------------------方法------------------*/
public:
	/*
	* 指向GTT视图下的VeUE的指针
	*/
	GTT_RSU* getGTTPoint() { return m_GTT; }

	/*
	* 指向RRM视图下的VeUE的指针
	*/
	RRM_RSU* getRRMPoint() { return m_RRM; }

	/*
	* 指向WT视图下的VeUE的指针
	*/
	WT_RSU* getWTPoint() { return m_WT; }

	/*
	* 指向TMC视图下的VeUE的指针
	*/
	TMC_RSU* getTMCPoint() { return m_TMC; }
};




