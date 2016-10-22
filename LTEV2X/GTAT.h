#pragma once
// GTAT Geographical topology and transport
#include<random>
#include"RSU.h"
#include"VUE.h"
#include"eNB.h"
#include"Road.h"
#include"Config.h"


struct Mobility {
	int tmp;
};
struct Geography {
	int tmp;
};


class GTAT_Basic {
public:
	GTAT_Basic() = delete;
	GTAT_Basic(int &systemTTI, sConfigure& systemConfig, eNB* &systemeNBAry, cRoad* &systemRoadAry, RSU* &systemRSUAry, cVeUE* &systemVeUEAry) :
		m_TTI(systemTTI), m_Config(systemConfig), m_eNBAry(systemeNBAry), m_RoadAry(systemRoadAry), m_RSUAry(systemRSUAry), m_VeUEAry(systemVeUEAry) {}
	
	int& m_TTI;//��ǰ��TTIʱ��
	sConfigure& m_Config;//ϵͳ��������
	eNB* &m_eNBAry;//��վ����������Ϊʲô�������������ͣ���Ϊϵͳ����Щ����ָ����뿿��ģ������ʼ������˲��ܴ��뿽��
	cRoad* &m_RoadAry;//��·����
	RSU* &m_RSUAry;//RSU����
	cVeUE* &m_VeUEAry;//VeUE����


	virtual void configure() = 0;//��������
	virtual void initialize() = 0;//��ʼ��

	virtual void channelGeneration() = 0;//�ŵ�����
	virtual void freshLoc() = 0;//�ŵ�ˢ��
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &ou2) = 0; //д�����λ�ø�����־
	virtual void calculateInterference(std::vector<int> transimitingVeUEId) = 0;//������ž���
};
