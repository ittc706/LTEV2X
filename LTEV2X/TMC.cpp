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
#include"System.h"
#include"TMC.h"

#include"Function.h"
#include"ConfigLoader.h"
#include"Log.h"

#define INVALID -1

using namespace std;

default_random_engine TMC_VeUE::s_Engine((unsigned)time(NULL));

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
}


TMC::TMC(System* t_Context) :
	m_Context(t_Context) {

	//事件链表容器初始化
	m_EventTTIList = vector<list<int>>(getContext()->m_Config.NTTI);

	//吞吐率容器初始化
	m_TTIRSUThroughput = vector<vector<int>>(getContext()->m_Config.NTTI, vector<int>(getContext()->m_Config.RSUNum));
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


void TMC::buildEventList(ofstream& t_File) {
	/*按时间顺序（事件的Id与时间相关，Id越小，事件发生的时间越小生成事件链表*/

	default_random_engine dre;//随机数引擎
	dre.seed((unsigned)time(NULL));//iomanip
	uniform_real_distribution<double> urd(0, 1);


	//首先生成各个车辆的周期性事件的起始时刻(相对时刻，即[0 , m_Config.periodicEventNTTI)
	vector<list<int>> startTTIVec(s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL[0], list<int>());
	uniform_int_distribution<int> uid(0, s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL[0] - 1);
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		int startTTI = uid(dre);
		startTTIVec[startTTI].push_back(VeUEId);
	}

	//生成紧急事件的发生时刻，每个时间槽存放该时刻发生紧急事件的车辆
	m_VeUEEmergencyNum = vector<int>(getContext()->m_Config.VeUENum, 0);//初始化统计量
	int countEmergency = 0;
	vector<list<int>> emergencyEventTriggerTTI(getContext()->m_Config.NTTI);
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
					emergencyEventTriggerTTI[IntegerT].push_back(VeUEId);
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
	vector<list<int>> dataEventTriggerTTI(getContext()->m_Config.NTTI);
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
					dataEventTriggerTTI[IntegerT].push_back(VeUEId);
					++m_VeUEDataNum[VeUEId];
					++countData;
				}
			}
		}
	}
	cout << "countData: " << countData << endl;


	//根据startTTIVec依次填充PERIOD事件并在其中插入服从泊松分布的紧急事件

	int startTTIOfEachPeriod = 0;//每个周期的起始时刻
	while (startTTIOfEachPeriod < getContext()->m_Config.NTTI) {
		//TTIOffset为相对于startTTIOfEachPeriod的偏移量
		for (int TTIOffset = 0; TTIOffset <s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL[0]; TTIOffset++) {
			//压入紧急事件
			if (startTTIOfEachPeriod + TTIOffset < getContext()->m_Config.NTTI) {
				list<int> &emergencyList = emergencyEventTriggerTTI[startTTIOfEachPeriod + TTIOffset];
				for (int VeUEId : emergencyList) {
					/*-----------------------ATTENTION-----------------------
					* 这里先生成sEvent的对象，然后将其压入m_EventVec
					* 由于Vector<T>.push_back是压入传入对象的复制品，因此会调用sEvent的拷贝构造函数
					* sEvent默认的拷贝构造函数会赋值id成员（因此是安全的）
					*sEvent如果自定义拷贝构造函数，必须在构造函数的初始化部分拷贝id成员
					*-----------------------ATTENTION-----------------------*/
					Event evt = Event(VeUEId, startTTIOfEachPeriod + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.getEventId());
					--countEmergency;
				}
			}


			//压入数据业务事件
			if (startTTIOfEachPeriod + TTIOffset < getContext()->m_Config.NTTI) {
				list<int> &dataList = dataEventTriggerTTI[startTTIOfEachPeriod + TTIOffset];
				for (int VeUEId : dataList) {
					/*-----------------------ATTENTION-----------------------
					* 这里先生成sEvent的对象，然后将其压入m_EventVec
					* 由于Vector<T>.push_back是压入传入对象的复制品，因此会调用sEvent的拷贝构造函数
					* sEvent默认的拷贝构造函数会赋值id成员（因此是安全的）
					*sEvent如果自定义拷贝构造函数，必须在构造函数的初始化部分拷贝id成员
					*-----------------------ATTENTION-----------------------*/
					Event evt = Event(VeUEId, startTTIOfEachPeriod + TTIOffset, DATA);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.getEventId());
					--countData;
				}
			}


			//产生周期性事件
			if (startTTIOfEachPeriod + TTIOffset < getContext()->m_Config.NTTI) {
				list<int> &periodList = startTTIVec[TTIOffset];
				for (int VeUEId : periodList) {
					Event evt = Event(VeUEId, startTTIOfEachPeriod + TTIOffset, PERIOD);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.getEventId());
				}
			}
		}
		startTTIOfEachPeriod += s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL[0];
	}

	//打印事件链表
	writeEventListInfo(t_File);
}

void TMC::processStatistics(
	ofstream& t_FileStatisticsDescription,
	ofstream& t_FileEmergencyDelay, ofstream& t_FilePeriodDelay, ofstream& t_FileDataDelay,
	ofstream& t_FileEmergencyPossion, ofstream& t_FileDataPossion,
	ofstream& t_FileEmergencyConflict, ofstream& t_FilePeriodConflict, ofstream& t_FileDataConflict,
	ofstream& t_FilePackageLoss, ofstream& t_FilePackageTransimit,
	ofstream& t_FileEventLog) {

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


	t_FileStatisticsDescription << "<EmergencyEventNum>" << m_TransimitSucceedEventNumPerEventType[EMERGENCY] << "</EmergencyEventNum>" << endl;
	t_FileStatisticsDescription << "<PeriodEventNum>" << m_TransimitSucceedEventNumPerEventType[PERIOD] << "</PeriodEventNum>" << endl;
	t_FileStatisticsDescription << "<DataEventNum>" << m_TransimitSucceedEventNumPerEventType[DATA] << "</DataEventNum>" << endl;

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
	t_FileEmergencyDelay << ssEmergency.str() << endl;
	t_FilePeriodDelay << ssPeriod.str() << endl;
	t_FileDataDelay << ssData.str() << endl;


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
	t_FileEmergencyDelay << ssEmergency.str() << endl;
	t_FilePeriodDelay << ssPeriod.str() << endl;
	t_FileDataDelay << ssData.str() << endl;


	/*------------------统计紧急事件分布情况------------------*/
	for (int num : m_VeUEEmergencyNum)
		t_FileEmergencyPossion << num << " ";
	t_FileEmergencyPossion << endl;//这里很关键，将缓存区的数据刷新到流中

								   //统计数据业务事件分布情况
	for (int num : m_VeUEDataNum)
		t_FileDataPossion << num << " ";
	t_FileDataPossion << endl;//这里很关键，将缓存区的数据刷新到流中

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
	t_FileEmergencyConflict << ssEmergency.str() << endl;
	t_FilePeriodConflict << ssPeriod.str() << endl;
	t_FileDataConflict << ssData.str() << endl;
	writeEventLogInfo(t_FileEventLog);

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
		g_FileTTIThroughput << throughput << " ";
	g_FileTTIThroughput << endl;

	//以RSU为单位统计吞吐率
	for (int throughput : tmpRSUThroughput)
		g_FileRSUThroughput << throughput << " ";
	g_FileRSUThroughput << endl;


	/*------------------统计丢包率------------------*/
	int transimitPackageNum = 0;
	int lossPacketNum = 0;
	for (Event &event : m_EventVec) {
		transimitPackageNum += event.getTransimitPackageNum();
		lossPacketNum += event.getPacketLossNum();
		for (double &d : event.getPackageLossDistanceVec())
			t_FilePackageLoss << d << endl;
		for (double &d : event.getPackageTransimitDistanceVec())
			g_FilePackageTransimit << d << endl;
	}
	t_FileStatisticsDescription << "<TransimitPackageNum>" << transimitPackageNum << "</TransimitPackageNum>" << endl;
	t_FileStatisticsDescription << "<PackageLossRate>" << (double)lossPacketNum / (double)transimitPackageNum << "</PackageLossRate>" << endl;
}

void TMC::writeEventListInfo(ofstream &t_File) {
	for (int i = 0; i < getContext()->m_Config.NTTI; i++) {
		t_File << "[ TTI = " << left << setw(3) << i << " ]" << endl;
		t_File << "{" << endl;
		for (int eventId : m_EventTTIList[i]) {
			Event& e = m_EventVec[eventId];
			t_File << "    " << e.toString() << endl;
		}
		t_File << "}\n\n" << endl;
	}
}

void TMC::writeEventLogInfo(ofstream &t_File) {
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
		t_File << "EventId = " << eventId << endl;
		t_File << "{" << endl;
		t_File << "    " << "VeUEId = " << m_EventVec[eventId].getVeUEId() << endl;
		t_File << "    " << "MessageType = " << s << endl;
		t_File << "    " << "SendDelay = " << m_EventVec[eventId].getSendDelay() << "(TTI)" << endl;
		t_File << "    " << "QueueDelay = " << m_EventVec[eventId].getQueueDelay() << "(TTI)" << endl;
		t_File << "    " << "ProcessDelay = " << m_EventVec[eventId].getProcessDelay() << "(TTI)" << endl;
		t_File << m_EventVec[eventId].toLogString(1);
		t_File << "}" << endl;
	}
}


