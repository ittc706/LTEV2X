#pragma once
#include<vector>
#include<fstream>
#include"ConfigLoader.h"
#include"Config.h"
#include"eNB.h"
#include"RSU.h"
#include"VUE.h"
#include"Event.h"
#include"Road.h"
#include"GTT.h"
#include"GTT_Urban.h"
#include"GTT_HighSpeed.h"
#include"WT.h"
#include"WT_B.h"
#include"RRM.h"
#include"RRM_TDM_DRA.h"
#include"RRM_ICC_DRA.h"
#include"RRM_RR.h"
#include"TMC.h"
#include"TMC_B.h"

class System{
	/*------------------域------------------*/
public:
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
	* 事件容器，下标代表事件ID
	*/
	std::vector<Event> m_EventVec;

	/*
	* 以TTI为下标的事件容器
	* 事件触发链表，m_EventTTIList[i]代表第i个TTI的事件表
	*/
	std::vector<std::list<int>> m_EventTTIList;

	/*
	* 吞吐率
	* 外层下标为TTI，内层下标为RSUId
	*/
	std::vector<std::vector<int>> m_TTIRSUThroughput;


	/*
	* 模块控制器
	* GTT模块，RRM模块，WT模块，TMC模块
	*/
	GTT_Basic* m_GTTPoint = nullptr;
	RRM_Basic* m_RRMPoint = nullptr;
	TMC_Basic* m_TMCPoint = nullptr;
	WT_Basic* m_WTPoint = nullptr;

	/*
	* 模块实现的具体类别
	*/
	GTTMode m_GTTMode;
	WTMode m_WTMode;
	RRMMode m_RRMMode;


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
};



