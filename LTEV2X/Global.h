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
void RandomUniform(double *t_pfArray, unsigned long t_ulNumber, double t_fUpBound, double t_fDownBound, bool t_bFlagZero);
void RandomGaussian(double *t_pfArray, unsigned long t_ulNumber, double t_fMean, double t_fStandardDeviation);
void SortBubble(double *t_pfArray, unsigned short t_wNumber, bool t_bFlagDirection, bool t_bFlagFabs);
void SelectMax(double *t_pfArray, unsigned char t_byNumber, unsigned char *t_pbyFirst, unsigned char *t_pbySecond);



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
	const unsigned short c_eNBNumber = 7;
	const unsigned short c_roadNumber = 14;
	const unsigned short c_RSUNumber = 78;
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
	const unsigned short c_WrapAroundRoad[c_roadNumber][9] =
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

	const unsigned short c_RSUClusterNum[c_RSUNumber] = {
		8,4,8,4,8,4,8,4,8,
		4,4,4,4,4,
		4,4,4,4,4,
		8,4,8,4,8,4,8,4,8,4,8,4,8,
		4,4,4,4,4,4,4,
		4,4,4,4,4,4,4,
		8,4,8,4,8,4,8,4,8,4,8,4,8,
		4,4,4,4,4,
		4,4,4,4,4,
		8,4,8,4,8,4,8,4,8
	};

	const double c_RSUTopoRatio[c_RSUNumber * 2] =
	{
		-2.0f, 1.5f,
		-1.5f, 1.5f,
		-1.0f, 1.5f,
		-0.5f, 1.5f,
		0.0f, 1.5f,
		0.5f, 1.5f,
		1.0f, 1.5f,
		1.5f, 1.5f,
		2.0f, 1.5f,
		-2.0f, 1.2f,
		-1.0f, 1.2f,
		0.0f, 1.2f,
		1.0f, 1.2f,
		2.0f, 1.2f,
		-2.0f, 0.8f,
		-1.0f, 0.8f,
		0.0f, 0.8f,
		1.0f, 0.8f,
		2.0f, 0.8f,
		-3.0f, 0.5f,
		-2.5f, 0.5f,
		-2.0f, 0.5f,
		-1.5f, 0.5f,
		-1.0f, 0.5f,
		-0.5f, 0.5f,
		0.0f, 0.5f,
		0.5f, 0.5f,
		1.0f, 0.5f,
		1.5f, 0.5f,
		2.0f, 0.5f,
		2.5f, 0.5f,
		3.0f, 0.5f,
		-3.0f, 0.2f,
		-2.0f, 0.2f,
		-1.0f, 0.2f,
		0.0f, 0.2f,
		1.0f, 0.2f,
		2.0f, 0.2f,
		3.0f, 0.2f,
		-3.0f,-0.2f,
		-2.0f,-0.2f,
		-1.0f,-0.2f,
		0.0f,-0.2f,
		1.0f,-0.2f,
		2.0f,-0.2f,
		3.0f,-0.2f,
		-3.0f,-0.5f,
		-2.5f,-0.5f,
		-2.0f,-0.5f,
		-1.5f,-0.5f,
		-1.0f,-0.5f,
		-0.5f,-0.5f,
		0.0f,-0.5f,
		0.5f,-0.5f,
		1.0f,-0.5f,
		1.5f,-0.5f,
		2.0f,-0.5f,
		2.5f,-0.5f,
		3.0f,-0.5f,
		-2.0f,-0.8f,
		-1.0f,-0.8f,
		0.0f,-0.8f,
		1.0f,-0.8f,
		2.0f,-0.8f,
		-2.0f,-1.2f,
		-1.0f,-1.2f,
		0.0f,-1.2f,
		1.0f,-1.2f,
		2.0f,-1.2f,
		-2.0f,-1.5f,
		-1.5f,-1.5f,
		-1.0f,-1.5f,
		-0.5f,-1.5f,
		0.0f,-1.5f,
		0.5f,-1.5f,
		1.0f,-1.5f,
		1.5f,-1.5f,
		2.0f,-1.5f,
	};

	const unsigned short c_RSUInRoad[c_roadNumber][10] =
	{
		{ 0,1,2,10,15,23,22,21,14,9 },
		{ 2,3,4,11,16,25,24,23,15,10 },
		{ 4,5,6,12,17,27,26,25,16,11 },
		{ 6,7,8,13,18,29,28,27,17,12 },
		{ 19,20,21,33,40,48,47,46,39,32 },
		{ 21,22,23,34,41,50,49,48,40,33 },
		{ 23,24,25,35,42,52,51,50,41,34 },
		{ 25,26,27,36,43,54,53,52,42,35 },
		{ 27,28,29,37,44,56,55,54,43,36 },
		{ 29,30,31,38,45,58,57,56,44,37 },
		{ 48,49,50,60,65,71,70,69,64,59 },
		{ 50,51,52,61,66,73,72,71,65,60 },
		{ 52,53,54,62,67,75,74,73,66,61 },
		{ 54,55,56,63,68,77,76,75,67,62 }
	};
}


namespace ns_GTAT_High {
	const unsigned short c_eNBNumber = 2;
	const unsigned short c_laneNumber = 6;
	const unsigned short c_RSUNumber = 35;
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

	const unsigned short c_RSUClusterNum = 2;//每个RSU都只有2个簇

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
*          无线资源管理单元常量
* ==========================================*/
const int gc_PeriodMessageBitNum = 2400;
const int gc_EmergencyMessageBitNum = 2400;
const int gc_DataMessageBitNum = 18000;



