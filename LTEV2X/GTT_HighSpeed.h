#pragma once
#include"GTT.h"

// <GTT_HighSpeed>: Geographical Topology and Transport HighSpeed

class GTT_HighSpeed :public GTT_Basic {
public:
	static std::default_random_engine s_Engine;
	GTT_HighSpeed() = delete;
	GTT_HighSpeed(int &t_TTI, Configure& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry, VeUE* &t_VeUEAry);

	std::vector<std::vector<int>> m_VeUENumPerRSU;//ͳ��ÿ��RSU�µĳ�����Ŀ������±����ڼ���λ�ø���(��0��ʼ)���ڲ��±����RSU���

	void configure()override;//��������
	void cleanWhenLocationUpdate()override;//������λ�ø���ʱ���������ĵ��������Ϣ
	void initialize()override;//��ʼ��

	void channelGeneration()override;//�ŵ�����
	void freshLoc() override;//�ŵ�ˢ��
	void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) override; //д�����λ�ø�����־
	void calculateInterference(const std::vector<std::list<int>>& RRMInterferenceVec) override;//������ž���
};