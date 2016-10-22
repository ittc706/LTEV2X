#pragma once
#include"GTAT.h"



class GTAT_Urban :public GTAT_Basic {
public:
	static std::default_random_engine s_Engine;
	GTAT_Urban() = delete;
	GTAT_Urban(int &systemTTI, sConfigure& systemConfig, eNB* &systemeNBAry, cRoad* &systemRoadAry, RSU* &systemRSUAry, cVeUE* &systemVeUEAry);

	std::vector<std::vector<int>> m_VeUENumPerRSU;//ͳ��ÿ��RSU�µĳ�����Ŀ������±����ڼ���λ�ø���(��0��ʼ)���ڲ��±����RSU���

	void configure()override;//��������
	void initialize()override;//��ʼ��

	void channelGeneration()override;//�ŵ�����
	void freshLoc() override;//�ŵ�ˢ��
	void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) override; //д�����λ�ø�����־
	void calculateInterference(std::vector<int> transimitingVeUEId) override;//������ž���
};