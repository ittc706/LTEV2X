#pragma once
#include<fstream>
#include<vector>
#include"Enumeration.h"



/*===========================================
*            输出日志文件流声明
* ==========================================*/
extern std::ofstream g_FileTemp;
//GTT_Urban模块
extern std::ofstream g_FileVeUELocationUpdateLogInfo;
extern std::ofstream g_FileVeUENumPerRSULogInfo;
extern std::ofstream g_FileLocationInfo;
extern std::ofstream g_FileVeUEMessage;
//RRM模块
extern std::ofstream g_FileScheduleInfo;
extern std::ofstream g_FileClasterPerformInfo;
extern std::ofstream g_FileEventListInfo;
extern std::ofstream g_FileTTILogInfo;
extern std::ofstream g_FileEventLogInfo;
//TMC_B模块
extern std::ofstream g_FileStatisticsDescription;
extern std::ofstream g_FileEmergencyDelayStatistics;
extern std::ofstream g_FilePeriodDelayStatistics;
extern std::ofstream g_FileDataDelayStatistics;
extern std::ofstream g_FileEmergencyPossion;
extern std::ofstream g_FileDataPossion;
extern std::ofstream g_FileEmergencyConflictNum;
extern std::ofstream g_FilePeriodConflictNum;
extern std::ofstream g_FileDataConflictNum;
extern std::ofstream g_FileTTIThroughput;
extern std::ofstream g_FileRSUThroughput;
extern std::ofstream g_FilePackageLoss;
extern std::ofstream g_FilePackageTransimit;

void logFileConfig(Platform t_Platform);


/*===========================================
*          无线资源管理单元常量
* ==========================================*/
/*
* 10MHz，总带宽(Hz)
*/
const int gc_TotalBandwidth =10 * 1000 * 1000;

/*
* 每个RB的带宽(Hz)
*/
const int gc_BandwidthOfRB = 12 * 1000 * 15;

/*
* 单位(个),由于RB带宽为180kHz，TTI为1ms，因此单位TTI单位RB传输的比特数为180k*1ms=180
*/
const int gc_BitNumPerRB=180;

/*
* 调制方式
*/
const ModulationType gc_ModulationType = QPSK;

/*
* 信道编码码率
*/
const double gc_CodeRate = 0.5;

/*
* 不丢包传输的最小载干比
* 用于判断是否丢包之用
*/
const double gc_CriticalPoint = 1.99;//

/*===========================================
*          RRM_TDM_DRA模块常量定义
* ==========================================*/
namespace ns_RRM_TDM_DRA {
	/*
	* 所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)
	*/
	const int gc_NTTI = 100;

	/*
	* 事件的Pattern的类型种类
	* 即紧急事件，周期事件，数据业务事件
	*/
	const int gc_PatternTypeNum = 3;

	/*
	* 每个Pattern种类所占的RB数量
	*/
	const int gc_RBNumPerPatternType[gc_PatternTypeNum] = { 2,10,10 };

	/*
	* 每个Pattern种类对应的Pattern数量
	*/
	const int gc_PatternNumPerPatternType[gc_PatternTypeNum] = { 0,5,0 };

	/*
	* 每个种类的事件，其各自的Pattern的开始与结束编号，即[startIdx,endIdx]，闭区间
	*/
	const int gc_PatternTypePatternIdxInterval[gc_PatternTypeNum][2] = {
		{ 0,gc_PatternNumPerPatternType[0] - 1 },
		{ gc_PatternNumPerPatternType[0],gc_PatternNumPerPatternType[0] + gc_PatternNumPerPatternType[1] - 1 },
		{ gc_PatternNumPerPatternType[0] + gc_PatternNumPerPatternType[1],gc_PatternNumPerPatternType[0] + gc_PatternNumPerPatternType[1] + gc_PatternNumPerPatternType[2] - 1 },
	};

	/*
	* 所有Pattern数量，包括三个事件
	*/
	const int gc_TotalPatternNum = [&]() {
		int res = 0;
		for (int num : gc_PatternNumPerPatternType)
			res += num;
		return res;
	}();
}


/*===========================================
*          RRM_RR模块常量定义
* ==========================================*/
namespace ns_RRM_RR {
	/*
	* 每个Pattern的RB数量
	*/
	const int gc_RBNumPerPattern = 10;

	/*
	* 总的Pattern数量
	*/
	const int gc_TotalPatternNum = gc_TotalBandwidth / gc_BandwidthOfRB / gc_RBNumPerPattern;
}


/*===========================================
*          RRM_ICC_DRA模块常量定义
* ==========================================*/
namespace ns_RRM_ICC_DRA {
	/*
	* 每个Pattern的RB数量
	*/
	const int gc_RBNumPerPattern = 10;

	/*
	* 总的Pattern数量
	*/
	const int gc_TotalPatternNum = gc_TotalBandwidth / gc_BandwidthOfRB / gc_RBNumPerPattern;
}



/*===========================================
*          数据业务与统计单元常量
* ==========================================*/
/*
* 紧急事件/周期事件/数据业务事件 的数据包数量
* 下标以MessageType的定义为准
*/
const std::vector<int> gc_MessagePackageNum = { 4,4,4 };

/*
* 紧急事件/周期事件/数据业务事件 每个数据包的bit数量
* 下标以MessageType的定义为准
*/
const std::vector<std::vector<int>> gc_MessageBitNumPerPackage{
	{ 1520,1520,1520,2400 },
	{ 1520,1520,1520,2400 },
	{ 1520,1520,1520,2400 },
};

/*
* 紧急事件/周期事件/数据业务事件 初始的退避窗大小
* 下标以MessageType的定义为准
*/
const std::vector<int> gc_InitialWindowSize = { 5,5,5 };

/*
* 紧急事件/周期事件/数据业务事件 最大的退避窗大小
* 下标以MessageType的定义为准
*/
const std::vector<int> gc_MaxWindowSize = { 20,20,20 };





