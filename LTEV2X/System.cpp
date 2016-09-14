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
	TMACPoint->buildEventList(g_FileEventListInfo);

	//开始仿真
	for (int count = 0;count < m_Config.NTTI;count++) {
		cout << "Current TTI = " << m_TTI << endl;
		//地理位置更新
		if (count % m_Config.locationUpdateNTTI == 0)
			GTATPoint->channelGeneration();
		//开始资源分配
		RRMPoint->schedule();
		m_TTI++;
	}

	//处理各项业务时延数据
	TMACPoint->processStatistics(g_FileDelayStatistics, g_FileEmergencyPossion, g_FileDataPossion, g_FileConflictNum, g_FileEventLogInfo);

	//打印车辆地理位置更新日志信息
	GTATPoint->writeVeUELocationUpdateLogInfo(g_FileVeUELocationUpdateLogInfo);
}

void cSystem::configure() {//系统仿真参数配置

	m_Config.NTTI = 20000;//仿真TTI时间
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
	GTATPoint->configure();

	//初始化eNB、Rode、RSU、VUE等容器
	GTATPoint->initialize();

	//RRM模块初始化
	RRMModuleInitialize();

	//TMAC模块初始化
	TMACmODULEInitialize();
}


void cSystem::GTATModuleInitialize() {
	switch (m_GTATMode) {
	case URBAN:
		GTATPoint = new GTAT_Urban(m_TTI, m_Config, m_eNBAry, m_RoadAry, m_RSUAry, m_VeUEAry);
		break;
	}
}


void cSystem::RRMModuleInitialize() {
	switch (m_RRMMode) {
	case RR:
		RRMPoint = new RRM_RR(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput);
		break;
	case DRA:
		RRMPoint = new RRM_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, P123);
		break;
	default:
		break;
	}
}


void cSystem::TMACmODULEInitialize() {
	TMACPoint = new TMAC_B(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput);
}


void cSystem::dispose() {
	delete TMACPoint;
	delete RRMPoint;
}

