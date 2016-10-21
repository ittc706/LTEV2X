#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"
#include"RSU.h"


using namespace std;

void cSystem::process() {
	//参数配置
	configure();

	//仿真初始化
	initialization();

	//创建事件链表
	m_TMACPoint->buildEventList(g_FileEventListInfo);

	//开始仿真
	for (int count = 0;count < m_Config.NTTI;count++) {
		cout << "Current TTI = " << m_TTI << endl;
		//地理位置更新
		if (count % m_Config.locationUpdateNTTI == 0)
			m_GTATPoint->channelGeneration();
		//开始资源分配
		m_RRMPoint->schedule();
		m_TTI++;
	}

	//处理各项业务时延数据
	m_TMACPoint->processStatistics(g_FileDelayStatistics, g_FileEmergencyPossion, g_FileDataPossion, g_FileConflictNum, g_FileEventLogInfo);

	//打印车辆地理位置更新日志信息
	m_GTATPoint->writeVeUELocationUpdateLogInfo(g_FileVeUELocationUpdateLogInfo, g_FileVeUENumPerRSULogInfo);
}

void cSystem::configure() {//系统仿真参数配置

	m_Config.NTTI = 20;//仿真TTI时间
	m_Config.periodicEventNTTI = 500;
	m_Config.emergencyLambda = 0.0001;// 0.001;
	m_Config.dataLambda = 0.0001;
	m_Config.locationUpdateNTTI = 1000;

	//地理拓扑与传输模式
	m_GTATMode = URBAN;

	//无线资源管理模式
	m_RRMMode = DRA;
}


void cSystem::initialization() {
	m_TTI = 0;	

	//GTAT模块初始化
	GTATModuleInitialize();

	//初始化地理拓扑参数
	m_GTATPoint->configure();

	//初始化eNB、Rode、RSU、VUE等容器
	m_GTATPoint->initialize();

	//WT模块初始化
	WTModuleInitialize();

	//RRM模块初始化
	RRMModuleInitialize();

	//TMAC模块初始化
	TMACmODULEInitialize();
}


void cSystem::GTATModuleInitialize() {
	switch (m_GTATMode) {
	case URBAN:
		m_GTATPoint = new GTAT_Urban(m_TTI, m_Config, m_eNBAry, m_RoadAry, m_RSUAry, m_VeUEAry);
		break;
	}
}

void cSystem::WTModuleInitialize() {
	m_WTPoint = new WT_B(m_VeUEAry);
}


void cSystem::RRMModuleInitialize() {
	switch (m_RRMMode) {
	case RR:
		m_RRMPoint = new RRM_RR(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput);
		break;
	case DRA:
		m_RRMPoint = new RRM_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, P123, m_WTPoint, m_GTATPoint);
		break;
	default:
		break;
	}
}


void cSystem::TMACmODULEInitialize() {
	m_TMACPoint = new TMAC_B(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput);
}


void cSystem::dispose() {
	delete m_TMACPoint;
	delete m_RRMPoint;
	delete m_GTATPoint;
	delete m_WTPoint;
}

