#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"
#include"RSU.h"


using namespace std;

void cSystem::process() {
	//��������
	configure();

	//�����ʼ��
	initialization();

	//�����¼�����
	m_TMACPoint->buildEventList(g_FileEventListInfo);

	//��ʼ����
	for (int count = 0;count < m_Config.NTTI;count++) {
		cout << "Current TTI = " << m_TTI << endl;
		//����λ�ø���
		if (count % m_Config.locationUpdateNTTI == 0)
			m_GTATPoint->channelGeneration();
		//��ʼ��Դ����
		m_RRMPoint->schedule();
		m_TTI++;
	}

	//�������ҵ��ʱ������
	m_TMACPoint->processStatistics(g_FileDelayStatistics, g_FileEmergencyPossion, g_FileDataPossion, g_FileConflictNum, g_FileEventLogInfo);

	//��ӡ��������λ�ø�����־��Ϣ
	m_GTATPoint->writeVeUELocationUpdateLogInfo(g_FileVeUELocationUpdateLogInfo, g_FileVeUENumPerRSULogInfo);
}

void cSystem::configure() {//ϵͳ�����������

	m_Config.NTTI = 20;//����TTIʱ��
	m_Config.periodicEventNTTI = 500;
	m_Config.emergencyLambda = 0.0001;// 0.001;
	m_Config.dataLambda = 0.0001;
	m_Config.locationUpdateNTTI = 1000;

	//���������봫��ģʽ
	m_GTATMode = URBAN;

	//������Դ����ģʽ
	m_RRMMode = DRA;
}


void cSystem::initialization() {
	m_TTI = 0;	

	//GTATģ���ʼ��
	GTATModuleInitialize();

	//��ʼ���������˲���
	m_GTATPoint->configure();

	//��ʼ��eNB��Rode��RSU��VUE������
	m_GTATPoint->initialize();

	//WTģ���ʼ��
	WTModuleInitialize();

	//RRMģ���ʼ��
	RRMModuleInitialize();

	//TMACģ���ʼ��
	TMACmODULEInitialize();
}


void cSystem::GTATModuleInitialize() {
	switch (m_GTATMode) {
	case URBAN:
		m_GTATPoint = new GTAT_Urban(m_TTI, m_Config, m_eNBAry, m_RoadAry, m_RSUAry, m_VeUEAry);
		break;
	}
}

void cSystem::WTModuleInitialize() {
	m_WTPoint = new WT_B(m_VeUEAry);
}


void cSystem::RRMModuleInitialize() {
	switch (m_RRMMode) {
	case RR:
		m_RRMPoint = new RRM_RR(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput);
		break;
	case DRA:
		m_RRMPoint = new RRM_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, P123, m_WTPoint, m_GTATPoint);
		break;
	default:
		break;
	}
}


void cSystem::TMACmODULEInitialize() {
	m_TMACPoint = new TMAC_B(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput);
}


void cSystem::dispose() {
	delete m_TMACPoint;
	delete m_RRMPoint;
	delete m_GTATPoint;
	delete m_WTPoint;
}

