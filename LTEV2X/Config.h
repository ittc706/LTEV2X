#pragma once
#include"Enumeration.h"

/*ϵͳ�����ķ������*/
struct sConfigure {
	int NTTI;//�����ܹ���TTI

	int periodicEventNTTI;//�������¼������ڣ���λTTI��
	double dataLambda;//����ҵ���¼����ɹ���Lamda
	double emergencyLambda;//�����¼����ɹ���Lamda,��λ��/TTI
	int locationUpdateNTTI;//����ˢ��λ�õ�����

	float ISD;
	float fc;
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
	float *pueTopo;//һ��Road��ue�������

	float fv;//��������

};

/*eNB���ò���*/
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
	float fX;
	float fY;
	float fAbsX;
	float fAbsY;
	float fv;
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
	float fDistance; // m
	float fDistance1; // m
	float fDistance2; // m
	float feNBAntH; // m
	float fUEAntH; // m
	float afPosCor[5];

};

/*�������ò���*/
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
