/*
* =====================================================================================
*
*       Filename:  GTT.cpp
*
*    Description:  GTT模块
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  LK,WYB
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<iomanip>
#include<sstream>
#include"System.h"
#include"GTT.h"
#include"IMTA.h"
#include"Function.h"
#include"ConfigLoader.h"

#define INVALID -1

using namespace std;




int GTT::s_CONGESTION_LEVEL_NUM = INVALID;

vector<int> GTT::s_VEUE_NUM_PER_CONGESTION_LEVEL;

void GTT::loadConfig(Platform t_Platform) {
	ConfigLoader configLoader;
	if (t_Platform == Windows) {
		configLoader.resolvConfigPath("Config\\GTTConfig.xml");
	}
	else if (t_Platform == Linux) {
		configLoader.resolvConfigPath("Config/GTTConfig.xml");
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

	if ((temp = configLoader.getParam("VeUENumPerCongestionLevel")) != nullString) {
		s_VEUE_NUM_PER_CONGESTION_LEVEL.clear();
		ss << temp;
		string temp2;
		while (ss >> temp2) {
			s_VEUE_NUM_PER_CONGESTION_LEVEL.push_back(ConfigLoader::stringToInt(temp2));
		}
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");


	/*cout << "CongestionLevelNum: " << s_CONGESTION_LEVEL_NUM << endl;
	cout << "VeUENumPerCongestionLevel: " << endl;
	Print::printVectorDim1(s_VEUE_NUM_PER_CONGESTION_LEVEL);
	system("pause");*/
}

int GTT::calcuateCongestionLevel(int t_VeUENum) {
	for (int contestionLevel = 0; contestionLevel < s_CONGESTION_LEVEL_NUM - 1; contestionLevel++)
		if (t_VeUENum < s_VEUE_NUM_PER_CONGESTION_LEVEL[contestionLevel])
			return contestionLevel;
	return s_CONGESTION_LEVEL_NUM - 1;
}

GTT::GTT(System* t_Context) : m_Context(t_Context) {
	if (getContext()->m_Config.platform == Windows) {
		m_FileVeUELocationUpdateLogInfo.open("Log\\GTTLog\\VeUELocationUpdateLogInfo.txt");
		m_FileVeUENumPerRSULogInfo.open("Log\\GTTLog\\VeUENumPerRSULogInfo.txt");
		m_FileLocationInfo.open("Log\\GTTLog\\LocationInfo.txt");
		m_FileVeUEMessage.open("Log\\GTTLog\\VeUEMessage.txt");
	}
	else if (getContext()->m_Config.platform == Linux) {
		m_FileVeUELocationUpdateLogInfo.open("Log/GTTLog/VeUELocationUpdateLogInfo.txt");
		m_FileVeUENumPerRSULogInfo.open("Log/GTTLog/VeUENumPerRSULogInfo.txt");
		m_FileLocationInfo.open("Log/GTTLog/LocationInfo.txt");
		m_FileVeUEMessage.open("Log/GTTLog/VeUEMessage.txt");
	}
	else {
		throw logic_error("Platform Config Error!");
	}
}


GTT::~GTT(){
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++)
		Delete::safeDelete(m_VeUEAry[VeUEId]);
	Delete::safeDelete(m_VeUEAry, true);

	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++)
		Delete::safeDelete(m_RSUAry[RSUId]);
	Delete::safeDelete(m_RSUAry, true);

	for (int eNBId = 0; eNBId < getContext()->m_Config.eNBNum; eNBId++)
		Delete::safeDelete(m_eNBAry[eNBId]);
	Delete::safeDelete(m_eNBAry,true);

	for (int roadId = 0; roadId < getContext()->m_Config.RoadNum; roadId++)
		Delete::safeDelete(m_RoadAry[roadId]);
	Delete::safeDelete(m_RoadAry, true);

	m_FileVeUELocationUpdateLogInfo.close();
	m_FileVeUENumPerRSULogInfo.close();
	m_FileLocationInfo.close();
	m_FileVeUEMessage.close();
}
