#pragma once
#include<fstream>


/*===========================================
*               变量定义
* ==========================================*/
extern std::ofstream g_OutDRAScheduleInfo;
extern std::ofstream g_OutClasterPerformInfo;
extern std::ofstream g_OutEventListInfo;
extern std::ofstream g_OutTTILogInfo;
extern std::ofstream g_OutEventLogInfo;
extern std::ofstream g_OutVeUELocationUpdateLogInfo;


/*===========================================
*               常量定义
* ==========================================*/


const int gc_RBNum = 100;   //子带数量
const int gc_ByteNumPerRB=100;  //每个RB可传输的比特数量
const int gc_DRA_NTTI=25; //所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)




/*===========================================
*               全域函数声明
* ==========================================*/
int max(int t1, int t2);




/*===========================================
*        简单的函数定义为内敛形式
* ==========================================*/

inline
int max(int t1, int t2) {
	return t1 < t2 ? t2 : t1;
}