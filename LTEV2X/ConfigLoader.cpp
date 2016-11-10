#include<iostream>
#include"ConfigLoader.h"

using namespace std;

ConfigLoader::ConfigLoader(string t_FilePath) {
	ifstream in(t_FilePath);
	istream_iterator<char> if_it(in), if_eof;
	m_Content.assign(if_it, if_eof);
	in.close();
}

void ConfigLoader::load() {
	regex r("<([^<>]*)>([^<>]*)</([^<>]*)>");
	for (sregex_iterator it(m_Content.begin(), m_Content.end(), r), eof; it != eof; ++it) {
		string leftTag = it->operator[](1);
		string rightTag = it->operator[](3);
		string content = it->operator[](2);
		if (leftTag != rightTag) {
			throw exception("tag not match");
		}
		m_TagContentMap[leftTag] = content;
	}
	/*for (pair<string, string> p : m_TagContentMap) 
		cout << "[" << p.first << "," << p.second <<"]"<< endl;*/
}

std::string ConfigLoader::getParam(std::string t_Param) {
	if (m_TagContentMap.find(t_Param) == m_TagContentMap.end()) return "";
	return m_TagContentMap[t_Param];
}