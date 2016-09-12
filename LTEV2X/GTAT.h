#pragma once
// GTAT Geographical topology and transport
#include<random>
#include"RSU.h"
#include"VUE.h"
#include"eNB.h"
#include"Road.h"
#include"Config.h"

class GTAT_Basic {
public:
	GTAT_Basic() = delete;
	GTAT_Basic(int &systemTTI, sConfigure& systemConfig, ceNB* &systemeNBAry, cRoad* &systemRoadAry, cRSU* &systemRSUAry, cVeUE* &systemVeUEAry) :
		m_TTI(systemTTI), m_Config(systemConfig), m_eNBAry(systemeNBAry), m_RoadAry(systemRoadAry), m_RSUAry(systemRSUAry), m_VeUEAry(systemVeUEAry) {}


	int& m_TTI;//��ǰ��TTIʱ��
	sConfigure& m_Config;//ϵͳ��������
	ceNB* &m_eNBAry;//��վ����
	cRoad* &m_RoadAry;//��·����
	cRSU* &m_RSUAry;//RSU����
	cVeUE* &m_VeUEAry;//VeUE����


	virtual void configure() = 0;//��������
	virtual void initialize() = 0;//��ʼ��

	virtual void channelGeneration() = 0;//�ŵ�����
	virtual void freshLoc() = 0;//�ŵ�ˢ��
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &out) = 0; //д�����λ�ø�����־
};
