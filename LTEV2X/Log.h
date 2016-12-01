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
//TMC模块
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




