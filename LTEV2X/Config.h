#pragma once
#include"Enumeration.h"

/*ϵͳ�����ķ������*/
struct Configure {
	int NTTI;//�����ܹ���TTI
	int locationUpdateNTTI;//����ˢ��λ�õ�����

	int periodicEventNTTI;//�������¼������ڣ���λTTI��
	double dataLambda;//����ҵ���¼����ɹ���Lamda
	double emergencyLambda;//�����¼����ɹ���Lamda,��λ��/TTI

	int VeUENum;//��������
	int RSUNum;//RSU����
	int eNBNum;//��վ����
};

/*eNB���ò���*/
struct eNBConfigure {
	Configure* systemConfig;
	int roadId;
	int eNBId;
	double X;
	double Y;
	double AbsX;
	double AbsY;
};

/*Road���ò���*/
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


/*VeUE���ò���*/
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

/*RSU���ò���*/
struct RSUConfigure {
	int RSUId;
};

/*����λ�����ò���*/
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

/*�������ò���*/
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
