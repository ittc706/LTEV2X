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

/*
* 前置声明
*/
class GTT_VeUE;
class RRM_VeUE;
class WT_VeUE;
class TMC_VeUE;

/*
* 每一个单元都有该单元对应的VeUE视图
* 该类的作用就是提供不同单元之间VeUE实体的信息交互
*/

class VeUE {
	friend class System;
	/*------------------域------------------*/
private:
	/*
	* 指向GTT视图下的VeUE实体对象
	*/
	GTT_VeUE * m_GTT = nullptr;

	/*
	* 指向RRM视图下的VeUE实体对象
	*/
	RRM_VeUE * m_RRM = nullptr;

	/*
	* 指向WT视图下的VeUE实体对象
	*/
	WT_VeUE * m_WT = nullptr;

	/*
	* 指向TMC视图下的VeUE实体对象
	*/
	TMC_VeUE * m_TMC = nullptr;

	/*------------------方法------------------*/
public:
	/*
	* 指向GTT视图下的VeUE的指针
	*/
	GTT_VeUE* getGTTPoint() { return m_GTT; }

	/*
	* 指向RRM视图下的VeUE的指针
	*/
	RRM_VeUE* getRRMPoint() { return m_RRM; }

	/*
	* 指向WT视图下的VeUE的指针
	*/
	WT_VeUE* getWTPoint() { return m_WT; }

	/*
	* 指向TMC视图下的VeUE的指针
	*/
	TMC_VeUE* getTMCPoint() { return m_TMC; }
};






