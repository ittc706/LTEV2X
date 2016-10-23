#pragma once
#include"GTAT.h"

class GTAT_HighSpeed :public GTAT_Basic {
public:
	static std::default_random_engine s_Engine;
	GTAT_HighSpeed() = delete;
	GTAT_HighSpeed(int &systemTTI, Configure& systemConfig, eNB* &systemeNBAry, Road* &systemRoadAry, RSU* &systemRSUAry, VeUE* &systemVeUEAry);

	std::vector<std::vector<int>> m_VeUENumPerRSU;//ͳ��ÿ��RSU�µĳ�����Ŀ������±����ڼ���λ�ø���(��0��ʼ)���ڲ��±����RSU���

	void configure()override;//��������
	void initialize()override;//��ʼ��

	void channelGeneration()override;//�ŵ�����
	void freshLoc() override;//�ŵ�ˢ��
	void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) override; //д�����λ�ø�����־
	void calculateInterference(const std::set<int>& transimitingVeUEId) override;//������ž���
};