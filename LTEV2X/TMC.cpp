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

const std::vector<int> TMC::s_MESSAGE_PACKAGE_NUM = { 4,4,4 };

const std::vector<std::vector<int>> TMC::s_MESSAGE_BIT_NUM_PER_PACKAGE{
	{ 1520,1520,1520,2400 },
	{ 1520,1520,1520,2400 },
	{ 1520,1520,1520,2400 },
};

const vector<int> TMC::s_INITIAL_WINDOW_SIZE = { 5,5,5 };

const vector<int> TMC::s_MAX_WINDOW_SIZE = { 20,20,20 };

int TMC::s_CONGESTION_LEVEL_NUM = INVALID;

vector<int> TMC::s_PERIODIC_EVENT_PERIOD_PER_CONGESTION_LEVEL;

double TMC::s_EMERGENCY_POISSON = INVALID;

double TMC::s_DATA_POISSON = INVALID;

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
}

TMC::~TMC() {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++)
		Delete::safeDelete(m_VeUEAry[VeUEId]);
	Delete::safeDelete(m_VeUEAry, true);

	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++)
		Delete::safeDelete(m_RSUAry[RSUId]);
	Delete::safeDelete(m_RSUAry, true);
}