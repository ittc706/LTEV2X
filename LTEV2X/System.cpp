#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"
#include"RSU.h"


using namespace std;

void System::process() {
	//��������
	configure();

	//�����ʼ��
	initialization();

	//�����¼�����
	m_TMCPoint->buildEventList(g_FileEventListInfo);

	//��ʼ����
	for (int count = 0;count < m_Config.NTTI;count++) {
		cout << "Current TTI = " << m_TTI << endl;
		//����λ�ø���
		if (count % m_Config.locationUpdateNTTI == 0) {
			m_GTTPoint->channelGeneration();
			m_GTTPoint->cleanWhenLocationUpdate();
			m_RRMPoint->cleanWhenLocationUpdate();
		}
		//��ʼ��Դ����
		m_RRMPoint->schedule();
		m_TTI++;
	}

	cout.setf(ios::fixed);
	cout << "�����ŵ������ʱ��" << m_RRMPoint->m_GTTTimeConsume / 1000.0L << " s\n" << endl;
	cout << "SINR�����ʱ��" << m_RRMPoint->m_WTTimeConsume / 1000.0L << " s\n" << endl;
	cout.unsetf(ios::fixed);

	//�������ҵ��ʱ������
	m_TMCPoint->processStatistics(g_FileDelayStatistics, g_FileEmergencyPossion, g_FileDataPossion, g_FileConflictNum, g_FileEventLogInfo);

	//��ӡ��������λ�ø�����־��Ϣ
	m_GTTPoint->writeVeUELocationUpdateLogInfo(g_FileVeUELocationUpdateLogInfo, g_FileVeUENumPerRSULogInfo);
}

void System::configure() {//ϵͳ�����������
	ConfigLoader configLoader("Config\\systemConfig.html");
	configLoader.load();//���������ļ�

	stringstream ss;

	const string nullString("");
	string temp;
	if ((temp = configLoader.getParam("NTTI")) != nullString) {
		ss << temp;
		ss >> m_Config.NTTI;
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("periodicEventNTTI")) != nullString) {
		ss << temp;
		ss >> m_Config.periodicEventNTTI;
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");


	if ((temp = configLoader.getParam("emergencyLambda")) != nullString) {
		ss << temp;
		ss >> m_Config.emergencyLambda;
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("dataLambda")) != nullString) {
		ss << temp;
		ss >> m_Config.dataLambda;
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("locationUpdateNTTI")) != nullString) {
		ss << temp;
		ss >> m_Config.locationUpdateNTTI;
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("GTTMode")) != nullString) {
		if (temp == "URBAN") {
			m_GTTMode = URBAN;
			cout << "GTT��Ԫ��URBANģʽ" << endl;
		}
		else if (temp == "HIGHSPEED") {
			m_GTTMode = HIGHSPEED;
			cout << "GTT��Ԫ��HIGHSPEEDģʽ" << endl;
		}
		else
			throw Exp("�������˵�Ԫ�������ô���");
	}
	else
		throw Exp("ConfigLoaderError");

	if ((temp = configLoader.getParam("RRMMode")) != nullString) {
		if (temp == "TDM_DRA") {
			m_RRMMode = TDM_DRA;
			cout << "RRM��Ԫ��TDM_DRAģʽ" << endl;
		}
		else if (temp == "ICC_DRA") {
			m_RRMMode = ICC_DRA;
			cout << "RRM��Ԫ��ICC_DRAģʽ" << endl;
		}
		else if (temp == "RR") {
			m_RRMMode = RR;
			cout << "RRM��Ԫ��RRģʽ" << endl;
		}
		else
			throw Exp("������Դ����Ԫ�������ô���");
	}
	else
		throw Exp("ConfigLoaderError");
	/*cout << m_Config.NTTI << endl;
	cout << m_Config.periodicEventNTTI << endl;
	cout << m_Config.emergencyLambda << endl;
	cout << m_Config.dataLambda << endl;
	cout << m_Config.locationUpdateNTTI << endl;
	system("pause");*/
}


void System::initialization() {
	m_TTI = 0;	

	//GTTģ���ʼ��
	initializeGTTModule();

	//WTģ���ʼ��
	initializeWTModule();

	//RRMģ���ʼ��
	initializeRRMModule();

	//TMCģ���ʼ��
	initializeTMCModule();
}


void System::initializeGTTModule() {
	switch (m_GTTMode) {
	case URBAN:
		m_GTTPoint = new GTT_Urban(m_TTI, m_Config, m_eNBAry, m_RoadAry, m_RSUAry, m_VeUEAry);
		break;
	case HIGHSPEED:
		m_GTTPoint = new GTT_HighSpeed(m_TTI, m_Config, m_eNBAry, m_RoadAry, m_RSUAry, m_VeUEAry);
		break;
	}
	//��ʼ���������˲���
	m_GTTPoint->configure();

	//��ʼ��eNB��Rode��RSU��VUE������
	m_GTTPoint->initialize();
}

void System::initializeWTModule() {
	m_WTPoint = new WT_B(m_Config, m_RSUAry, m_VeUEAry);
	m_WTPoint->initialize();//ģ���ʼ��
}


void System::initializeRRMModule() {
	switch (m_RRMMode) {
	case RR:
		m_RRMPoint = new RRM_RR(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, 1);
		break;
	case TDM_DRA:
		m_RRMPoint = new RRM_TDM_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, 4);
		break;
	case ICC_DRA:
		m_RRMPoint = new RRM_ICC_DRA(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput, m_GTTPoint, m_WTPoint, 4);
		break;
	default:
		break;
	}
	m_RRMPoint->initialize();//ģ���ʼ��
}


void System::initializeTMCModule() {
	m_TMCPoint = new TMC_B(m_TTI, m_Config, m_RSUAry, m_VeUEAry, m_EventVec, m_EventTTIList, m_TTIRSUThroughput);
	m_TMCPoint->initialize();//ģ���ʼ��
}


System::~System() {
	//����ģ��ָ��
	if (m_TMCPoint != nullptr) {
		delete m_TMCPoint;
		m_TMCPoint = nullptr;
	}
	if (m_RRMPoint != nullptr) {
		delete m_RRMPoint;
		m_RRMPoint = nullptr;
	}
	if (m_GTTPoint != nullptr) {
		delete m_GTTPoint;
		m_GTTPoint = nullptr;
	}
	if (m_WTPoint != nullptr) {
		delete m_WTPoint;
		m_WTPoint = nullptr;
	}

	//�����ʵ��������
	if (m_eNBAry != nullptr) {
		delete[] m_eNBAry;
		m_eNBAry = nullptr;
	}
	if (m_RSUAry != nullptr) {
		delete[] m_RSUAry;
		m_RSUAry = nullptr;
	}
	if (m_VeUEAry != nullptr) {
		delete[] m_VeUEAry;
		m_VeUEAry = nullptr;
	}
	if (m_RoadAry != nullptr) {
		delete[] m_RoadAry;
		m_RoadAry = nullptr;
	}
}

