#pragma once


#define UPLINK_MCS_LEVELS 29
#define DOWNLINK_MCS_LEVELS 28

#define INVALID_USER -1
//#define INVALID_CQI -1
//#define INVALID_PMI -1
//#define INVALID_RI -1
#define INVALID_CODEWORD -1
//#define INVALID_MCS -1
//#define INVALID_HARQ -1
//#define LOCK 0
//#define UNLOCK 1
//
//
#define ABS_2_DB(x) (10.0f*log10((float)(x)))
#define DB_2_ABS(x) (pow(10.0f,((float)(x))/10.0f))
//#define DBM_2_ABS(x) (DB_2_ABS(x)/1000.0f)
//#define SINR_2_CAP(x) (log(1+((float)(x))))
#define COMPACT_CSI(pmi,ri) ((((ri)&0xf)<<8)+((pmi)&0xff))
#define COMPACT_CSI_2_PMI(csi) (csi&0xff)
#define COMPACT_CSI_2_RI(csi) (csi>>8)
#define CHOOSE_MCS(mcs_idx) ((mcs_idx)>=0?(mcs_idx):0)

//
//#define RB_SIZE 12
#define MAX_SUBBAND_NUM 10
//#define MAX_H_NUM 100
//
#define BLER_TARGET 0.1
//#define _NINF -1e20
//#define _INF 1e20
#define EPS 1e-20
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
#define OLLA_STEP 100

#define MIN(x,y) (((x) < (y)) ? (x) : (y))  //lapacke.h包无法打开，就粘贴到这里