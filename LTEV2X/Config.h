#pragma once
#include"Enumeration.h"

/*系统基本的仿真参数*/
struct Configure {
	int NTTI;//仿真总共的TTI

	int periodicEventNTTI;//周期性事件的周期（单位TTI）
	double dataLambda;//数据业务事件泊松过程Lamda
	double emergencyLambda;//紧急事件泊松过程Lamda,单位次/TTI
	int locationUpdateNTTI;//车辆刷新位置的周期

	double ISD;
	double fc;
	int VeUENum;//车辆总数
	int PeUENum;//行人总数

	int eNBNum;//基站总数
	int UrbanRoadNum;//城镇Road总数
	int HighSpeedRodeNum;//高速Rode总数
	int RSUNum;//RSU总数
	int* pupr;//user per road array
	int wxNum;//ueTopo x轴 格数
	int wyNum;//ueTopo y轴 格数
	int ueTopoNum;
	double *pueTopo;//一个Road中ue相对坐标

	double fv;//车辆车速
};

/*eNB配置参数*/
struct eNBConfigure {
	Configure* systemConfig;
	int roadId;
	int eNBId;
	double X;
	double Y;
	double AbsX;
	double AbsY;
};

/*Road配置参数*/
struct UrbanRoadConfigure {
	Configure* systemConfig;
	int roadId;
	void *peNB;
	int eNBNum;
	int eNBOffset;
	void *pLane;
	int laneNum;
	int laneOffset;
};


struct HighSpeedRodeConfigure {
	Configure* systemConfig;
	int roadId;
};


/*VeUE配置参数*/
struct VeUEConfigure {
	int roadId;
	int laneId;
	int locationId;
	double X;
	double Y;
	double AbsX;
	double AbsY;
	double V;
};

/*RSU配置参数*/
struct RSUConfigure {
	int RSUId;
};

/*地理位置配置参数*/
struct Location {
	bool bManhattan;
	LocationType eType;
	double distance; // m
	double distance1; // m
	double distance2; // m
	double eNBAntH; // m
	double VeUEAntH; // m
	double afPosCor[5];

};

/*天线配置参数*/
struct Antenna {
	double fTxAngle; // degree
	double fRxAngle; // degree
	double fMaxAttenu; // dB
	int byTxAntNum;
	int byRxAntNum;
	double *pfTxSlantAngle; // degree
	double *pfRxSlantAngle; // degree
	double *pfTxAntSpacing;
	double *pfRxAntSpacing;
	double fAntGain;
};
