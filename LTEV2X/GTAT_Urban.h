#pragma once
#include"GTAT.h"

struct sMobility {
	int tmp;
};
struct sGeography {
	int tmp;
};


class GTAT_Urban :public GTAT_Basic {
public:
	static default_random_engine s_Engine;
	GTAT_Urban() = delete;
	GTAT_Urban(int &systemTTI, sConfigure& systemConfig, ceNB* &systemeNBAry, cRoad* &systemRoadAry, cRSU* &systemRSUAry, cVeUE* &systemVeUEAry);

	void configure()override;//��������
	void initialize()override;//��ʼ��

	void channelGeneration()override;//�ŵ�����
	void freshLoc() override;//�ŵ�ˢ��
	void writeVeUELocationUpdateLogInfo(std::ofstream &out) override; //д�����λ�ø�����־
};