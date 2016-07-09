#pragma once

#include"Definition.h"
#include"Numerical.h"
#include"Enumeration.h"


/*===========================================
*               全局常量
* ==========================================*/
extern int g_TTI;

/*===========================================
*               常量定义
* ==========================================*/
const int gc_DRA_FBNum = 10;
const int gc_RBNum = 100;   //子带数量
const double gc_PI = 3.1415926535897932384626433832795;
const int gc_ByteNumPerRB=50;  //每个RB可传输的比特数量
const int gc_DRA_NTTI=25; //所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)







/*===========================================
*               常量定义
* ==========================================*/

int max(int t1, int t2);
