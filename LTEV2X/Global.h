#pragma once
#include<fstream>
#include<vector>
#include"Enumeration.h"



/*===========================================
*            �����־�ļ�������
* ==========================================*/
extern std::ofstream g_FileTemp;
//GTT_Urbanģ��
extern std::ofstream g_FileVeUELocationUpdateLogInfo;
extern std::ofstream g_FileVeUENumPerRSULogInfo;
extern std::ofstream g_FileLocationInfo;
extern std::ofstream g_FileVeUEMessage;
//RRMģ��
extern std::ofstream g_FileScheduleInfo;
extern std::ofstream g_FileClasterPerformInfo;
extern std::ofstream g_FileEventListInfo;
extern std::ofstream g_FileTTILogInfo;
extern std::ofstream g_FileEventLogInfo;
//TMC_Bģ��
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
*               ȫ��������
* ==========================================*/
//�����ض��ֲ������
void randomUniform(double *t_pfArray, long t_ulNumber, double t_fUpBound, double t_fDownBound, bool t_bFlagZero);
void randomGaussian(double *t_pfArray, long t_ulNumber, double t_fMean, double t_fStandardDeviation);
void sortBubble(double *t_pfArray, int t_wNumber, bool t_bFlagDirection, bool t_bFlagFabs);
void selectMax(double *t_pfArray, int t_byNumber, int *t_pbyFirst, int *t_pbySecond);



/*===========================================
*             �������˵�Ԫ����
* ==========================================*/
//��ֵ��������
const double gc_PI = 3.1415926535897932384626433832795f;
const double gc_PINeg = -3.1415926535897932384626433832795f;
const double gc_PI2 = 6.283185307179586476925286766559f;
const double gc_PIHalf = 1.5707963267948966192313216916398f;
const double gc_Degree2PI = 0.01745329251994329576923690768489f;
const double gc_SqrtHalf = 0.70710678118654752440084436210485f;
const double gc_SqrtThree = 1.73205080756887729f;
const double gc_C = 299792458.0f;
const double gc_FC = 6e9f;
	
namespace ns_GTT_HighSpeed {

	const int gc_eNBNumber = 2;
	const int gc_LaneNumber = 6;
	const int gc_RSUNumber = 35;
	const int gc_Length = 3464;
	const double gc_LaneWidth = 4.0f;
	const double gc_FreshTime = 0.1f;
	const double gc_ISD = 1732.0f;

	const double gc_LaneTopoRatio[gc_LaneNumber * 2] = {
		0.0f, -2.5f,
		0.0f, -1.5f,
		0.0f, -0.5f,
		0.0f, 0.5f,
		0.0f, 1.5f,
		0.0f, 2.5f,
	};

	const int gc_RSUClusterNum = 2;//ÿ��RSU��ֻ��2����

	const double gc_RSUTopoRatio[gc_RSUNumber * 2] = {
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
	const double gc_eNBTopo[gc_eNBNumber * 2] = {
		-0.5f*gc_ISD,35,
		0.5f*gc_ISD,35,
	};
}



/*===========================================
*          ������Դ����Ԫ����
* ==========================================*/
/*
* 10MHz���ܴ���(Hz)
*/
const int gc_TotalBandwidth =10 * 1000 * 1000;

/*
* ÿ��RB�Ĵ���(Hz)
*/
const int gc_BandwidthOfRB = 12 * 1000 * 15;

/*
* ��λ(��),����RB����Ϊ180kHz��TTIΪ1ms����˵�λTTI��λRB����ı�����Ϊ180k*1ms=180
*/
const int gc_BitNumPerRB=180;

/*
* ���Ʒ�ʽ
*/
const ModulationType gc_ModulationType = QPSK;

/*
* �ŵ���������
*/
const double gc_CodeRate = 0.5;

/*
* �������������С�ظɱ�
* �����ж��Ƿ񶪰�֮��
*/
const double gc_CriticalPoint = 1.99;//

/*===========================================
*          RRM_TDM_DRAģ�鳣������
* ==========================================*/
namespace ns_RRM_TDM_DRA {
	/*
	* ���дؽ���һ��DRA��ռ�õ�TTI������(NTTI:Number of TTI)
	*/
	const int gc_NTTI = 100;

	/*
	* �¼���Pattern����������
	* �������¼��������¼�������ҵ���¼�
	*/
	const int gc_PatternTypeNum = 3;

	/*
	* ÿ��Pattern������ռ��RB����
	*/
	const int gc_RBNumPerPatternType[gc_PatternTypeNum] = { 2,10,10 };

	/*
	* ÿ��Pattern�����Ӧ��Pattern����
	*/
	const int gc_PatternNumPerPatternType[gc_PatternTypeNum] = { 0,5,0 };

	/*
	* ÿ��������¼�������Ե�Pattern�Ŀ�ʼ�������ţ���[startIdx,endIdx]��������
	*/
	const int gc_PatternTypePatternIdxInterval[gc_PatternTypeNum][2] = {
		{ 0,gc_PatternNumPerPatternType[0] - 1 },
		{ gc_PatternNumPerPatternType[0],gc_PatternNumPerPatternType[0] + gc_PatternNumPerPatternType[1] - 1 },
		{ gc_PatternNumPerPatternType[0] + gc_PatternNumPerPatternType[1],gc_PatternNumPerPatternType[0] + gc_PatternNumPerPatternType[1] + gc_PatternNumPerPatternType[2] - 1 },
	};

	/*
	* ����Pattern���������������¼�
	*/
	const int gc_TotalPatternNum = [&]() {
		int res = 0;
		for (int num : gc_PatternNumPerPatternType)
			res += num;
		return res;
	}();
}


/*===========================================
*          RRM_RRģ�鳣������
* ==========================================*/
namespace ns_RRM_RR {
	/*
	* ÿ��Pattern��RB����
	*/
	const int gc_RBNumPerPattern = 10;

	/*
	* �ܵ�Pattern����
	*/
	const int gc_TotalPatternNum = gc_TotalBandwidth / gc_BandwidthOfRB / gc_RBNumPerPattern;
}


/*===========================================
*          RRM_ICC_DRAģ�鳣������
* ==========================================*/
namespace ns_RRM_ICC_DRA {
	/*
	* ÿ��Pattern��RB����
	*/
	const int gc_RBNumPerPattern = 10;

	/*
	* �ܵ�Pattern����
	*/
	const int gc_TotalPatternNum = gc_TotalBandwidth / gc_BandwidthOfRB / gc_RBNumPerPattern;
}



/*===========================================
*          ����ҵ����ͳ�Ƶ�Ԫ����
* ==========================================*/
/*
* �����¼�/�����¼�/����ҵ���¼� �����ݰ�����
* �±���MessageType�Ķ���Ϊ׼
*/
const std::vector<int> gc_MessagePackageNum = { 4,4,4 };

/*
* �����¼�/�����¼�/����ҵ���¼� ÿ�����ݰ���bit����
* �±���MessageType�Ķ���Ϊ׼
*/
const std::vector<std::vector<int>> gc_MessageBitNumPerPackage{
	{ 1520,1520,1520,2400 },
	{ 1520,1520,1520,2400 },
	{ 1520,1520,1520,2400 },
};

/*
* �����¼�/�����¼�/����ҵ���¼� ��ʼ���˱ܴ���С
* �±���MessageType�Ķ���Ϊ׼
*/
const std::vector<int> gc_InitialWindowSize = { 5,5,5 };

/*
* �����¼�/�����¼�/����ҵ���¼� �����˱ܴ���С
* �±���MessageType�Ķ���Ϊ׼
*/
const std::vector<int> gc_MaxWindowSize = { 20,20,20 };





