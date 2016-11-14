#pragma once
#include"GTT.h"

// <GTT_HighSpeed>: Geographical Topology and Transport HighSpeed

class GTT_HighSpeed :public GTT_Basic {
	/*------------------��̬------------------*/
public:
	/*
	* ��������棬���๲��
	*/
	static std::default_random_engine s_Engine;
	/*------------------��------------------*/
private:
	/*
	* ����Rode����
	*/
	int m_HighSpeedRodeNum;

	/*
	* user per road array
	*/
	int* m_pupr;

	/*
	* ��������
	*/
	double m_Speed;

	/*
	* ͳ��ÿ��RSU�µĳ�����Ŀ
	* ����±����ڼ���λ�ø���(��0��ʼ)���ڲ��±����RSU���
	*/
	std::vector<std::vector<int>> m_VeUENumPerRSU;

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	GTT_HighSpeed() = delete;

	/*
	* ���캯��
	*/
	GTT_HighSpeed(int &t_TTI, SystemConfig& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry, VeUE* &t_VeUEAry);

	/*
	* ��������
	*/
	void configure()override;

	/*
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	void cleanWhenLocationUpdate()override;

	/*
	* ��ʼ��
	*/
	void initialize()override;

	/*
	* �ŵ�����
	*/
	void channelGeneration()override;

	/*
	* �ŵ�ˢ��
	*/
	void freshLoc() override;

	/*
	* д�����λ�ø�����־
	*/
	void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) override;

	/*
	* ������ž���
	*/
	void calculateInterference(const std::vector<std::vector<std::list<int>>>& RRMInterferenceVec) override;
};