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
* 每一个类都有该类对应的VeUE视图
*/
class GTT_VeUE;
class RRM_VeUE;
class WT_VeUE;
class TMC_VeUE;

class VeUE {
	/*------------------域------------------*/
public:
	GTT_VeUE * m_GTT = nullptr;
	RRM_VeUE * m_RRM = nullptr;
	WT_VeUE * m_WT = nullptr;
	TMC_VeUE * m_TMC = nullptr;
};






