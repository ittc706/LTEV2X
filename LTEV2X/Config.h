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
	void *eNB;
	int eNBNum;
	int eNBOffset;
	void *lane;
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
	bool manhattan;
	LocationType locationType;
	double distance; //��λ:m
	double distance1; //��λ:m
	double distance2; //��λ:m
	double eNBAntH; //��λ:m
	double VeUEAntH; //��λ:m
	double posCor[5];

};

/*�������ò���*/
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
