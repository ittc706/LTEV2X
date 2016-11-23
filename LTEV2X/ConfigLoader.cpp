/*
* =====================================================================================
*
*       Filename:  ConfigLoader.cpp
*
*    Description:  TMCÄ£¿é
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
#include"ConfigLoader.h"

using namespace std;

void ConfigLoader::resolvConfigPath(string t_FilePath) {
	//Çå¿Õ»º´æ
	m_Content.clear();
	m_TagContentMap.clear();

	//¶ÁÈ¡´ý½âÎö×Ö·û´®
	ifstream in(t_FilePath);
	istream_iterator<char> if_it(in), if_eof;
	m_Content.assign(if_it, if_eof);
	in.close();

	//½âÎö£¬²¢´æ´¢
	regex r("<([^<>]*)>([^<>]*)</([^<>]*)>");
	for (sregex_iterator it(m_Content.begin(), m_Content.end(), r), eof; it != eof; ++it) {
		string leftTag = it->operator[](1);
		string rightTag = it->operator[](3);
		string content = it->operator[](2);
		if (leftTag != rightTag) {
			throw Exp("tag not match");
		}
		m_TagContentMap[leftTag] = content;
	}
}

std::string ConfigLoader::getParam(std::string t_Param) {
	if (m_TagContentMap.find(t_Param) == m_TagContentMap.end()) return "";
	return m_TagContentMap[t_Param];
}