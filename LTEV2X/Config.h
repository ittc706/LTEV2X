#pragma once
#include"Enumeration.h"

/*系统基本的仿真参数*/
struct sConfigure {

	int periodicEventNTTI;//周期性事件的周期（单位TTI）
	double emergencyLamda;//紧急事件的期望值（在给定的仿真周期内的期望值）
	int locationUpdateNTTI;//车辆刷新位置的周期

	float ISD;
	float fc;
	int VeUENum;//车辆总数
	int PeUENum;//行人总数

	int eNBNum;//基站总数
	int RoadNum;//Road总数
	int RSUNum;//RSU总数
	int* pupr;//user per road array
	unsigned short wxNum;//ueTopo x轴 格数
	unsigned short wyNum;//ueTopo y轴 格数
	int ueTopoNum;
	float *pueTopo;//一个Road中ue相对坐标

	float fv;//车辆车速

};

/*eNB配置参数*/
struct seNBConfigure
{
	sConfigure* sys_config;
	unsigned short wRoadID;
	unsigned short weNBID;
	float fX;
	float fY;
	float fAbsX;
	float fAbsY;
};
/*Road配置参数*/
struct sRoadConfigure
{
	sConfigure* sys_config;
	unsigned short wRoadID;
	void *peNB;
	unsigned short weNBNum;
	unsigned short weNBOffset;
	void *pLane;
	unsigned short wLaneNum;
	unsigned short wLaneOffset;
};

/*UE配置参数*/
struct sUEConfigure
{
	unsigned short wRoadID;
	int locationID;
	float fX;
	float fY;
	float fAbsX;
	float fAbsY;
	float fv;
};

/*RSU配置参数*/
struct sRSUConfigure
{
	unsigned short wRSUID;
};

/*地理位置配置参数*/
struct sLocation
{
	bool bManhattan;
	eLocationType eType;
	float fDistance; // m
	float fDistance1; // m
	float fDistance2; // m
	float feNBAntH; // m
	float fUEAntH; // m
	float afPosCor[5];

};

/*天线配置参数*/
struct sAntenna
{
	float fTxAngle; // degree
	float fRxAngle; // degree
	float fMaxAttenu; // dB
	unsigned char byTxAntNum;
	unsigned char byRxAntNum;
	float *pfTxSlantAngle; // degree
	float *pfRxSlantAngle; // degree
	float *pfTxAntSpacing;
	float *pfRxAntSpacing;
	float fAntGain;
};
