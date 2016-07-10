#pragma once


//这里若包含了VUE.h RSU.h eNB.h会引发大量的未声明的错误


/*===========================================
*               常量定义
* ==========================================*/
const int gc_DRA_FBNum = 10;
const int gc_RBNum = 100;   //子带数量
const int gc_ByteNumPerRB=100;  //每个RB可传输的比特数量
const int gc_DRA_NTTI=25; //所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)







/*===========================================
*               常量定义
* ==========================================*/

int max(int t1, int t2);
