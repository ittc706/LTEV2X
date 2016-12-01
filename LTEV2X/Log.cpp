#include<iomanip>
#include<cmath>
#include"Log.h"
#include"Function.h"

using namespace std;

void logFileConfig(Platform t_Platform) {
	if (t_Platform == Windows) {
		/*===========================================
		*            输出日志文件流定义
		* ==========================================*/
		g_FileTemp.open("Log\\Temp.txt");
		//GTT模块
		g_FileVeUELocationUpdateLogInfo.open("Log\\GTTLog\\VeUELocationUpdateLogInfo.txt");
		g_FileVeUENumPerRSULogInfo.open("Log\\GTTLog\\VeUENumPerRSULogInfo.txt");
		g_FileLocationInfo.open("Log\\GTTLog\\LocationInfo.txt");
		g_FileVeUEMessage.open("Log\\GTTLog\\VeUEMessage.txt");
		//RRM模块
		g_FileScheduleInfo.open("Log\\RRMLog\\ScheduleInfo.txt");
		g_FileClasterPerformInfo.open("Log\\RRMLog\\ClasterPerformInfo.txt");
		g_FileEventListInfo.open("Log\\RRMLog\\EventListInfo.txt");
		g_FileTTILogInfo.open("Log\\RRMLog\\TTILogInfo.txt");
		g_FileEventLogInfo.open("Log\\RRMLog\\EventLogInfo.txt");
		//TMC模块
		g_FileStatisticsDescription.open("Log\\TMCLog\\StatisticsDescription.txt");
		g_FileEmergencyDelayStatistics.open("Log\\TMCLog\\EmergencyDelayStatistics.txt");
		g_FilePeriodDelayStatistics.open("Log\\TMCLog\\PeriodDelayStatistics.txt");
		g_FileDataDelayStatistics.open("Log\\TMCLog\\DataDelayStatistics.txt");
		g_FileEmergencyPossion.open("Log\\TMCLog\\EmergencyPossion.txt");
		g_FileDataPossion.open("Log\\TMCLog\\DataPossion.txt");
		g_FileEmergencyConflictNum.open("Log\\TMCLog\\EmergencyConflictNum.txt");
		g_FilePeriodConflictNum.open("Log\\TMCLog\\PeriodConflictNum.txt");
		g_FileDataConflictNum.open("Log\\TMCLog\\DataConflictNum.txt");
		g_FileTTIThroughput.open("Log\\TMCLog\\TTIThroughput.txt");
		g_FileRSUThroughput.open("Log\\TMCLog\\RSUThroughput.txt");
		g_FilePackageLoss.open("Log\\TMCLog\\PackageLoss.txt");
		g_FilePackageTransimit.open("Log\\TMCLog\\PackageTransimit.txt");
	}
	else {
		/*===========================================
		*            输出日志文件流定义
		* ==========================================*/
		g_FileTemp.open("Log/Temp.txt");
		//GTT模块
		g_FileVeUELocationUpdateLogInfo.open("Log/GTTLog/VeUELocationUpdateLogInfo.txt");
		g_FileVeUENumPerRSULogInfo.open("Log/GTTLog/VeUENumPerRSULogInfo.txt");
		g_FileLocationInfo.open("Log/GTTLog/LocationInfo.txt");
		g_FileVeUEMessage.open("Log/GTTLog/VeUEMessage.txt");
		//RRM模块
		g_FileScheduleInfo.open("Log/RRMLog/ScheduleInfo.txt");
		g_FileClasterPerformInfo.open("Log/RRMLog/ClasterPerformInfo.txt");
		g_FileEventListInfo.open("Log/RRMLog/EventListInfo.txt");
		g_FileTTILogInfo.open("Log/RRMLog/TTILogInfo.txt");
		g_FileEventLogInfo.open("Log/RRMLog/EventLogInfo.txt");
		//TMC模块
		g_FileStatisticsDescription.open("Log/TMCLog/StatisticsDescription.txt");
		g_FileEmergencyDelayStatistics.open("Log/TMCLog/EmergencyDelayStatistics.txt");
		g_FilePeriodDelayStatistics.open("Log/TMCLog/PeriodDelayStatistics.txt");
		g_FileDataDelayStatistics.open("Log/TMCLog/DataDelayStatistics.txt");
		g_FileEmergencyPossion.open("Log/TMCLog/EmergencyPossion.txt");
		g_FileDataPossion.open("Log/TMCLog/DataPossion.txt");
		g_FileEmergencyConflictNum.open("Log/TMCLog/EmergencyConflictNum.txt");
		g_FilePeriodConflictNum.open("Log/TMCLog/PeriodConflictNum.txt");
		g_FileDataConflictNum.open("Log/TMCLog/DataConflictNum.txt");
		g_FileTTIThroughput.open("Log/TMCLog/TTIThroughput.txt");
		g_FileRSUThroughput.open("Log/TMCLog/RSUThroughput.txt");
		g_FilePackageLoss.open("Log/TMCLog/PackageLoss.txt");
		g_FilePackageTransimit.open("Log/TMCLog/PackageTransimit.txt");
	}

}
ofstream g_FileTemp;
//GTT_Urban模块
ofstream g_FileVeUELocationUpdateLogInfo;
ofstream g_FileVeUENumPerRSULogInfo;
ofstream g_FileLocationInfo;
ofstream g_FileVeUEMessage;
//RRM模块
ofstream g_FileScheduleInfo;
ofstream g_FileClasterPerformInfo;
ofstream g_FileEventListInfo;
ofstream g_FileTTILogInfo;
ofstream g_FileEventLogInfo;
//TMC模块
ofstream g_FileStatisticsDescription;
ofstream g_FileEmergencyDelayStatistics;
ofstream g_FilePeriodDelayStatistics;
ofstream g_FileDataDelayStatistics;
ofstream g_FileEmergencyPossion;
ofstream g_FileDataPossion;
ofstream g_FileEmergencyConflictNum;
ofstream g_FilePeriodConflictNum;
ofstream g_FileDataConflictNum;
ofstream g_FileTTIThroughput;
ofstream g_FileRSUThroughput;
ofstream g_FilePackageLoss;
ofstream g_FilePackageTransimit;



/*===========================================
*               全域函数定义
* ==========================================*/


