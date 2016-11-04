#pragma once
#include<random>
#include<set>
#include"RSU.h"
#include"VUE.h"
#include"eNB.h"
#include"Road.h"
#include"Config.h"

// <GTT>: Geographical Topology and Transport

class GTT_Basic {
public:
	GTT_Basic() = delete;
	GTT_Basic(int &systemTTI, Configure& systemConfig, eNB* &systemeNBAry, Road* &systemRoadAry, RSU* &systemRSUAry, VeUE* &systemVeUEAry) :
		m_TTI(systemTTI), m_Config(systemConfig), m_eNBAry(systemeNBAry), m_RoadAry(systemRoadAry), m_RSUAry(systemRSUAry), m_VeUEAry(systemVeUEAry) {}
	
	int& m_TTI;//��ǰ��TTIʱ��
	Configure& m_Config;//ϵͳ��������
	eNB* &m_eNBAry;//��վ����������Ϊʲô�������������ͣ���Ϊϵͳ����Щ����ָ����뿿��ģ������ʼ������˲��ܴ��뿽��
	Road* &m_RoadAry;//��·����
	RSU* &m_RSUAry;//RSU����
	VeUE* &m_VeUEAry;//VeUE����


	virtual void configure() = 0;//��������
	virtual void cleanWhenLocationUpdate() = 0;//������λ�ø���ʱ���������ĵ��������Ϣ
	virtual void initialize() = 0;//��ʼ��

	virtual void channelGeneration() = 0;//�ŵ�����
	virtual void freshLoc() = 0;//�ŵ�ˢ��
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &ou2) = 0; //д�����λ�ø�����־
	virtual void calculateInterference(const std::vector<std::list<int>>& RRMInterferenceVec) = 0;//������ž���
};
