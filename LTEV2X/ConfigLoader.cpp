/*
* =====================================================================================
*
*       Filename:  ConfigLoader.cpp
*
*    Description:  配置文件加载器
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

#include<iostream>
#include<sstream>
#include<stdexcept>
#include"ConfigLoader.h"

using namespace std;

int ConfigLoader::stringToInt(std::string t_String) {
	if (t_String.size() > 10) throw logic_error("IntOverFlow");
	stringstream ss;
	ss << t_String;
	long res;
	ss >> res;
	if(res>(numeric_limits<int>::max)()|| res<(numeric_limits<int>::min)()) throw logic_error("IntOverFlow");
	return static_cast<int>(res);
}

double ConfigLoader::stringToDouble(std::string t_String) {
	//由于double实在太大了，暂不处理溢出了，即默认输入为正常范围
	stringstream ss;
	ss << t_String;
	double res;
	ss >> res;
	return res;
}

void ConfigLoader::resolvConfigPath(string t_FilePath) {
	//清空缓存
	m_Content.clear();
	m_TagContentMap.clear();

	//读取待解析字符串
	ifstream in(t_FilePath);
	istreambuf_iterator<char> if_it(in), if_eof;
	m_Content.assign(if_it, if_eof);
	in.close();

	//解析，并存储
	regex r("<([^<>]*)>([^<>]*)</([^<>]*)>");
	for (sregex_iterator it(m_Content.begin(), m_Content.end(), r), eof; it != eof; ++it) {
		string leftTag = it->operator[](1);
		string rightTag = it->operator[](3);
		string content = it->operator[](2);
		if (leftTag != rightTag) {
			throw logic_error("tag not match");
		}
		m_TagContentMap[leftTag] = content;
	}
}

std::string ConfigLoader::getParam(std::string t_Param) {
	if (m_TagContentMap.find(t_Param) == m_TagContentMap.end()) return "";
	return m_TagContentMap[t_Param];
}