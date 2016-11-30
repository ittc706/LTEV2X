#pragma once
#include<regex>
#include<string>
#include<fstream>
#include<map>

class ConfigLoader {
	/*------------------静态------------------*/
public:
	/*
	* 将String转为Int
	*/
	static int stringToInt(std::string t_String);

	/*
	* 将String转为Double
	*/
	static double stringToDouble(std::string t_String);
private:
	/*
	* 配置文件字符串
	*/
	std::string m_Content;

	/*
	* [标签-值]对
	*/
	std::map<std::string, std::string> m_TagContentMap;

	/*------------------方法------------------*/
public:
	/*
	* 默认构造函数
	*/
	ConfigLoader() {}

	/*
	* 接受文件名的构造函数
	*/
	void resolvConfigPath(std::string t_FilePath);

	/*
	* 从m_TagContentMap根据标签名取出值
	*/
	std::string getParam(std::string t_Param);
};