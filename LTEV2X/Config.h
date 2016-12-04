#pragma once
#include"Enumeration.h"

//系统基本的仿真参数
struct SystemConfig {
	/*
	* 仿真平台，Linux或Windows，仅涉及到文件路径的格式
	*/
	Platform platform;

	/*
	* GTT模块实现的具体类别
	*/
	GTTMode _GTTMode;

	/*
	* RRM模块实现的具体类别
	*/
	RRMMode _RRMMode;

	/*
	* WT模块实现的具体类别
	*/
	WTMode _WTMode;

	/*
	* 仿真总共的TTI
	*/
	int NTTI;

	/*
	* 线程数量
	*/
	int threadNum;

	/*
	* 车辆刷新位置的周期
	*/
	int locationUpdateNTTI;

	/*
	* TTI日志开关
	*/
	bool TTILogIsOn = false;

	/*
	* Event日志开关
	*/
	bool eventLogIsOn = false;

	/*
	* Schedule日志开关
	*/
	bool scheduleLogIsOn = false;
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
