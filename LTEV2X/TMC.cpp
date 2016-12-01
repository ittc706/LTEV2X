/*
* =====================================================================================
*
*       Filename:  TMC.cpp
*
*    Description:  TMC模块
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<sstream>
#include<iostream>
#include<random>
#include"System.h"

#include"GTT.h"
#include"TMC.h"

#include"VUE.h"

#include"Function.h"
#include"ConfigLoader.h"

#define INVALID -1

using namespace std;

default_random_engine TMC_VeUE::s_Engine((unsigned)time(NULL));

TMC_VeUE::TMC_VeUE() {
	m_NextPeriodEventTriggerTTI = vector<int>(TMC::s_CONGESTION_LEVEL_NUM);

	uniform_int_distribution<int> u(0, TMC::s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL[0]);
	
	//给当前车辆一个初始化的触发事件，就以最小的拥塞等级来设定吧
	int initializeTrigger= u(s_Engine);
	
	//初始化触发时刻
	for (int congestionLevel = 0; congestionLevel < TMC::s_CONGESTION_LEVEL_NUM; congestionLevel++)
		m_NextPeriodEventTriggerTTI[congestionLevel] = initializeTrigger;
}

int TMC::s_CONGESTION_LEVEL_NUM = INVALID;

vector<int> TMC::s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL;

double TMC::s_EMERGENCY_POISSON = INVALID;

double TMC::s_DATA_POISSON = INVALID;

vector<int> TMC::s_MESSAGE_PACKAGE_NUM;

vector<std::vector<int>> TMC::s_MESSAGE_BIT_NUM_PER_PACKAGE;

vector<int> TMC::s_INITIAL_WINDOW_SIZE;

vector<int> TMC::s_MAX_WINDOW_SIZE;

void TMC::loadConfig(Platform t_Platform) {
	ConfigLoader configLoader;
	if (t_Platform == Windows) {
		configLoader.resolvConfigPath("Config\\TMCConfig.xml");
	}
	else if (t_Platform == Linux) {
		configLoader.resolvConfigPath("Config/TMCConfig.xml");
	}
	else {
		throw logic_error("Platform Config Error!");
	}

	stringstream ss;

	const string nullString("");
	string temp;

	if ((temp = configLoader.getParam("CongestionLevelNum")) != nullString) {
		ss << temp;
		ss >> s_CONGESTION_LEVEL_NUM;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("PeriodicEventPeriod")) != nullString) {
		s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL.clear();
		ss << temp;
		string temp2;
		while (ss >> temp2) {
			s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL.push_back(ConfigLoader::stringToInt(temp2));
		}
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("EmergencyPoisson")) != nullString) {
		ss << temp;
		ss >> s_EMERGENCY_POISSON;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("DataPoisson")) != nullString) {
		ss << temp;
		ss >> s_DATA_POISSON;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("MessagePackageNum")) != nullString) {
		s_MESSAGE_PACKAGE_NUM.clear();
		ss << temp;
		string temp2;
		while (ss >> temp2) {
			s_MESSAGE_PACKAGE_NUM.push_back(ConfigLoader::stringToInt(temp2));
		}
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("MessageBitNumPerPackage")) != nullString) {
		s_MESSAGE_BIT_NUM_PER_PACKAGE.assign(3,vector<int>(0));
		ss << temp;
		string temp2;
		int messageType = 0;
		int count = 0;
		while (ss >> temp2) {
			if (++count > s_MESSAGE_PACKAGE_NUM[messageType]) {
				count = 1;
				messageType++;
			}
			s_MESSAGE_BIT_NUM_PER_PACKAGE[messageType].push_back(ConfigLoader::stringToInt(temp2));
		}
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("InitialWindowSize")) != nullString) {
		s_INITIAL_WINDOW_SIZE.clear();
		ss << temp;
		string temp2;
		while (ss >> temp2) {
			s_INITIAL_WINDOW_SIZE.push_back(ConfigLoader::stringToInt(temp2));
		}
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("MaxWindowSize")) != nullString) {
		s_MAX_WINDOW_SIZE.clear();
		ss << temp;
		string temp2;
		while (ss >> temp2) {
			s_MAX_WINDOW_SIZE.push_back(ConfigLoader::stringToInt(temp2));
		}
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");


	/*cout << "CongestionLevelNum: " << s_CONGESTION_LEVEL_NUM << endl;
	cout << "PeriodicEventPeriod: " << endl;
	Print::printVectorDim1(s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL);
	cout << "EmergencyPoisson: " << s_EMERGENCY_POISSON << endl;
	cout << "DataPoisson: " << s_DATA_POISSON << endl;
	cout << "MessagePackageNum: " << endl;
	Print::printVectorDim1(s_MESSAGE_PACKAGE_NUM);
	cout << "MessageBitNumPerPackage: " << endl;
	Print::printVectorDim2(s_MESSAGE_BIT_NUM_PER_PACKAGE);
	cout << "InitialWindowSize: " << endl;
	Print::printVectorDim1(s_INITIAL_WINDOW_SIZE);
	cout << "MaxWindowSize: " << endl;
	Print::printVectorDim1(s_MAX_WINDOW_SIZE);
	cout << endl;*/
}

TMC::~TMC() {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++)
		Delete::safeDelete(m_VeUEAry[VeUEId]);
	Delete::safeDelete(m_VeUEAry, true);

	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++)
		Delete::safeDelete(m_RSUAry[RSUId]);
	Delete::safeDelete(m_RSUAry, true);

	m_FileEventLogInfo.close();
	m_FileEventListInfo.close();
	m_FileStatisticsDescription.close();
	m_FileEmergencyDelayStatistics.close();
	m_FilePeriodDelayStatistics.close();
	m_FileDataDelayStatistics.close();
	m_FileEmergencyPossion.close();
	m_FileDataPossion.close();
	m_FileEmergencyConflictNum.close();
	m_FilePeriodConflictNum.close();
	m_FileDataConflictNum.close();
	m_FileTTIThroughput.close();
	m_FileRSUThroughput.close();
	m_FilePackageLoss.close();
	m_FilePackageTransimit.close();
}


TMC::TMC(System* t_Context) :
	m_Context(t_Context) {

	m_EventTTIList = vector<list<int>>(getContext()->m_Config.NTTI);

	m_EergencyVeUEIdListOfTriggerTTI = vector<list<int>>(getContext()->m_Config.NTTI);

	m_DataVeUEIdListOfTriggerTTI = vector<list<int>>(getContext()->m_Config.NTTI);

	m_TTIRSUThroughput = vector<vector<int>>(getContext()->m_Config.NTTI, vector<int>(getContext()->m_Config.RSUNum));

	if (getContext()->m_Config.platform == Windows) {
		m_FileEventLogInfo.open("Log\\TMCLog\\EventLogInfo.txt");
		m_FileEventListInfo.open("Log\\TMCLog\\EventListInfo.txt");
		m_FileStatisticsDescription.open("Log\\TMCLog\\StatisticsDescription.txt");
		m_FileEmergencyDelayStatistics.open("Log\\TMCLog\\EmergencyDelayStatistics.txt");
		m_FilePeriodDelayStatistics.open("Log\\TMCLog\\PeriodDelayStatistics.txt");
		m_FileDataDelayStatistics.open("Log\\TMCLog\\DataDelayStatistics.txt");
		m_FileEmergencyPossion.open("Log\\TMCLog\\EmergencyPossion.txt");
		m_FileDataPossion.open("Log\\TMCLog\\DataPossion.txt");
		m_FileEmergencyConflictNum.open("Log\\TMCLog\\EmergencyConflictNum.txt");
		m_FilePeriodConflictNum.open("Log\\TMCLog\\PeriodConflictNum.txt");
		m_FileDataConflictNum.open("Log\\TMCLog\\DataConflictNum.txt");
		m_FileTTIThroughput.open("Log\\TMCLog\\TTIThroughput.txt");
		m_FileRSUThroughput.open("Log\\TMCLog\\RSUThroughput.txt");
		m_FilePackageLoss.open("Log\\TMCLog\\PackageLoss.txt");
		m_FilePackageTransimit.open("Log\\TMCLog\\PackageTransimit.txt");
	}
	else if (getContext()->m_Config.platform == Linux) {
		m_FileEventLogInfo.open("Log/TMCLog/EventLogInfo.txt");
		m_FileEventListInfo.open("Log/TMCLog/EventListInfo.txt");
		m_FileStatisticsDescription.open("Log/TMCLog/StatisticsDescription.txt");
		m_FileEmergencyDelayStatistics.open("Log/TMCLog/EmergencyDelayStatistics.txt");
		m_FilePeriodDelayStatistics.open("Log/TMCLog/PeriodDelayStatistics.txt");
		m_FileDataDelayStatistics.open("Log/TMCLog/DataDelayStatistics.txt");
		m_FileEmergencyPossion.open("Log/TMCLog/EmergencyPossion.txt");
		m_FileDataPossion.open("Log/TMCLog/DataPossion.txt");
		m_FileEmergencyConflictNum.open("Log/TMCLog/EmergencyConflictNum.txt");
		m_FilePeriodConflictNum.open("Log/TMCLog/PeriodConflictNum.txt");
		m_FileDataConflictNum.open("Log/TMCLog/DataConflictNum.txt");
		m_FileTTIThroughput.open("Log/TMCLog/TTIThroughput.txt");
		m_FileRSUThroughput.open("Log/TMCLog/RSUThroughput.txt");
		m_FilePackageLoss.open("Log/TMCLog/PackageLoss.txt");
		m_FilePackageTransimit.open("Log/TMCLog/PackageTransimit.txt");
	}
	else {
		throw logic_error("Platform Config Error!");
	}
}


void TMC::initialize() {
	//初始化VeUE的该模块参数部分
	m_VeUEAry = new TMC_VeUE*[getContext()->m_Config.VeUENum];
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId] = new TMC_VeUE();
	}

	//初始化RSU的该模块参数部分
	m_RSUAry = new TMC_RSU*[getContext()->m_Config.RSUNum];
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId] = new TMC_RSU();
	}
}

void TMC::eventTrigger() {
	int curTTI = getContext()->m_TTI;
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		TMC_VeUE* _VeUE = m_VeUEAry[VeUEId];
		int congestionLevel = _VeUE->getSystemPoint()->getGTTPoint()->m_CongestionLevel;

		if (curTTI != _VeUE->m_NextPeriodEventTriggerTTI[congestionLevel]) continue;
		Event evt = Event(VeUEId, curTTI, PERIOD);
		m_EventVec.push_back(evt);
		m_EventTTIList[curTTI].push_back(evt.getEventId());

		for (int congestionLevel = 0; congestionLevel < TMC::s_CONGESTION_LEVEL_NUM; congestionLevel++)
			_VeUE->m_NextPeriodEventTriggerTTI[congestionLevel] = curTTI + TMC::s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL[congestionLevel];
	}

	for (int VeUEId : m_EergencyVeUEIdListOfTriggerTTI[curTTI]) {
		Event evt = Event(VeUEId, curTTI, EMERGENCY);
		m_EventVec.push_back(evt);
		m_EventTTIList[curTTI].push_back(evt.getEventId());
	}

	for (int VeUEId : m_DataVeUEIdListOfTriggerTTI[curTTI]) {
		Event evt = Event(VeUEId, curTTI, DATA);
		m_EventVec.push_back(evt);
		m_EventTTIList[curTTI].push_back(evt.getEventId());
	}
}

void TMC::buildEmergencyDataEventTriggerTTI() {
	/*按时间顺序（事件的Id与时间相关，Id越小，事件发生的时间越小生成事件链表*/

	default_random_engine dre;//随机数引擎
	dre.seed((unsigned)time(NULL));//iomanip
	uniform_real_distribution<double> urd(0, 1);

	//生成紧急事件的发生时刻，每个时间槽存放该时刻发生紧急事件的车辆
	m_VeUEEmergencyNum = vector<int>(getContext()->m_Config.VeUENum, 0);//初始化统计量
	int countEmergency = 0;
	if (s_EMERGENCY_POISSON != 0) {
		for (int VeUEId = 0; VeUEId <getContext()->m_Config.VeUENum; VeUEId++) {
			//依次生成每个车辆的紧急事件到达时刻
			double T = 0;
			while (T <getContext()->m_Config.NTTI) {
				double u = urd(dre);
				if (u == 0) throw logic_error("uniform_real_distribution生成范围包含边界");
				T = T - (1 / s_EMERGENCY_POISSON)*log(u);
				int IntegerT = static_cast<int>(T);
				if (IntegerT < getContext()->m_Config.NTTI) {
					m_EergencyVeUEIdListOfTriggerTTI[IntegerT].push_back(VeUEId);
					++m_VeUEEmergencyNum[VeUEId];
					++countEmergency;
				}
			}
		}
	}
	cout << "countEmergency: " << countEmergency << endl;


	//生成数据业务事件的发生时刻，每个时间槽存放该时刻发生数据业务事件的车辆
	m_VeUEDataNum = vector<int>(getContext()->m_Config.VeUENum, 0);//初始化统计量
	int countData = 0;
	if (s_DATA_POISSON != 0) {
		for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
			//依次生成每个车辆的Data事件到达时刻
			double T = 0;
			while (T < getContext()->m_Config.NTTI) {
				double u = urd(dre);
				if (u == 0) throw logic_error("uniform_real_distribution生成范围包含边界");
				T = T - (1 / s_DATA_POISSON)*log(u);
				int IntegerT = static_cast<int>(T);
				if (IntegerT < getContext()->m_Config.NTTI) {
					m_DataVeUEIdListOfTriggerTTI[IntegerT].push_back(VeUEId);
					++m_VeUEDataNum[VeUEId];
					++countData;
				}
			}
		}
	}
	cout << "countData: " << countData << endl;
}

void TMC::processStatistics() {

	//打印事件链表
	writeEventListInfo();

	stringstream ssPeriod;
	stringstream ssEmergency;
	stringstream ssData;

	/*-----------------------ATTENTION-----------------------
	* endl操纵符很重要，将缓存中的数据刷新到流中
	* 否则当数据量不大时，可能写完数据，文件还是空的
	*-----------------------ATTENTION-----------------------*/

	/*------------------统计成功传输的事件数目------------------*/
	m_TransimitSucceedEventNumPerEventType = vector<int>(3);
	for (Event &event : m_EventVec) {
		if (event.isFinished()) {
			switch (event.getMessageType()) {
			case EMERGENCY:
				m_TransimitSucceedEventNumPerEventType[0]++;
				break;
			case PERIOD:
				m_TransimitSucceedEventNumPerEventType[1]++;
				break;
			case DATA:
				m_TransimitSucceedEventNumPerEventType[2]++;
				break;
			}
		}
	}


	m_FileStatisticsDescription << "<EmergencyEventNum>" << m_TransimitSucceedEventNumPerEventType[EMERGENCY] << "</EmergencyEventNum>" << endl;
	m_FileStatisticsDescription << "<PeriodEventNum>" << m_TransimitSucceedEventNumPerEventType[PERIOD] << "</PeriodEventNum>" << endl;
	m_FileStatisticsDescription << "<DataEventNum>" << m_TransimitSucceedEventNumPerEventType[DATA] << "</DataEventNum>" << endl;

	/*------------------统计等待时延------------------*/
	for (Event &event : m_EventVec)
		if (event.isFinished()) {
			switch (event.getMessageType()) {
			case EMERGENCY:
				ssEmergency << event.getQueueDelay() << " ";
				break;
			case PERIOD:
				ssPeriod << event.getQueueDelay() << " ";
				break;
			case DATA:
				ssData << event.getQueueDelay() << " ";
				break;
			default:
				throw logic_error("非法消息类型");
			}
		}
	m_FileEmergencyDelayStatistics << ssEmergency.str() << endl;
	m_FilePeriodDelayStatistics << ssPeriod.str() << endl;
	m_FileDataDelayStatistics << ssData.str() << endl;


	/*------------------统计传输时延------------------*/
	ssPeriod.str("");
	ssEmergency.str("");
	ssData.str("");
	for (Event &event : m_EventVec)
		if (event.isFinished()) {
			switch (event.getMessageType()) {
			case EMERGENCY:
				ssEmergency << event.getSendDelay() << " ";
				break;
			case PERIOD:
				ssPeriod << event.getSendDelay() << " ";
				break;
			case DATA:
				ssData << event.getSendDelay() << " ";
				break;
			default:
				throw logic_error("非法消息类型");
			}
		}
	m_FileEmergencyDelayStatistics << ssEmergency.str() << endl;
	m_FilePeriodDelayStatistics << ssPeriod.str() << endl;
	m_FileDataDelayStatistics << ssData.str() << endl;


	/*------------------统计紧急事件分布情况------------------*/
	for (int num : m_VeUEEmergencyNum)
		m_FileEmergencyPossion << num << " ";
	m_FileEmergencyPossion << endl;//这里很关键，将缓存区的数据刷新到流中

								   //统计数据业务事件分布情况
	for (int num : m_VeUEDataNum)
		m_FileDataPossion << num << " ";
	m_FileDataPossion << endl;//这里很关键，将缓存区的数据刷新到流中

							  //统计冲突情况
	ssPeriod.str("");
	ssEmergency.str("");
	ssData.str("");
	for (Event &event : m_EventVec) {
		switch (event.getMessageType()) {
		case EMERGENCY:
			ssEmergency << event.getConflictNum() << " ";
			break;
		case PERIOD:
			ssPeriod << event.getConflictNum() << " ";
			break;
		case DATA:
			ssData << event.getConflictNum() << " ";
			break;
		default:
			throw logic_error("非法消息类型");
		}
	}
	m_FileEmergencyConflictNum << ssEmergency.str() << endl;
	m_FilePeriodConflictNum << ssPeriod.str() << endl;
	m_FileDataConflictNum << ssData.str() << endl;
	writeEventLogInfo();

	/*------------------统计吞吐率------------------*/
	vector<int> tmpTTIThroughput(getContext()->m_Config.NTTI);
	vector<int> tmpRSUThroughput(getContext()->m_Config.RSUNum);
	for (int tmpTTI = 0; tmpTTI < getContext()->m_Config.NTTI; tmpTTI++) {
		for (int tmpRSUId = 0; tmpRSUId < getContext()->m_Config.RSUNum; tmpRSUId++) {
			tmpTTIThroughput[tmpTTI] += m_TTIRSUThroughput[tmpTTI][tmpRSUId];
			tmpRSUThroughput[tmpRSUId] += m_TTIRSUThroughput[tmpTTI][tmpRSUId];
		}
	}

	//以TTI为单位统计吞吐率
	for (int throughput : tmpTTIThroughput)
		m_FileTTIThroughput << throughput << " ";
	m_FileTTIThroughput << endl;

	//以RSU为单位统计吞吐率
	for (int throughput : tmpRSUThroughput)
		m_FileRSUThroughput << throughput << " ";
	m_FileRSUThroughput << endl;


	/*------------------统计丢包率------------------*/
	int transimitPackageNum = 0;
	int lossPacketNum = 0;
	for (Event &event : m_EventVec) {
		transimitPackageNum += event.getTransimitPackageNum();
		lossPacketNum += event.getPacketLossNum();
		for (double &d : event.getPackageLossDistanceVec())
			m_FilePackageLoss << d << endl;
		for (double &d : event.getPackageTransimitDistanceVec())
			m_FilePackageTransimit << d << endl;
	}
	m_FileStatisticsDescription << "<TransimitPackageNum>" << transimitPackageNum << "</TransimitPackageNum>" << endl;
	m_FileStatisticsDescription << "<PackageLossRate>" << (double)lossPacketNum / (double)transimitPackageNum << "</PackageLossRate>" << endl;
}

void TMC::writeEventListInfo() {
	for (int i = 0; i < getContext()->m_Config.NTTI; i++) {
		m_FileEventListInfo << "[ TTI = " << left << setw(3) << i << " ]" << endl;
		m_FileEventListInfo << "{" << endl;
		for (int eventId : m_EventTTIList[i]) {
			Event& e = m_EventVec[eventId];
			m_FileEventListInfo << "    " << e.toString() << endl;
		}
		m_FileEventListInfo << "}\n\n" << endl;
	}
}

void TMC::writeEventLogInfo() {
	for (int eventId = 0; eventId < static_cast<int>(m_EventVec.size()); eventId++) {
		string s;
		switch (m_EventVec[eventId].getMessageType()) {
		case EMERGENCY:
			s = "EMERGENCY";
			break;
		case PERIOD:
			s = "PERIOD";
			break;
		case DATA:
			s = "DATA";
			break;
		}
		m_FileEventLogInfo << "EventId = " << eventId << endl;
		m_FileEventLogInfo << "{" << endl;
		m_FileEventLogInfo << "    " << "VeUEId = " << m_EventVec[eventId].getVeUEId() << endl;
		m_FileEventLogInfo << "    " << "MessageType = " << s << endl;
		m_FileEventLogInfo << "    " << "SendDelay = " << m_EventVec[eventId].getSendDelay() << "(TTI)" << endl;
		m_FileEventLogInfo << "    " << "QueueDelay = " << m_EventVec[eventId].getQueueDelay() << "(TTI)" << endl;
		m_FileEventLogInfo << "    " << "ProcessDelay = " << m_EventVec[eventId].getProcessDelay() << "(TTI)" << endl;
		m_FileEventLogInfo << m_EventVec[eventId].toLogString(1);
		m_FileEventLogInfo << "}" << endl;
	}
}


