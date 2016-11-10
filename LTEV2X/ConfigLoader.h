#pragma once
#include<regex>
#include<string>
#include<fstream>
#include<set>
#include<map>

class ConfigLoader {
private:
	std::string m_Content;
	std::map<std::string, std::string> m_TagContentMap;
public:
	ConfigLoader() = delete;
	explicit ConfigLoader(std::string t_FilePath);
	std::string getParam(std::string t_Param);
	void load();
};