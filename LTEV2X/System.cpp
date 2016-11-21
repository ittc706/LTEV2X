#include<iomanip>
#include<fstream>
#include<stdlib.h>
#include"System.h"
#include"Global.h"
#include"RSU.h"
#include"Function.h"


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
		g_FilePackageLoss,
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
	ifstream inPlatformWindows("Config\\systemConfig.html"),
		inPlatformLinux("Config/systemConfig.html");
	
	if (inPlatformWindows.is_open()) {
		m_Config.platform = Windows;
		cout << "您当前的平台为：Windows" << endl;
		configLoader.initialize("Config\\systemConfig.html");
	}
	else if (inPlatformLinux.is_open()) {
		m_Config.platform = Linux;
		cout << "您当前的平台为：Linux" << endl;
		configLoader.initialize("Config/systemConfig.html");
	}
	else
		throw Exp("PlatformError");

	//初始化输出流对象
	logFileConfig(m_Config.platform);

	//开始解析配置文件
	configLoader.load();//解析配置文件

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
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("periodicEventNTTI")) != nullString) {
		ss << temp;
		ss >> m_Config.periodicEventNTTI;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");


	if ((temp = configLoader.getParam("emergencyLambda")) != nullString) {
		ss << temp;
		ss >> m_Config.emergencyLambda;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("dataLambda")) != nullString) {
		ss << temp;
		ss >> m_Config.dataLambda;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("locationUpdateNTTI")) != nullString) {
		ss << temp;
		ss >> m_Config.locationUpdateNTTI;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("GTTMode")) != nullString) {
		if (temp == "URBAN") {
			m_GTTMode = URBAN;
			cout << "GTT单元：URBAN模式" << endl;
		}
		else if (temp == "HIGHSPEED") {
			m_GTTMode = HIGHSPEED;
			cout << "GTT单元：HIGHSPEED模式" << endl;
		}
		else
			throw Exp("地理拓扑单元参数配置错误");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("RRMMode")) != nullString) {
		if (temp == "TDM_DRA") {
			m_RRMMode = TDM_DRA;
			cout << "RRM单元：TDM_DRA模式" << endl;
		}
		else if (temp == "ICC_DRA") {
			m_RRMMode = ICC_DRA;
			cout << "RRM单元：ICC_DRA模式" << endl;
		}
		else if (temp == "RR") {
			m_RRMMode = RR;
			cout << "RRM单元：RR模式" << endl;
		}
		else
			throw Exp("无限资源管理单元参数配置错误");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("ThreadNum")) != nullString) {
		ss << temp;
		ss >> m_Config.threadNum;
		ss.clear();//清除标志位
		ss.str("");
		cout << "开辟的线程数量为: " << m_Config.threadNum << endl;
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("WTMode")) != nullString) {
		if (temp == "SINR_MRC") {
			m_WTMode = SINR_MRC;
			cout << "WT单元：SINR_MRC模式" << endl;
		}
		else if (temp == "SINR_MMSE") {
			m_WTMode = SINR_MMSE;
			cout << "WT单元：SINR_MMSE模式" << endl;
		}
		else
			throw Exp("无线传输单元参数配置错误");
	}
	else
		throw Exp("ConfigLoaderError");
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
}


void System::initializeGTTModule() {
	switch (m_GTTMode) {
	case URBAN:
		m_GTTPoint = new GTT_Urban(m_TTI, m_Config, m_eNBAry, m_RoadAry, m_RSUAry, m_VeUEAry);
		break;
	case HIGHSPEED:
		m_GTTPoint = new GTT_HighSpeed(m_TTI, m_Config, m_eNBAry, m_RoadAry, m_RSUAry, m_VeUEAry);
		break;
	}
	//初始化地理拓扑参数
	m_GTTPoint->configure();

	//初始化eNB、Rode、RSU、VUE等容器
	m_GTTPoint->initialize();
}

void System::initializeWTModule() {
	m_WTPoint = new WT_B(m_Config, m_RSUAry, m_VeUEAry, m_WTMode);
	m_WTPoint->initialize();//模块初始化
}


void System::initializeRRMModule() {
	switch (m_RRMMode) {
	case RR:
		m_RRMPoint = new RRM_RR(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, m_Config.threadNum);
		break;
	case TDM_DRA:
		m_RRMPoint = new RRM_TDM_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, m_Config.threadNum);
		break;
	case ICC_DRA:
		m_RRMPoint = new RRM_ICC_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, m_Config.threadNum);
		break;
	default:
		break;
	}
	m_RRMPoint->initialize();//模块初始化
}


void System::initializeTMCModule() {
	m_TMCPoint = new TMC_B(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput);
	m_TMCPoint->initialize();//模块初始化
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

