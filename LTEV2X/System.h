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
#include"GTAT_HighSpeed.h"
#include"WT.h"
#include"WT_B.h"
#include"RRM.h"
#include"RRM_DRA.h"
#include"RRM_RR.h"
#include"TMAC.h"
#include"TMAC_B.h"

class System{
public:
	/*------------------数据成员------------------*/
	int m_TTI;//当前的TTI时刻
	Configure m_Config;//系统参数配置
	eNB* m_eNBAry;//基站容器
	Road* m_RoadAry;//道路容器
	RSU* m_RSUAry;//RSU容器
	VeUE* m_VeUEAry;//VeUE容器

	std::vector<Event> m_EventVec;//事件容器，下标代表事件ID
	std::vector<std::list<int>> m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	std::vector<std::vector<int>> m_TTIRSUThroughput;//吞吐率，外层下标为TTI，内层下标为RSUId


	/*-----------------模块控制器-----------------*/
	//地理拓扑与传输单元
	GTATMode m_GTATMode;//地理拓扑模式选择
	GTAT_Basic* m_GTATPoint;

    //无线传输单元
	WT_Basic* m_WTPoint;

	//业务模型与控制单元
	TMAC_Basic* m_TMACPoint;

	//无限资源管理单元
	RRMMode m_RRMMode;//调度模式选择
	RRM_Basic* m_RRMPoint;


	/*------------------成员函数------------------*/
	void process();//系统仿真流程

private:
	void configure();//系统仿真参数配置
	void initialization();//系统参数配置，完成系统初始化
	void GTATModuleInitialize();//GTAT模块对象初始化
	void WTModuleInitialize();//WT模块对象初始化
	void RRMModuleInitialize();//RRM模块对象初始化
	void TMACModuleInitialize();//TMAC模块对象初始化
	void dispose();//内存清理
};



