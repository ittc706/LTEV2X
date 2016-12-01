/*
* =====================================================================================
*
*       Filename:  GTT.cpp
*
*    Description:  TMC模块
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

int GTT_VeUE::s_VeUECount = 0;

GTT_VeUE::~GTT_VeUE() {
	Delete::safeDelete(m_IMTA, true);
	Delete::safeDelete(m_H, true);
	for (double*& p : m_InterferenceH) {
		Delete::safeDelete(p);
	}
	Delete::safeDelete(m_Distance, true);
}


GTT_RSU::~GTT_RSU() {
	Delete::safeDelete(m_IMTA, true);
}

int GTT_RSU::s_RSUCount = 0;

std::string GTT_RSU::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "Road[" << m_RSUId << "]: (" << m_AbsX << "," << m_AbsY << ")" << endl;
	return ss.str();
}


string GTT_eNB::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "eNB[" << m_eNBId << "] : (" << m_AbsX << "," << m_AbsY << ")" << endl;
	ss << indent << "{" << endl;
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	int cnt = 0;
	for (int VeUEId : m_VeUEIdList) {
		if (cnt % 10 == 0)
			ss << indent << "        [ ";
		ss << left << setw(3) << VeUEId << " , ";
		if (cnt % 10 == 9)
			ss << " ]" << endl;
		cnt++;
	}
	if (cnt != 0 && cnt % 10 != 0)
		ss << " ]" << endl;

	ss << indent << "    " << "{" << endl;

	ss << indent << "    " << "RSUIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	cnt = 0;
	for (int RSUId : m_RSUIdList) {
		if (cnt % 10 == 0)
			ss << indent << "        [ ";
		ss << left << setw(3) << RSUId << " , ";
		if (cnt % 10 == 9)
			ss << " ]" << endl;
		cnt++;
	}
	if (cnt != 0 && cnt % 10 != 0)
		ss << " ]" << endl;

	ss << indent << "    " << "{" << endl;
	ss << indent << "}" << endl;
	return ss.str();
}


std::string GTT_Road::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "Road[" << m_RoadId << "]: (" << m_AbsX << "," << m_AbsY << ")" << endl;
	return ss.str();
}


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
