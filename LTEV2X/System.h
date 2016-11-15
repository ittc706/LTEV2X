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
public:
	/*------------------域------------------*/
	int m_TTI;//当前的TTI时刻
	SystemConfig m_Config;//系统参数配置
	eNB* m_eNBAry = nullptr;//基站容器
	Road* m_RoadAry = nullptr;//道路容器
	RSU* m_RSUAry = nullptr;//RSU容器
	VeUE* m_VeUEAry = nullptr;//VeUE容器

	std::vector<Event> m_EventVec;//事件容器，下标代表事件ID
	std::vector<std::list<int>> m_EventTTIList;//事件触发链表，m_EventList[i]代表第i个TTI的事件表
	std::vector<std::vector<int>> m_TTIRSUThroughput;//吞吐率，外层下标为TTI，内层下标为RSUId


	/*---模块控制器---*/
	GTTMode m_GTTMode;//地理拓扑模式选择
	GTT_Basic* m_GTTPoint = nullptr;//地理拓扑与传输单元

	WT_Basic* m_WTPoint = nullptr;//无线传输单元
	WTMode m_WTMode;

	TMC_Basic* m_TMCPoint = nullptr;//业务模型与控制单元

	RRMMode m_RRMMode;//调度模式选择
	RRM_Basic* m_RRMPoint = nullptr;//无限资源管理单元


	/*------------------方法------------------*/
public:/*---接口---*/
	void process();//系统仿真流程
	~System();
private:/*---实现---*/
	void configure();//系统仿真参数配置
	void initialization();//系统参数配置，完成系统初始化
	void initializeGTTModule();//GTT模块对象初始化
	void initializeWTModule();//WT模块对象初始化
	void initializeRRMModule();//RRM模块对象初始化
	void initializeTMCModule();//TMC模块对象初始化
};



