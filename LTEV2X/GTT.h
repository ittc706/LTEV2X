#pragma once
#include<random>
#include<vector>
#include<list>
#include<tuple>
#include<fstream>
#include"GTT_VeUE.h"
#include"GTT_RSU.h"
#include"GTT_eNB.h"
#include"GTT_Road.h"

#include"Enumeration.h"

// <GTT>: Geographical Topology and Transport

class System;

class GTT {
	/*------------------��̬------------------*/
public:
	/*
	* ��������
	*/
	static int s_VeUE_NUM;

	/*
	* RSU����
	*/
	static int s_RSU_NUM;

	/*
	* ��վ����
	*/
	static int s_eNB_NUM;

	/*
	* ��·����
	*/
	static int s_ROAD_NUM;

	/*
	* ӵ���ȼ�����
	*/
	static int s_CONGESTION_LEVEL_NUM;

	/*
	* ӵ���ȼ���Ӧ�ĳ�����Ŀ
	*/
	static std::vector<int> s_VEUE_NUM_PER_CONGESTION_LEVEL;

	/*
	* ����GTTģ��
	*/
	static void loadConfig();

	/*
	* ����������Ŀ������ӵ���ȼ�
	*/
	static int calcuateCongestionLevel(int t_VeUENum);
	/*------------------��------------------*/
private:
	/*
	* ָ��ϵͳ��ָ��
	*/
	System* m_Context;
public:
	/*
	* GTT��Ԫ��ͼ�µĻ�վ����
	*/
	GTT_eNB** m_eNBAry;

	/*
	* GTT��Ԫ��ͼ�µĵ�·����
	*/
	GTT_Road** m_RoadAry;

	/*
	* GTT��Ԫ��ͼ�µ�RSU����
	*/
	GTT_RSU** m_RSUAry;

	/*
	* VeUE����
	* ��һά�ȵ�ָ��ָ�����飬��������ָ��GTT_VeUEʵ���ָ��
	* Ϊʲô��������ָ�룬��Ϊ��Ҫʵ�ֶ�̬
	*/
	GTT_VeUE** m_VeUEAry;

	/*
	* ��־�ļ�
	*/
	std::ofstream m_FileStatisticsDescription;
	std::ofstream m_FileVeUELocationUpdateLogInfo;
	std::ofstream m_FileVeUENumPerRSULogInfo;
	std::ofstream m_FileLocationInfo;
	std::ofstream m_FileVeUEMessage;
	std::ofstream m_FileVeUECongestionInfo;

	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	GTT() = delete;

	/*
	* ���캯��
	*/
	GTT(System* t_Context);

	/*
	* ��������
	*/
	~GTT();

	/*
	* ��ȡϵͳ���ָ��
	*/
	System* getContext() { return m_Context; }

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
	virtual void writeVeUELocationUpdateLogInfo() = 0;

	/*
	* ������ž���
	* ����Ĳ�������
	*		����±�Ϊ�������
	*		�ڲ��±�ΪPattern���
	*		���ڲ�listΪ�ó����ڸ�Pattern�µĸ����б�
	* Ŀǰ���дؼ���ţ���ΪRSU�����̫С���������Ժ���
	*/
	virtual void calculateInterference(const std::vector<std::vector<std::list<int>>>& t_RRMInterferenceVec) = 0;
};
