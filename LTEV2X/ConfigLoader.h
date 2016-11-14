#pragma once
#include<regex>
#include<string>
#include<fstream>
#include<set>
#include<map>

class ConfigLoader {
private:
	/*
	* 配置文件字符串
	*/
	std::string m_Content;

	/*
	* [标签-值]对
	*/
	std::map<std::string, std::string> m_TagContentMap;
public:
	/*
	* 默认构造函数定义为删除
	*/
	ConfigLoader() = delete;

	/*
	* 接受文件名的构造函数
	*/
	explicit ConfigLoader(std::string t_FilePath);

	/*
	* 从m_TagContentMap根据标签名取出值
	*/
	std::string getParam(std::string t_Param);

	/*
	* 解析配置文件
	*/
	void load();
};