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
	/*
	* ��ǰ��TTIʱ��,ָ��ϵͳ�ĸò���
	*/
	int& m_TTI;

	/*
	* ϵͳ���ò���,ָ��ϵͳ�ĸò���
	*/
	SystemConfig& m_Config;

	/*
	* ��վ����,ָ��ϵͳ�ĸò���
	* ����Ϊʲô�������������ͣ���Ϊϵͳ����Щ����ָ����뿿��ģ������ʼ������˲��ܴ��뿽��
	*/
	eNB* &m_eNBAry;

	/*
	* ��·����,ָ��ϵͳ�ĸò���
	*/
	Road* &m_RoadAry;

	/*
	* RSU����,ָ��ϵͳ�ĸò���
	*/
	RSU* &m_RSUAry;

	/*
	* VeUE����,ָ��ϵͳ�ĸò���
	*/
	VeUE* &m_VeUEAry;

	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	GTT_Basic() = delete;

	/*
	* ���캯��
	* ����ָ�붼���������ͣ���Ϊ��Ҫ��ʼ��ϵͳ�ĸ���ʵ������
	* �ù��캯��Ҳ�����˸�ģ�����ͼ
	*/
	GTT_Basic(int &t_TTI, SystemConfig& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry, VeUE* &t_VeUEAry) :
		m_TTI(t_TTI), m_Config(t_Config), m_eNBAry(t_eNBAry), m_RoadAry(t_RoadAry), m_RSUAry(t_RSUAry), m_VeUEAry(t_VeUEAry) {}

	/*
	* ģ���������
	*/
	virtual void configure() = 0;

	/*
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* ��ʼ������ʵ������
	*/
	virtual void initialize() = 0;

	/*
	* �ŵ�����
	*/
	virtual void channelGeneration() = 0;

	/*
	* �ŵ�ˢ��
	*/
	virtual void freshLoc() = 0;

	/*
	* д�����λ�ø�����־
	*/
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &ou2) = 0;

	/*
	* ������ž���
	* ����Ĳ�������
	*		����±�Ϊ�������
	*		�ڲ��±�ΪPattern���
	*		���ڲ�listΪ�ó����ڸ�Pattern�µĸ����б�
	* Ŀǰ���дؼ���ţ���ΪRSU�����̫С���������Ժ���
	*/
	virtual void calculateInterference(const std::vector<std::vector<std::list<int>>>& RRMInterferenceVec) = 0;
};
