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
	/*------------------��------------------*/
public:
	int& m_TTI;//��ǰ��TTIʱ��
	SystemConfig& m_Config;//ϵͳ��������
	eNB* &m_eNBAry;//��վ����������Ϊʲô�������������ͣ���Ϊϵͳ����Щ����ָ����뿿��ģ������ʼ������˲��ܴ��뿽��
	Road* &m_RoadAry;//��·����
	RSU* &m_RSUAry;//RSU����
	VeUE* &m_VeUEAry;//VeUE����

	/*------------------�ӿ�------------------*/
public:
	GTT_Basic() = delete;
	GTT_Basic(int &t_TTI, SystemConfig& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry, VeUE* &t_VeUEAry) :
		m_TTI(t_TTI), m_Config(t_Config), m_eNBAry(t_eNBAry), m_RoadAry(t_RoadAry), m_RSUAry(t_RSUAry), m_VeUEAry(t_VeUEAry) {}

	virtual void configure() = 0;//��������
	virtual void cleanWhenLocationUpdate() = 0;//������λ�ø���ʱ���������ĵ��������Ϣ
	virtual void initialize() = 0;//��ʼ��

	virtual void channelGeneration() = 0;//�ŵ�����
	virtual void freshLoc() = 0;//�ŵ�ˢ��
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &ou2) = 0; //д�����λ�ø�����־
	virtual void calculateInterference(const std::vector<std::vector<std::list<int>>>& RRMInterferenceVec) = 0;//������ž���
};
