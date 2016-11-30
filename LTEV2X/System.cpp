/*
* =====================================================================================
*
*       Filename:  System.cpp
*
*    Description:  TMC模块
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<iomanip>
#include<fstream>
#include<sstream>
#include<stdlib.h>
#include<stdexcept>
#include"System.h"

#include"GTT.h"
#include"GTT_HighSpeed.h"
#include"GTT_Urban.h"

#include"RRM.h"
#include"RRM_ICC_DRA.h"
#include"RRM_TDM_DRA.h"
#include"RRM_RR.h"

#include"TMC.h"
#include"TMC_B.h"

#include"WT.h"
#include"WT_B.h"

#include"VUE.h"
#include"RSU.h"
#include"eNB.h"
#include"Road.h"

#include"Config.h"
#include"Function.h"
#include"ConfigLoader.h"
#include"Log.h"


using namespace std;

void System::process() {
	long double programStart = clock();

	//参数配置
	configure();

	//仿真初始化
	initialization();

	//创建事件链表
	m_TMCPoint->buildEventList(g_FileEventListInfo);

	//开始仿真
	for (int count = 0;count < m_Config.NTTI;count++) {
		cout << "Current TTI = " << m_TTI << endl;
		//地理位置更新
		if (count % m_Config.locationUpdateNTTI == 0) {
			m_GTTPoint->channelGeneration();
			m_GTTPoint->cleanWhenLocationUpdate();
			m_RRMPoint->cleanWhenLocationUpdate();
		}
		//开始资源分配
		m_RRMPoint->schedule();
		m_TTI++;
	}

	cout.setf(ios::fixed);
	double timeFactor;
	if (m_Config.platform == Windows)
		timeFactor = 1000L;
	else
		timeFactor = 1000000L;
	cout << "干扰信道计算耗时：" << m_RRMPoint->m_GTTTimeConsume / timeFactor << " s\n" << endl;
	cout << "SINR计算耗时：" << m_RRMPoint->m_WTTimeConsume / timeFactor << " s\n" << endl;
	cout.unsetf(ios::fixed);

	//处理各项业务时延数据
	m_TMCPoint->processStatistics(
		g_FileStatisticsDescription,
		g_FileEmergencyDelayStatistics, g_FilePeriodDelayStatistics, g_FileDataDelayStatistics,
		g_FileEmergencyPossion, g_FileDataPossion,
		g_FileEmergencyConflictNum, g_FilePeriodConflictNum, g_FileDataConflictNum,
		g_FilePackageLoss, g_FilePackageTransimit,
		g_FileEventLogInfo);

	//打印车辆地理位置更新日志信息
	m_GTTPoint->writeVeUELocationUpdateLogInfo(g_FileVeUELocationUpdateLogInfo, g_FileVeUENumPerRSULogInfo);

	//整个程序计时
	long double programEnd = clock();
	cout.setf(ios::fixed);
	cout << "\nRunning Time :" << setprecision(1) << (programEnd - programStart) / timeFactor << " s\n" << endl;
	cout.unsetf(ios::fixed);
}

void System::configure() {//系统仿真参数配置
	srand((unsigned)time(NULL));//设置真个仿真的随机数种子

	ConfigLoader configLoader;

	//首先先判断当前的平台，利用路径的表示在两个平台下的差异来判断
	ifstream inPlatformWindows("Config\\SystemConfig.xml"),
		inPlatformLinux("Config/SystemConfig.xml");
	
	if (inPlatformWindows.is_open()) {
		m_Config.platform = Windows;
		cout << "您当前的平台为：Windows" << endl;	
	}
	else if (inPlatformLinux.is_open()) {
		m_Config.platform = Linux;
		cout << "您当前的平台为：Linux" << endl;	
	}
	else
		throw logic_error("PlatformError");

	//初始化输出流对象
	logFileConfig(m_Config.platform);

	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>开始解析系统配置文件<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	switch (m_Config.platform) {
	case Windows:
		configLoader.resolvConfigPath("Config\\SystemConfig.xml");
		break;
	case Linux:
		configLoader.resolvConfigPath("Config/SystemConfig.xml");
		break;
	default:
		throw logic_error("Platform Config Error!");
	}

	stringstream ss;

	const string nullString("");
	string temp;

	if ((temp = configLoader.getParam("NTTI")) != nullString) {
		ss << temp;
		ss >> m_Config.NTTI;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");


	if ((temp = configLoader.getParam("locationUpdateNTTI")) != nullString) {
		ss << temp;
		ss >> m_Config.locationUpdateNTTI;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("GTTMode")) != nullString) {
		if (temp == "URBAN") {
			m_Config._GTTMode = URBAN;
			cout << "GTT单元：URBAN模式" << endl;
		}
		else if (temp == "HIGHSPEED") {
			m_Config._GTTMode = HIGHSPEED;
			cout << "GTT单元：HIGHSPEED模式" << endl;
		}
		else
			throw logic_error("地理拓扑单元参数配置错误");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("RRMMode")) != nullString) {
		if (temp == "TDM_DRA") {
			m_Config._RRMMode = TDM_DRA;
			cout << "RRM单元：TDM_DRA模式" << endl;
		}
		else if (temp == "ICC_DRA") {
			m_Config._RRMMode = ICC_DRA;
			cout << "RRM单元：ICC_DRA模式" << endl;
		}
		else if (temp == "RR") {
			m_Config._RRMMode = RR;
			cout << "RRM单元：RR模式" << endl;
		}
		else
			throw logic_error("无限资源管理单元参数配置错误");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("ThreadNum")) != nullString) {
		ss << temp;
		ss >> m_Config.threadNum;
		ss.clear();//清除标志位
		ss.str("");
		cout << "开辟的线程数量为: " << m_Config.threadNum << endl;
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("WTMode")) != nullString) {
		if (temp == "SINR_MRC") {
			m_Config._WTMode = SINR_MRC;
			cout << "WT单元：SINR_MRC模式" << endl;
		}
		else if (temp == "SINR_MMSE") {
			m_Config._WTMode = SINR_MMSE;
			cout << "WT单元：SINR_MMSE模式" << endl;
		}
		else
			throw logic_error("无线传输单元参数配置错误");
	}
	else
		throw logic_error("ConfigLoaderError");


	/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>开始解析日志配置文件<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
	switch (m_Config.platform) {
	case Windows:
		configLoader.resolvConfigPath("Config\\LogControlConfig.xml");
		break;
	case Linux:
		configLoader.resolvConfigPath("Config/LogControlConfig.xml");
		break;
	default:
		throw logic_error("Platform Config Error!");
	}

	if ((temp = configLoader.getParam("TTILog")) != nullString) {
		if (temp == "ON")
			m_Config.TTILogIsOn = true;
		else if (temp == "OFF")
			m_Config.TTILogIsOn = false;
		else
			throw logic_error("TTI日志参数配置错误");
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("EventLog")) != nullString) {
		if (temp == "ON")
			m_Config.eventLogIsOn = true;
		else if (temp == "OFF")
			m_Config.eventLogIsOn = false;
		else
			throw logic_error("Event日志参数配置错误");
		ss.clear();//清除标志位
		ss.str("");
		Event::s_LogIsOn = m_Config.eventLogIsOn;
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("ScheduleLog")) != nullString) {
		if (temp == "ON")
			m_Config.scheduleLogIsOn = true;
		else if (temp == "OFF")
			m_Config.scheduleLogIsOn = false;
		else
			throw logic_error("Schedule日志参数配置错误");
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");


	//读取各个模块的配置文件
	GTT_Urban::loadConfig(m_Config.platform);

	GTT_HighSpeed::loadConfig(m_Config.platform);

	TMC::loadConfig(m_Config.platform);
}


void System::initialization() {
	m_TTI = 0;	

	//GTT模块初始化
	initializeGTTModule();

	//WT模块初始化
	initializeWTModule();

	//RRM模块初始化
	initializeRRMModule();

	//TMC模块初始化
	initializeTMCModule();

	initializeBuildConnection();
}


void System::initializeGTTModule() {
	switch (m_Config._GTTMode) {
	case URBAN:
		m_GTTPoint = new GTT_Urban(this);
		break;
	case HIGHSPEED:
		m_GTTPoint = new GTT_HighSpeed(this);
		break;
	}
	//初始化地理拓扑参数
	m_GTTPoint->configure();

	//初始化eNB、Rode、RSU、VUE等容器
	m_GTTPoint->initialize();
}

void System::initializeWTModule() {
	m_WTPoint = new WT_B(this);
	m_WTPoint->initialize();//模块初始化
}


void System::initializeRRMModule() {
	switch (m_Config._RRMMode) {
	case RR:
		m_RRMPoint = new RRM_RR(this);
		break;
	case TDM_DRA:
		m_RRMPoint = new RRM_TDM_DRA(this);
		break;
	case ICC_DRA:
		m_RRMPoint = new RRM_ICC_DRA(this);
		break;
	default:
		break;
	}
	m_RRMPoint->initialize();//模块初始化
}


void System::initializeTMCModule() {
	m_TMCPoint = new TMC_B(this);
	m_TMCPoint->initialize();//模块初始化
}


void System::initializeBuildConnection() {
	//系统VeUE与各个单元中VeUE视图建立关联
	m_VeUEAry = new VeUE[m_Config.VeUENum];
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].m_GTT = m_GTTPoint->m_VeUEAry[VeUEId];
		m_GTTPoint->m_VeUEAry[VeUEId]->setSystemPoint(&m_VeUEAry[VeUEId]);

		m_VeUEAry[VeUEId].m_RRM = m_RRMPoint->m_VeUEAry[VeUEId];
		m_RRMPoint->m_VeUEAry[VeUEId]->setSystemPoint(&m_VeUEAry[VeUEId]);

		m_VeUEAry[VeUEId].m_WT = m_WTPoint->m_VeUEAry[VeUEId];
		(m_WTPoint->m_VeUEAry[VeUEId])->setSystemPoint(&m_VeUEAry[VeUEId]);

		m_VeUEAry[VeUEId].m_TMC= m_TMCPoint->m_VeUEAry[VeUEId];
		m_TMCPoint->m_VeUEAry[VeUEId]->setSystemPoint(&m_VeUEAry[VeUEId]);
	}

	//系统RSU与各个单元中RSU视图建立关联
	m_RSUAry = new RSU[m_Config.RSUNum];
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].m_GTT = m_GTTPoint->m_RSUAry[RSUId];
		m_GTTPoint->m_RSUAry[RSUId]->setSystemPoint(&m_RSUAry[RSUId]);

		m_RSUAry[RSUId].m_RRM = m_RRMPoint->m_RSUAry[RSUId];
		m_RRMPoint->m_RSUAry[RSUId]->setSystemPoint(&m_RSUAry[RSUId]);

		m_RSUAry[RSUId].m_WT = m_WTPoint->m_RSUAry[RSUId];
		(m_WTPoint->m_RSUAry[RSUId])->setSystemPoint(&m_RSUAry[RSUId]);

		m_RSUAry[RSUId].m_TMC = m_TMCPoint->m_RSUAry[RSUId];
		m_TMCPoint->m_RSUAry[RSUId]->setSystemPoint(&m_RSUAry[RSUId]);

		//必须等到各个单元的RSU建立连接后，才能对RRM单元内的RSU对象进行初始化
		m_RRMPoint->m_RSUAry[RSUId]->initialize();
	}

	//系统eNB与各个单元中eNB视图建立关联
	m_eNBAry = new eNB[m_Config.eNBNum];
	for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
		m_eNBAry[eNBId].m_GTT = m_GTTPoint->m_eNBAry[eNBId];
		m_GTTPoint->m_eNBAry[eNBId]->setSystemPoint(&m_eNBAry[eNBId]);
	}

	//系统Road与各个单元中Road视图建立关联
	m_RoadAry = new Road[m_Config.RoadNum];
	for (int roadId = 0; roadId < m_Config.RoadNum; roadId++) {
		m_RoadAry[roadId].m_GTT = m_GTTPoint->m_RoadAry[roadId];
		m_GTTPoint->m_RoadAry[roadId]->setSystemPoint(&m_RoadAry[roadId]);
	}
}


System::~System() {
	//清理模块指针
	Delete::safeDelete(m_TMCPoint);
	Delete::safeDelete(m_RRMPoint);
	Delete::safeDelete(m_GTTPoint);
	Delete::safeDelete(m_WTPoint);

	//清理各实体类数组
	Delete::safeDelete(m_eNBAry, true);
	Delete::safeDelete(m_RSUAry, true);
	Delete::safeDelete(m_VeUEAry, true);
	Delete::safeDelete(m_RoadAry, true);

    //关闭文件流
	g_FileTemp.close();

	g_FileVeUELocationUpdateLogInfo.close();
	g_FileVeUENumPerRSULogInfo.close();
	g_FileLocationInfo.close();

	g_FileScheduleInfo.close();
	g_FileClasterPerformInfo.close();
	g_FileEventListInfo.close();
	g_FileTTILogInfo.close();
	g_FileEventLogInfo.close();

	g_FileEmergencyDelayStatistics.close();
	g_FileEmergencyPossion.close();
	g_FileDataPossion.close();
	g_FileEmergencyConflictNum.close();
	g_FileTTIThroughput.close();
	g_FileRSUThroughput.close();
}

