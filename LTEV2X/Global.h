#pragma once
#include<fstream>
#include<vector>


/*===========================================
*            输出日志文件流声明
* ==========================================*/
extern std::ofstream g_FileTemp;
//GTAT_Urban模块
extern std::ofstream g_FileVeUELocationUpdateLogInfo;
extern std::ofstream g_FileVeUENumPerRSULogInfo;
//RRM_RR模块
extern std::ofstream g_FileRRScheduleInfo;
//RRM_DRA模块
extern std::ofstream g_FileDRAScheduleInfo;
extern std::ofstream g_FileClasterPerformInfo;
extern std::ofstream g_FileEventListInfo;
extern std::ofstream g_FileTTILogInfo;
extern std::ofstream g_FileEventLogInfo;
//TMAC_B模块
extern std::ofstream g_FileDelayStatistics;
extern std::ofstream g_FileEmergencyPossion;
extern std::ofstream g_FileDataPossion;
extern std::ofstream g_FileConflictNum;
extern std::ofstream g_FileTTIThroughput;
extern std::ofstream g_FileRSUThroughput;


/*===========================================
*               全域函数声明
* ==========================================*/
//产生特定分布随机数
void RandomUniform(double *t_pfArray, long t_ulNumber, double t_fUpBound, double t_fDownBound, bool t_bFlagZero);
void RandomGaussian(double *t_pfArray, long t_ulNumber, double t_fMean, double t_fStandardDeviation);
void SortBubble(double *t_pfArray, int t_wNumber, bool t_bFlagDirection, bool t_bFlagFabs);
void SelectMax(double *t_pfArray, int t_byNumber, int *t_pbyFirst, int *t_pbySecond);



/*===========================================
*             地理拓扑单元常量
* ==========================================*/
//数值常量设置
const double c_PI = 3.1415926535897932384626433832795f;
const double c_PINeg = -3.1415926535897932384626433832795f;
const double c_PI2 = 6.283185307179586476925286766559f;
const double c_PIHalf = 1.5707963267948966192313216916398f;
const double c_Degree2PI = 0.01745329251994329576923690768489f;
const double c_SqrtHalf = 0.70710678118654752440084436210485f;
const double c_SqrtThree = 1.73205080756887729f;
const double c_C = 299792458.0f;
const double c_FC = 2e9f;

namespace ns_GTAT_Urban {//城镇模块常量设置
	const int c_eNBNumber = 7;
	const int c_roadNumber = 14;
	const int c_RSUNumber = 24;//只有路口有RSU
	const int c_wide = 250;
	const int c_length = 433;
	const double c_lane_wide = 3.5;

	const double c_roadTopoRatio[c_roadNumber * 2] =
	{
		-1.5f, 1.0f,
		-0.5f, 1.0f,
		0.5f, 1.0f,
		1.5f, 1.0f,
		-2.5f, 0.0f,
		-1.5f, 0.0f,
		-0.5f, 0.0f,
		0.5f, 0.0f,
		1.5f, 0.0f,
		2.5f, 0.0f,
		-1.5f,-1.0f,
		-0.5f,-1.0f,
		0.5f,-1.0f,
		1.5f,-1.0f
	};
	const int c_WrapAroundRoad[c_roadNumber][9] =
	{
		{ 0,1,6,5,4,13,8,9,10 },
		{ 1,2,7,6,5,0,9,10,11 },
		{ 2,3,8,7,6,1,10,11,12 },
		{ 3,4,9,8,7,2,11,12,13 },
		{ 4,5,10,9,8,3,12,13,0 },
		{ 5,6,11,10,9,4,13,0,1 },
		{ 6,7,12,11,10,5,0,1,2 },
		{ 7,8,13,12,11,6,1,2,3 },
		{ 8,9,0,13,12,7,2,3,4 },
		{ 9,10,1,0,13,8,3,4,5 },
		{ 10,11,2,1,0,9,4,5,6 },
		{ 11,12,3,2,1,10,5,6,7 },
		{ 12,13,4,3,2,11,6,7,8 },
		{ 13,0,5,4,3,12,7,8,9 }

	};

	const int c_RSUClusterNum[c_RSUNumber] = {
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,

	};

	const double c_RSUTopoRatio[c_RSUNumber * 2] =
	{

		-2.0f, 1.5f,
		-1.0f, 1.5f,
		0.0f, 1.5f,
		1.0f, 1.5f,
		2.0f, 1.5f,
		-3.0f, 0.5f,
		-2.0f, 0.5f,
		-1.0f, 0.5f,
		0.0f, 0.5f,
		1.0f, 0.5f,
		2.0f, 0.5f,
		3.0f, 0.5f,
		-3.0f,-0.5f,
		-2.0f,-0.5f,
		-1.0f,-0.5f,
		0.0f,-0.5f,
		1.0f,-0.5f,
		2.0f,-0.5f,
		3.0f,-0.5f,
		-2.0f,-1.5f,
		-1.0f,-1.5f,
		0.0f,-1.5f,
		1.0f,-1.5f,
		2.0f,-1.5f,
	};

	const int c_RSUInRoad[c_roadNumber][4] =
	{
		{ 0,1,7,6},
		{ 1,2,8,7},
		{ 2,3,9,8},
		{ 3,4,10,9},
		{ 5,6,13,12},
		{ 6,7,14,13},
		{ 7,8,15,14},
		{ 8,9,16,15},
		{ 9,10,17,16},
		{ 10,11,18,17},
		{ 13,14,20,19},
		{ 14,15,21,20},
		{ 15,16,22,21},
		{ 16,17,23,22}
	};
}


namespace ns_GTAT_HighSpeed {
	const int c_eNBNumber = 2;
	const int c_laneNumber = 6;
	const int c_RSUNumber = 35;
	//const int c_wide=24;
	const int c_length = 3464;
	const double c_lane_wide = 4.0f;
	const double Fresh_time = 0.1f;
	const double c_ISD = 1732.0f;
	const int c_v = 140;

	const double c_laneTopoRatio[c_laneNumber * 2] =
	{
		0.0f, -2.5f,
		0.0f, -1.5f,
		0.0f, -0.5f,
		0.0f, 0.5f,
		0.0f, 1.5f,
		0.0f, 2.5f,
	};

	const int c_RSUClusterNum = 2;//每个RSU都只有2个簇

	const double c_RSUTopoRatio[c_RSUNumber * 2] =
	{
		17.0f, 0.0f,
		16.0f, 0.0f,
		15.0f, 0.0f,
		14.0f, 0.0f,
		13.0f, 0.0f,
		12.0f, 0.0f,
		11.0f, 0.0f,
		10.0f, 0.0f,
		9.0f, 0.0f,
		8.0f, 0.0f,
		7.0f, 0.0f,
		6.0f, 0.0f,
		5.0f, 0.0f,
		4.0f, 0.0f,
		3.0f, 0.0f,
		2.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		-1.0f, 0.0f,
		-2.0f, 0.0f,
		-3.0f, 0.0f,
		-4.0f, 0.0f,
		-5.0f, 0.0f,
		-6.0f, 0.0f,
		-7.0f, 0.0f,
		-8.0f, 0.0f,
		-9.0f, 0.0f,
		-10.0f, 0.0f,
		-11.0f, 0.0f,
		-12.0f, 0.0f,
		-13.0f, 0.0f,
		-14.0f, 0.0f,
		-15.0f, 0.0f,
		-16.0f, 0.0f,
		-17.0f, 0.0f,
	};
	const double c_eNBTopo[c_eNBNumber * 2] =
	{
		-0.5f*c_ISD,35,
		0.5f*c_ISD,35,
	};
}



/*===========================================
*          无线资源管理单元常量
* ==========================================*/
const int gc_TotalBandwidth =10 * 1000 * 1000;//10MHz
const int gc_BandwidthOfRB = 12 * 1000 * 15;//180kHZ
const int gc_RBNum = gc_TotalBandwidth/ gc_BandwidthOfRB;   //RB数量(111)
const int gc_BitNumPerRB=180;  //单位(个),由于RB带宽为180kHz，TTI为1ms，因此单位TTI单位RB传输的比特数为180k*1ms=180

/*===========================================
*          RRM_DRA模块常量定义
* ==========================================*/
const int gc_DRA_NTTI = 100; //所有簇进行一次DRA所占用的TTI数量。(NTTI:Number of TTI)


const int gc_DRAPatternTypeNum = 3;//事件的Pattern的类型种类
const int gc_DRA_RBNumPerPatternType[gc_DRAPatternTypeNum] = { 2,5,10 };//每个Pattern种类所占的RB数量
const int gc_DRAPatternNumPerPatternType[gc_DRAPatternTypeNum] = { 0,3,0 };//在全频段每个Pattern种类对应的Pattern数量

const int gc_DRAPatternTypePatternIdxInterval[gc_DRAPatternTypeNum][2] = {
	{0,gc_DRAPatternNumPerPatternType[0] - 1},
	{gc_DRAPatternNumPerPatternType[0],gc_DRAPatternNumPerPatternType[0] + gc_DRAPatternNumPerPatternType[1] - 1 },
	{ gc_DRAPatternNumPerPatternType[0] + gc_DRAPatternNumPerPatternType[1],gc_DRAPatternNumPerPatternType[0] + gc_DRAPatternNumPerPatternType[1] + gc_DRAPatternNumPerPatternType[2] - 1 },
};

const int gc_DRATotalPatternNum = [&]() {
	int res = 0;
	for (int num : gc_DRAPatternNumPerPatternType)
		res += num;
	return res;
}();//所有Pattern数量总和(包括Emergency)


/*===========================================
*          RRM_RR模块常量定义
* ==========================================*/
const int gc_RRNumRBPerPattern = 11;//每个Pattern的RB数量
const int gc_RRPatternNum = gc_TotalBandwidth / gc_BandwidthOfRB / gc_RRNumRBPerPattern;//总的Pattern数量



/*===========================================
*          数据业务与统计单元常量
* ==========================================*/
const int gc_PeriodMessagePackageNum = 4;
const int gc_PeriodMessageBitNumPerPackage[gc_PeriodMessagePackageNum] = { 1500,1500,1500,2000 };

const int gc_EmergencyMessagePackageNum = 4;
const int gc_EmergencyMessageBitNumPerPackage[gc_EmergencyMessagePackageNum] = { 1500,1500,1500,2000 };

const int gc_DataMessagePackageNum = 4;
const int gc_DataMessageBitNumPerPackage[gc_DataMessagePackageNum] = { 1500,1500,1500,2000 };




