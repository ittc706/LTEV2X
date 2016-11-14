#pragma once
#include<regex>
#include<string>
#include<fstream>
#include<set>
#include<map>

class ConfigLoader {
private:
	/*
	* �����ļ��ַ���
	*/
	std::string m_Content;

	/*
	* [��ǩ-ֵ]��
	*/
	std::map<std::string, std::string> m_TagContentMap;
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	ConfigLoader() = delete;

	/*
	* �����ļ����Ĺ��캯��
	*/
	explicit ConfigLoader(std::string t_FilePath);

	/*
	* ��m_TagContentMap���ݱ�ǩ��ȡ��ֵ
	*/
	std::string getParam(std::string t_Param);

	/*
	* ���������ļ�
	*/
	void load();
};