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

#define INVALID -1

using namespace std;

int TMC::s_CONGESTION_LEVEL_NUM = INVALID;

vector<int> TMC::s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL;

double TMC::s_EMERGENCY_POISSON = INVALID;

double TMC::s_DATA_POISSON = INVALID;

vector<int> TMC::s_MESSAGE_PACKAGE_NUM;

vector<std::vector<int>> TMC::s_MESSAGE_BIT_NUM_PER_PACKAGE;

vector<int> TMC::s_INITIAL_WINDOW_SIZE;

vector<int> TMC::s_MAX_WINDOW_SIZE;

/*
* 加载TMC模块配置参数
*/
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