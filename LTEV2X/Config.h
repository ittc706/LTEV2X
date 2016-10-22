#pragma once
#include"Enumeration.h"

/*ϵͳ�����ķ������*/
struct sConfigure {
	int NTTI;//�����ܹ���TTI

	int periodicEventNTTI;//�������¼������ڣ���λTTI��
	double dataLambda;//����ҵ���¼����ɹ���Lamda
	double emergencyLambda;//�����¼����ɹ���Lamda,��λ��/TTI
	int locationUpdateNTTI;//����ˢ��λ�õ�����

	double ISD;
	double fc;
	int VeUENum;//��������
	int PeUENum;//��������

	int eNBNum;//��վ����
	int RoadNum;//Road����
	int LaneNum;//Lane����
	int RSUNum;//RSU����
	int* pupr;//user per road array
	unsigned short wxNum;//ueTopo x�� ����
	unsigned short wyNum;//ueTopo y�� ����
	int ueTopoNum;
	double *pueTopo;//һ��Road��ue�������

	double fv;//��������

};

/*eNB���ò���*/
struct seNBConfigure
{
	sConfigure* sys_config;
	unsigned short wRoadID;
	unsigned short weNBID;
	double fX;
	double fY;
	double fAbsX;
	double fAbsY;
};
/*Road���ò���*/
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

struct sLaneConfigure
{
	sConfigure* sys_config;
	unsigned short wLaneID;
};


/*UE���ò���*/
struct sUEConfigure
{
	unsigned short wRoadID;
	unsigned short wLaneID;
	int locationID;
	double fX;
	double fY;
	double fAbsX;
	double fAbsY;
	double fv;
};

/*RSU���ò���*/
struct sRSUConfigure
{
	unsigned short wRSUID;
};

/*����λ�����ò���*/
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

/*�������ò���*/
struct sAntenna
{
	double fTxAngle; // degree
	double fRxAngle; // degree
	double fMaxAttenu; // dB
	unsigned char byTxAntNum;
	unsigned char byRxAntNum;
	double *pfTxSlantAngle; // degree
	double *pfRxSlantAngle; // degree
	double *pfTxAntSpacing;
	double *pfRxAntSpacing;
	double fAntGain;
};
