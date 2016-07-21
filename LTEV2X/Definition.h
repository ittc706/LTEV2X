#pragma once
#include "Constant.h"
#include <math.h>
#include <string>
#include <map>

using std::map;
using std::string;


enum eLocationType{Los,Nlos,None}; //位置类型 

////#define MULTI_THREAD
////#define __TEST__
//#define __TEST_CQI__
////#define __TEST_PRE_DETECTION_SINR__
////#define __TEST_CHANNEL__
//#define UPLINK_MCS_LEVELS 29
//#define DOWNLINK_MCS_LEVELS 28
////****************系统仿真类型设置****************//
//
//enum eDuplexing//双工方式
//{
//	TDD,
//	FDD
//};
//extern map<string, eDuplexing> eDuplexingMap;
//map<string, eDuplexing> eDuplexingMapInitial(void);
//enum eRxType
//{
//	MMSE_IRC,
//	MMSE_OPT1,
//	MRC,
//	PRE
//};
//extern map<string, eRxType> eRxTypeMap;
//map<string, eRxType> eRxTypeMapInitial(void);
//enum eLinkType//链路类型
//{
//	DOWNLINK,
//	UPLINK
//};
//extern map<string, eLinkType> eLinkTypeMap;
//map<string, eLinkType> eLinkTypeMapInitial(void);
//
//enum eTransMode//传输模式
//{
//	INVALID_TM,
//        TM_1,
//	TM_2,
//	CODEBOOK,
//	BEAMFORMING
//}; 
//extern map<string, eTransMode> eTransModeMap;
//map<string, eTransMode> eTransModeMapInitial(void);
//
//enum eCoordinateMode{
//	SCSU,//单小区单用户
//	SCMU,//单小区多用户
//	JT_MCSU,//多小区单用户
//	JT_MCMU,//多小区多用户
//	CS_MCSU,//多小区单用户
//	CS_MCMU//多小区多用户
//};
//
//extern map<string, eCoordinateMode> eCoordinateModeMap;
//map<string, eCoordinateMode> eCoordinateModeMapInitial(void);
//
//enum eUserGroup{
//	CCU,
//	CEU
//};
//
//enum eHarqType{
//        IR,
//        CC
//};
////***************地理拓扑类变量设置*****************//
//
//enum eCellNumber//小区数量
//{
//	CELL_NINETEEN = 19
//};
//
//enum eSectorNumber//扇区类型
//{
//	SECTOR_NONE = 0,
//	SECTOR_OMNI = 1,
//	SECTOR_THREE = 3,
//	SECTOR_SIX = 6
//};
//
//enum eBSType
//{
//	MACRO = 0,
//	PICO 
//};
//
//// enum eScenario//仿真场景
//// {
//// 	SCME_URBANMACRO = 0
//// };
//enum eNet
//{
//	HONET = 0,
//	HENET = 1
//};
//enum eTopology
//{
//        CASE_1 = 0,
//        CASE_2
//};
//enum eSubframeType
//{
//        NORMAL,
//        LP_ABS_SUBFRAME
//};
//extern map<string, eNet> eNetMap;
//map<string, eNet> eNetMapInitial(void);
//
////****************MAC层变量类型******************//
//
//enum eCACType//接纳控制类型
//{
//	CAC_CALL,
//	CAC_HANDOVER
//};
//
//enum eScheduleType//调度方法
//{
//	ROUNDROBIN,
//	PF
//};
//extern map<string, eScheduleType> eScheduleTypeMap;
//map<string, eScheduleType> eScheduleTypeMapInitial(void);
//
//enum eChannelType
//{
//	IMT,
//	SCME_MACRO,
//	SCME_MICRO
//};
//extern map<string, eChannelType> eChannelTypeMap;
//map<string, eChannelType> eChannelTypeMapInitial(void);
//
//enum eChannelEstimateType
//{
//	IDEAL_ESTIMATE,
//	NONIDEAL_ESTIMATE
//};
//extern map<string, eChannelEstimateType> eChannelEstimateTypeMap;
//map<string, eChannelEstimateType> eChannelEstimateTypeMapInitial(void);
//
//
//enum eScenario
//{
//	IMTA_INDOOR_HOTSPOT,
//	IMTA_URBAN_MICRO,
//	IMTA_SUBURBAN_MACRO,
//	IMTA_URBAN_MACRO,
//	IMTA_RURAL_MACRO,
//	SCME_SUBURBAN_MACRO,
//	SCME_URBAN_MACRO_LOW,
//	SCME_URBAN_MACRO_HIGH,
//	SCME_URBAN_MICRO,
//	SCM_SUBURBAN_MACRO,
//	SCM_URBAN_MACRO_LOW,
//	SCM_URBAN_MACRO_HIGH,
//	SCM_URBAN_MICRO,
//	SCM_URBAN_MICRO_NLOS,
//	SCM_URBAN_MICRO_LOS
//};
//extern map<string, eScenario> eScenarioMap;
//map<string, eScenario> eScenarioMapInitial(void);
//// enum eIMT
//// {
//// 	URBAN_MICRO,
//// 	URBAN_MACRO,
//// 	SUBURBAN_MACRO,
//// 	RURAL_MACRO
//// };
//// extern map<string, eIMT> eIMTMap;
//// map<string, eIMT> eIMTMapInitial(void);
//// enum eSCMEMacro
//// {
//// 	SUBURBAN,
//// 	URBAN_HIGH,
//// 	URBAN_LOW
//// };
//// extern map<string, eSCMEMacro> eSCMEMacroMap;
//// map<string, eSCMEMacro> eSCMEMacroMapInitial(void);
//// enum eSCMEMicro
//// {
//// 	URBAN_LOS,
//// 	URBAN_NLOS
//// };
//// extern map<string, eSCMEMicro> eSCMEMicroMap;
//// map<string, eSCMEMicro> eSCMEMicroMapInitial(void);
//
////****************物理层变量类型****************//
//
//enum eModType//调制方式
//{
//	BPSK = 1,
//	QPSK = 2,
//	_16QAM = 4,
//	_64QAM = 6
//};
//enum eFreqReuse
//{
//	STATIC = 0,
//	SOFT,
//	NONE
//};
//enum eFeedbackMode
//{
//	MODE_3_1,
//	INFINITE
//};
//enum eABSMode
//{
//        Z_ABS,
//        LP_ABS
//};
//enum eFeedbackCriterion
//{
//	MAX_CAP
//};
//
//enum eInterferenceDetect
//{
//	IDEAL,
//	REALISTIC
//};
//
//extern map<string, eFreqReuse> eFreqReuseMap;
//map<string, eFreqReuse> eFreqReuseMapInitial(void);
//
//typedef struct sMCS//调制编码方式等级
//{
//	eModType mod;
//	float rate;
//}sMCS;
//
//struct sComplex//复数类型
//{
//	float r;
//	float i;
//};
////BCI反馈结构
//typedef struct BCI{
//	double rate_SU;
//	double rate_MU;
//	int PMI_SU;
//	int RI_SU;
//	int PMI_BCI;
//	int RI_BCI;
//	int UE_id;
//}BCI;
//
//typedef struct sMacPacket{
//        int size;//mac 包的大小
//        double size_no_rate_loss;
//		double size_non_fullbuffer;//~~~
//        int ttime;//mac 包的产生时间
//        int rtime;//mac 包成功接收时间
//       
//}sMacPacket;
//typedef enum eAccess{
//	ACCESS_PICO,
//	ACCESS_MACRO,
//	ACCESS_ALL
//}eAccess;
//
//#define INVALID_USER -1
//#define INVALID_CQI -1
//#define INVALID_PMI -1
//#define INVALID_RI -1
//#define INVALID_CODEWORD -1
//#define INVALID_MCS -1
//#define INVALID_HARQ -1
//#define LOCK 0
//#define UNLOCK 1
//
//
//#define ABS_2_DB(x) (10.0f*log10((float)(x)))
//#define DB_2_ABS(x) (pow(10.0f,((float)(x))/10.0f))
//#define DBM_2_ABS(x) (DB_2_ABS(x)/1000.0f)
//#define SINR_2_CAP(x) (log(1+((float)(x))))
//#define COMPACT_CSI(pmi,ri) ((((ri)&0xf)<<8)+((pmi)&0xff))
//#define COMPACT_CSI_2_PMI(csi) (csi&0xff)
//#define COMPACT_CSI_2_RI(csi) (csi>>8)
//#define CHOOSE_MCS(mcs_idx) ((mcs_idx)>=0?(mcs_idx):0)
//
//#define RB_SIZE 12
//#define MAX_SUBBAND_NUM 10
//#define MAX_H_NUM 100
//
//#define BLER_TARGET 0.1
//#define _NINF -1e20
//#define _INF 1e20
//#define EPS 1e-20
//#define DEBUG
//
//#define SUBFRAME_NUM 10
//#define MAX_RB 110
//
//#define EFF_RATE_PERS 10001
//#define EFF_QUA_PERS 41
//#define EFF_RATE_MIN 0
//#define EFF_RATE_MAX 1
//#define EFF_QUA_MIN 2
//#define EFF_QUA_MAX 6
//#define EFF_RATE_PERS_LVL 10000
//#define EFF_QUA_PERS_LVL 10
//
//#define OLLA_DEC_STEP 25
//#define OLLA_INC_STEP 10
//#define OLLA_STEP 100
//
//
//
//#ifdef WIN32
//#define ADD_
//#define HAVE_LAPACK_CONFIG_H
//#define LAPACK_COMPLEX_STRUCTURE
//#pragma comment(lib,"lib/libblas.lib")
//#pragma comment(lib,"lib/liblapack.lib")
//#pragma comment(lib,"lib/liblapacke.lib")
//#endif
//
