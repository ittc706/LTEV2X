#pragma once
#include<vector>
#include<fstream>
#include"Config.h"
#include"Event.h"
/*
* 前置声明
* 由于System会被各个模块包含，因此System.h尽量避免包含其他模块
* 在.cpp里面包含即可
*/
class GTT;
class RRM;
class TMC;
class WT;

class VeUE;
class RSU;
class eNB;
class Road;

class System{
	/*
	* 声明友元，System所有域禁止外部直接访问
	*/
	friend class GTT;
	friend class GTT_Urban;
	friend class GTT_HighSpeed;
	friend class RRM;
	friend class RRM_TDM_DRA;
	friend class RRM_ICC_DRA;
	friend class RRM_RR;
	friend class TMC;
	friend class WT;
	/*------------------域------------------*/
private:
	/*
	* 系统当前的TTI时刻
	*/
	int m_TTI;

	/*
	* 系统参数配置
	*/
	SystemConfig m_Config;

	/*
	* 四个实体类容器
	* 分别是基站，道路，RSU，车辆
	*/
	eNB* m_eNBAry = nullptr;
	Road* m_RoadAry = nullptr;
	RSU* m_RSUAry = nullptr;
	VeUE* m_VeUEAry = nullptr;


	/*
	* 模块控制器
	* GTT模块，RRM模块，WT模块，TMC模块
	*/
	GTT* m_GTTPoint = nullptr;
	RRM* m_RRMPoint = nullptr;
	TMC* m_TMCPoint = nullptr;
	WT* m_WTPoint = nullptr;

	/*------------------方法------------------*/
public:
	/*
	* 系统仿真流程总控
	*/	
	void process();

	/*
	* 析构函数，负责各个实体类的清理工作
	*/
	~System();
private:
	/*
	* 系统仿真参数配置
	*/
	void configure();

	/*
	* 系统参数配置，完成系统初始化
	*/
	void initialization();

	/*
	* GTT模块对象初始化
	* 被initialization()调用
	*/
	void initializeGTTModule();

	/*
	* WT模块对象初始化
	* 被initialization()调用
	*/
	void initializeWTModule();

	/*
	* RRM模块对象初始化
	* 被initialization()调用
	*/
	void initializeRRMModule();

	/*
	* TMC模块对象初始化
	* 被initialization()调用
	*/
	void initializeTMCModule();

	/*
	* 建立中转实体对象初始化
	*/
	void initializeBuildConnection();
};



