#pragma once
#include<fstream>
#include<list>


/*===========================================
*            输出日志文件流定义
* ==========================================*/

extern std::ofstream g_OutDRAScheduleInfo;
extern std::ofstream g_OutClasterPerformInfo;
extern std::ofstream g_OutEventListInfo;
extern std::ofstream g_OutTTILogInfo;
extern std::ofstream g_OutEventLogInfo;
extern std::ofstream g_OutVeUELocationUpdateLogInfo;


/*===========================================
*               RSU级别的常量
* 不设置为RSU的静态变量，统一写在这里方便查找
* ==========================================*/
const int gc_RBNum = 100;   //RB数量
const int gc_ByteNumPerRB=100;  //每个RB可传输的比特数量
const int gc_DRA_NTTI=25; //所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)
const int gc_DRAEmergencyTotalPatternNum = 3;//传输紧急事件的Pattern数量
const int gc_DRAEmergencyFBNumPerPattern = 1;//每个紧急事件的Pattern占用的FB数量
const int gc_DRAPatternTypeNum = 2;//非紧急事件的Pattern的类型种类
const int gc_DRAPatternNumPerPatternType[gc_DRAPatternTypeNum]= { 20,5 };//在全频段每个Pattern种类对应的Pattern数量
const int gc_DRA_FBNumPerPatternType[gc_DRAPatternTypeNum]={ 1,5 };//每个Pattern种类所占的FB数量
const std::list<int> gc_DRAPatternIdxTable[gc_DRAPatternTypeNum]={ //每个Pattern种类对应的Pattern Idx的列表
	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19},
	{20,21,22,23,24}
};
const int gc_DRATotalPatternNum = 25;//所有非EmergencyPattern类型的Pattern数量总和




/*===========================================
*               全域函数声明
* ==========================================*/



/*===========================================
*                内敛函数定义
* ==========================================*/

inline
int max(int t1, int t2) {
	return t1 < t2 ? t2 : t1;
}

