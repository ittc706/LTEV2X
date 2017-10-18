#pragma once
#include<vector>
#include<fstream>
#include"Enumeration.h"
#include"Event.h"

/*
* ǰ������
* ����System�ᱻ����ģ����������System.h���������������ģ��
* ��.cpp�����������
*/
class GTT;
class RRM;
class TMC;
class WT;

class VeUE;
class RSU;
class eNB;
class Road;

class System{
	/*
	* ������Ԫ��System�������ֹ�ⲿֱ�ӷ���
	*/
	friend class GTT;
	friend class GTT_Urban;
	friend class GTT_HighSpeed;
	friend class RRM;
	friend class RRM_TDM_DRA;
	friend class RRM_ICC_DRA;
	friend class RRM_RR;
	friend class TMC;
	friend class WT;

	/*------------------�ڲ���------------------*/
	struct SystemConfig {
		/*
		* GTTģ��ʵ�ֵľ������
		*/
		GTTMode _GTTMode;

		/*
		* RRMģ��ʵ�ֵľ������
		*/
		RRMMode _RRMMode;

		/*
		* WTģ��ʵ�ֵľ������
		*/
		WTMode _WTMode;

		/*
		* �����ܹ���TTI
		*/
		int NTTI;

		/*
		* �߳�����
		*/
		int threadNum;

		/*
		* ����ˢ��λ�õ�����
		*/
		int locationUpdateNTTI;

		/*
		* TTI��־����
		*/
		bool TTILogIsOn = false;

		/*
		* Event��־����
		*/
		bool eventLogIsOn = false;

		/*
		* Schedule��־����
		*/
		bool scheduleLogIsOn = false;
	};
	/*------------------��------------------*/
private:
	/*
	* ϵͳ��ǰ��TTIʱ��
	*/
	int m_TTI;

	/*
	* ϵͳ��������
	*/
	SystemConfig m_Config;

	/*
	* �ĸ�ʵ��������
	* �ֱ��ǻ�վ����·��RSU������
	*/
	eNB* m_eNBAry = nullptr;
	Road* m_RoadAry = nullptr;
	RSU* m_RSUAry = nullptr;
	VeUE* m_VeUEAry = nullptr;


	/*
	* ģ�������
	* GTTģ�飬RRMģ�飬WTģ�飬TMCģ��
	*/
	GTT* m_GTTPoint = nullptr;
	RRM* m_RRMPoint = nullptr;
	TMC* m_TMCPoint = nullptr;
	WT* m_WTPoint = nullptr;

	/*------------------����------------------*/
public:
	/*
	* ϵͳ���������ܿ�
	*/	
	void process();

	/*
	* �����������������ʵ�����������
	*/
	~System();
private:
	/*
	* ϵͳ�����������
	*/
	void configure();

	/*
	* ϵͳ�������ã����ϵͳ��ʼ��
	*/
	void initialization();

	/*
	* GTTģ������ʼ��
	* ��initialization()����
	*/
	void initializeGTTModule();

	/*
	* WTģ������ʼ��
	* ��initialization()����
	*/
	void initializeWTModule();

	/*
	* RRMģ������ʼ��
	* ��initialization()����
	*/
	void initializeRRMModule();

	/*
	* TMCģ������ʼ��
	* ��initialization()����
	*/
	void initializeTMCModule();

	/*
	* ������תʵ������ʼ��
	*/
	void initializeBuildConnection();
};





