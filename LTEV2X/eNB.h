#pragma once
#include<vector>
#include<string>
#include"Global.h"
#include"Schedule.h"
#include"Config.h"

class ceNB{
public:
	/*--------------------------------------------------------------
	*                      �������˵�Ԫ
	* -------------------------------------------------------------*/
	unsigned short m_wRoadID;
	unsigned short m_eNBId;
	float m_fX;
	float m_fY;
	float m_fAbsX;
	float m_fAbsY;
	void initialize(seNBConfigure &t_eNBConfigure);


    //UNDONE
	std::list<int> m_RSUIdList;//�û�վ�е�RSU�������洢RSU��Id��
	std::list<int> m_VeUEIdList;//�û�վ�е�VeUE�������洢VeUE��Id��
	//UNDONE


	/*----------------------------------------------------
	*                   �ֲ�ʽ��Դ����
	*          DRA:Distributed Resource Allocation
	* ---------------------------------------------------*/
	bool m_UnassignedSubband[gc_RBNum];//������Դ�飬���Ϊtrueָʾδ�������RB
	std::vector<sScheduleInfo> m_vecScheduleInfo;//������Ϣ

	std::string toString(int n);//���ڴ�ӡ��վ��Ϣ
};
