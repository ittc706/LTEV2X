#pragma once
#include"Enumeration.h"

/*系统基本的仿真参数*/
struct sConfigure {
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
	int RoadNum;//Road总数
	int LaneNum;//Lane总数
	int RSUNum;//RSU总数
	int* pupr;//user per road array
	int wxNum;//ueTopo x轴 格数
	int wyNum;//ueTopo y轴 格数
	int ueTopoNum;
	double *pueTopo;//一个Road中ue相对坐标

	double fv;//车辆车速

};

/*eNB配置参数*/
struct seNBConfigure
{
	sConfigure* sys_config;
	int wRoadID;
	int weNBID;
	double fX;
	double fY;
	double fAbsX;
	double fAbsY;
};
/*Road配置参数*/
struct sRoadConfigure
{
	sConfigure* sys_config;
	int wRoadID;
	void *peNB;
	int weNBNum;
	int weNBOffset;
	void *pLane;
	int wLaneNum;
	int wLaneOffset;
};

struct sLaneConfigure
{
	sConfigure* sys_config;
	int wLaneID;
};


/*UE配置参数*/
struct sUEConfigure
{
	int wRoadID;
	int wLaneID;
	int locationID;
	double fX;
	double fY;
	double fAbsX;
	double fAbsY;
	double fv;
};

/*RSU配置参数*/
struct sRSUConfigure
{
	int wRSUID;
};

/*地理位置配置参数*/
struct sLocation
{
	bool bManhattan;
	eLocationType eType;
	double fDistance; // m
	double fDistance1; // m
	double fDistance2; // m
	double feNBAntH; // m
	double fUEAntH; // m
	double afPosCor[5];

};

/*天线配置参数*/
struct sAntenna
{
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
