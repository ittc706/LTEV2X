#pragma once
#include<fstream>
#include<vector>
#include"Utility.h"

/*===========================================
*            输出日志文件流声明
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
const int gc_DRATotalBandwidth = 20 * 1000 * 1000;//20MHz
const int gc_BandwidthOfRB = 12 * 1000 * 15;//180kHZ
const int gc_RBNum = gc_DRATotalBandwidth/ gc_BandwidthOfRB;   //RB数量(111)
const int gc_BitNumPerRB=1800;  //单位(个),由于RB带宽为180kHz，TTI为10ms，因此单位TTI单位RB传输的比特数为180k*10ms=1800
const int gc_DRA_NTTI=25; //所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)
const int gc_DRAEmergencyTotalPatternNum = 6;//传输紧急事件的Pattern数量
const int gc_DRAEmergencyFBNumPerPattern = 1;//每个紧急事件的Pattern占用的FB数量
const int gc_DRAPatternTypeNum = 2;//非紧急事件的Pattern的类型种类
const int gc_DRAPatternNumPerPatternType[gc_DRAPatternTypeNum]= { 60,9 };//在全频段每个Pattern种类对应的Pattern数量
const int gc_DRA_FBNumPerPatternType[gc_DRAPatternTypeNum]={ 1,5 };//每个Pattern种类所占的FB数量
const std::vector<int> gc_DRAPatternIdxTable[gc_DRAPatternTypeNum]={ //每个Pattern种类对应的Pattern Idx的列表
	makeContinuousSequence(0,59),
	makeContinuousSequence(60,68)
};

inline
int getDRATotalPatternNum() {
	int res = 0;
	for (int num : gc_DRAPatternNumPerPatternType)
		res += num;
	return res;
}
const int gc_DRATotalPatternNum = getDRATotalPatternNum();//所有非EmergencyPattern类型的Pattern数量总和




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

