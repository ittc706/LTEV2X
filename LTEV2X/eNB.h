#pragma once
#include<vector>
#include<string>
#include<list>
#include"Global.h"
#include"Config.h"

class eNB{
public:
	/*--------------------------------------------------------------
	*                      �������˵�Ԫ
	* -------------------------------------------------------------*/
	int m_RoadId;
	int m_eNBId;
	double m_X;
	double m_Y;
	double m_AbsX;
	double m_AbsY;
	void initializeUrban(seNBConfigure &t_eNBConfigure);
	void initializeHighSpeed(seNBConfigure &t_eNBConfigure);

    //UNDONE
	std::list<int> m_RSUIdList;//�û�վ�е�RSU�������洢RSU��Id��
	std::list<int> m_VeUEIdList;//�û�վ�е�VeUE�������洢VeUE��Id��
	//UNDONE


	std::string toString(int n);//���ڴ�ӡ��վ��Ϣ
};
