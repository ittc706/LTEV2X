#pragma once
#include<vector>
#include<string>
#include<list>
#include"Global.h"
#include"Config.h"

class ceNB{
public:
	/*--------------------------------------------------------------
	*                      �������˵�Ԫ
	* -------------------------------------------------------------*/
	int m_wRoadID;
	int m_eNBId;
	double m_fX;
	double m_fY;
	double m_fAbsX;
	double m_fAbsY;
	void initializeUrban(seNBConfigure &t_eNBConfigure);
	void initializeHigh(seNBConfigure &t_eNBConfigure);

    //UNDONE
	std::list<int> m_RSUIdList;//�û�վ�е�RSU�������洢RSU��Id��
	std::list<int> m_VeUEIdList;//�û�վ�е�VeUE�������洢VeUE��Id��
	//UNDONE


	std::string toString(int n);//���ڴ�ӡ��վ��Ϣ
};
