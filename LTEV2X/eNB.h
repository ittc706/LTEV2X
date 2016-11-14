#pragma once
#include<vector>
#include<string>
#include<list>
#include"Global.h"
#include"Config.h"

class eNB{
	/*------------------��------------------*/
public:
	/*
	* ���ڵ�·Id
	*/
	int m_RoadId;

	/*
	* ��վId
	*/
	int m_eNBId;

	/*
	* ��վ��Ժ�������
	*/
	double m_X, m_Y;

	/*
	* ��վ���Ժ�������
	*/
	double m_AbsX, m_AbsY;
	
    //<UNDONE>
	/*
	* �û�վ�е�RSU����(�洢RSU��Id)
	*/
	std::list<int> m_RSUIdList;

	//<UNDONE>
	/*
	* �û�վ�е�VeUE����(�洢VeUE��Id)
	*/
	std::list<int> m_VeUEIdList;


	/*------------------����------------------*/
public:
	/*
	* ���򳡾��µĳ�ʼ������
	*/
	void initializeUrban(eNBConfig &t_eNBConfig);

	/*
	* ���ٳ����µĳ�ʼ������
	*/
	void initializeHighSpeed(eNBConfig &t_eNBConfig);

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);
};
