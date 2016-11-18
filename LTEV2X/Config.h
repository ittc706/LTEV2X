#pragma once
#include"Enumeration.h"

//系统基本的仿真参数
struct SystemConfig {
	/*
	* 仿真总共的TTI
	*/
	int NTTI;

	/*
	* 车辆刷新位置的周期
	*/
	int locationUpdateNTTI;

	/*
	* 周期性事件的周期（单位TTI）
	*/
	int periodicEventNTTI;

	/*
	* 数据业务事件泊松过程Lamda
	*/
	double dataLambda;

	/*
	* 紧急事件泊松过程Lamda,单位次/TTI
	*/
	double emergencyLambda;

	/*
	* 车辆总数
	*/
	int VeUENum;

	/*
	* RSU总数
	*/
	int RSUNum;

	/*
	* 基站总数
	*/
	int eNBNum;
};

//eNB配置参数
struct eNBConfig {
	SystemConfig* systemConfig;
	int roadId;
	int eNBId;
	double X;
	double Y;
	double AbsX;
	double AbsY;
};

//Road配置参数
struct UrbanRoadConfig {
	SystemConfig* systemConfig;
	int roadId;
	void *eNB;
	int eNBNum;
	int eNBOffset;
	void *lane;
	int laneNum;
	int laneOffset;
};


struct HighSpeedRodeConfig {
	SystemConfig* systemConfig;
	int roadId;
};


//VeUE配置参数
struct VeUEConfig {
	int roadId;
	int laneId;
	int locationId;
	double X;
	double Y;
	double AbsX;
	double AbsY;
	double V;
};

//RSU配置参数
struct RSUConfig {
	int RSUId;
};

//地理位置配置参数
struct Location {
	bool manhattan;
	LocationType locationType;
	double distance; //单位:m
	double distance1; //单位:m
	double distance2; //单位:m
	double eNBAntH; //单位:m
	double VeUEAntH; //单位:m
	double RSUAntH;//单位：m
	double posCor[5];

};

//天线配置参数
struct Antenna {
	double TxAngle; // degree
	double RxAngle; // degree
	double maxAttenu; // dB
	int byTxAntNum;
	int byRxAntNum;
	double *TxSlantAngle; // degree
	double *RxSlantAngle; // degree
	double *TxAntSpacing;
	double *RxAntSpacing;
	double antGain;
};
