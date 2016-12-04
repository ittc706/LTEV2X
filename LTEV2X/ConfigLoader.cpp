/*
* =====================================================================================
*
*       Filename:  ConfigLoader.cpp
*
*    Description:  �����ļ�������
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
	//����doubleʵ��̫���ˣ��ݲ���������ˣ���Ĭ������Ϊ������Χ
	stringstream ss;
	ss << t_String;
	double res;
	ss >> res;
	return res;
}

void ConfigLoader::resolvConfigPath(string t_FilePath) {
	//��ջ���
	m_Content.clear();
	m_TagContentMap.clear();

	//��ȡ�������ַ���
	ifstream in(t_FilePath);
	istreambuf_iterator<char> if_it(in), if_eof;
	m_Content.assign(if_it, if_eof);
	in.close();

	//���������洢
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