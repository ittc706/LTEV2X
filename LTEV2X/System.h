#pragma once
#include<vector>
#include<fstream>
#include"Config.h"
#include"eNB.h"
#include"RSU.h"
#include"VUE.h"
#include"Event.h"
#include"Road.h"
#include"GTAT.h"
#include"GTAT_Urban.h"
#include"RRM.h"
#include"RRM_DRA.h"
#include"RRM_RR.h"
#include"TMAC.h"
#include"TMAC_B.h"

class cSystem{
public:
	/*------------------数据成员------------------*/
	int m_TTI;//当前的TTI时刻
	sConfigure m_Config;//系统参数配置
	ceNB* m_eNBAry;//基站容器
	cRoad* m_RoadAry;//道路容器
	cRSU* m_RSUAry;//RSU容器
	cVeUE* m_VeUEAry;//VeUE容器

	std::vector<sEvent> m_EventVec;//事件容器，下标代表事件ID
	std::vector<std::list<int>> m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	std::vector<std::vector<int>> m_TTIRSUThroughput;//吞吐率，外层下标为TTI，内层下标为RSUId


	/*-----------------模块控制器-----------------*/
	//地理拓扑与传输单元
	eGTATMode m_GTATMode;
	GTAT_Basic* GTATPoint;


	//业务模型与控制单元
	TMAC_Basic* TMACPoint;

	//无限资源管理单元
	eRRMMode m_RRMMode;//调度模式选择
	RRM_Basic* RRMPoint;

	/*------------------成员函数------------------*/

	void configure();//系统仿真参数配置
	void initialization();//系统参数配置，完成系统初始化
	void GTATModuleInitialize();//GTAT模块对象初始化
	void RRMModuleInitialize();//RRM模块对象初始化
	void TMACmODULEInitialize();//TMAC模块对象初始化
	void process();//系统仿真流程
	void dispose();//内存清理
};



