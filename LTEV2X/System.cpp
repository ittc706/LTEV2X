#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"
#include"RSU.h"


using namespace std;

void System::process() {
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
	cout << "干扰信道计算耗时：" << m_RRMPoint->m_GTTTimeConsume / 1000.0L << " s\n" << endl;
	cout << "SINR计算耗时：" << m_RRMPoint->m_WTTimeConsume / 1000.0L << " s\n" << endl;
	cout.unsetf(ios::fixed);

	//处理各项业务时延数据
	m_TMCPoint->processStatistics(g_FileDelayStatistics, g_FileEmergencyPossion, g_FileDataPossion, g_FileConflictNum, g_FileEventLogInfo);

	//打印车辆地理位置更新日志信息
	m_GTTPoint->writeVeUELocationUpdateLogInfo(g_FileVeUELocationUpdateLogInfo, g_FileVeUENumPerRSULogInfo);
}

void System::configure() {//系统仿真参数配置

	m_Config.NTTI = 20;//仿真TTI时间
	m_Config.periodicEventNTTI = 500;
	m_Config.emergencyLambda = 0;// 0.001;
	m_Config.dataLambda = 0;
	m_Config.locationUpdateNTTI = 1000;

	//地理拓扑与传输模式
	m_GTTMode = URBAN;

	//无线资源管理模式
	m_RRMMode = TDM_DRA;
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
	m_WTPoint = new WT_B(m_Config, m_RSUAry, m_VeUEAry);
	m_WTPoint->initialize();//模块初始化
}


void System::initializeRRMModule() {
	switch (m_RRMMode) {
	case RR:
		m_RRMPoint = new RRM_RR(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, 4);
		break;
	case TDM_DRA:
		m_RRMPoint = new RRM_TDM_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, 4);
		break;
	case ICC_DRA:
		m_RRMPoint = new RRM_ICC_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, 4);
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
	if (m_TMCPoint != nullptr) {
		delete m_TMCPoint;
		m_TMCPoint = nullptr;
	}
	if (m_RRMPoint != nullptr) {
		delete m_RRMPoint;
		m_RRMPoint = nullptr;
	}
	if (m_GTTPoint != nullptr) {
		delete m_GTTPoint;
		m_GTTPoint = nullptr;
	}
	if (m_WTPoint != nullptr) {
		delete m_WTPoint;
		m_WTPoint = nullptr;
	}

	//清理各实体类数组
	if (m_eNBAry != nullptr) {
		delete[] m_eNBAry;
		m_eNBAry = nullptr;
	}
	if (m_RSUAry != nullptr) {
		delete[] m_RSUAry;
		m_RSUAry = nullptr;
	}
	if (m_VeUEAry != nullptr) {
		delete[] m_VeUEAry;
		m_VeUEAry = nullptr;
	}
	if (m_RoadAry != nullptr) {
		delete[] m_RoadAry;
		m_RoadAry = nullptr;
	}
}

