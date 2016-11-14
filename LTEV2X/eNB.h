#pragma once
#include<vector>
#include<string>
#include<list>
#include"Global.h"
#include"Config.h"

class eNB{
	/*------------------��------------------*/
public:
	int m_RoadId;
	int m_eNBId;
	double m_X;
	double m_Y;
	double m_AbsX;
	double m_AbsY;
	

    //<UNDONE>
	std::list<int> m_RSUIdList;//�û�վ�е�RSU�������洢RSU��Id��
	std::list<int> m_VeUEIdList;//�û�վ�е�VeUE�������洢VeUE��Id��
	//<UNDONE>

	/*------------------����------------------*/
public:
	void initializeUrban(eNBConfig &t_eNBConfigure);
	void initializeHighSpeed(eNBConfig &t_eNBConfigure);
	std::string toString(int n);//���ڴ�ӡ��վ��Ϣ
};
