#pragma once
#include<vector>
#include<string>
#include"Global.h"
#include"Schedule.h"
#include"Config.h"

class ceNB{
public:
	/*--------------------------------------------------------------
	*                      地理拓扑单元
	* -------------------------------------------------------------*/
	unsigned short m_wRoadID;
	unsigned short m_eNBId;
	float m_fX;
	float m_fY;
	float m_fAbsX;
	float m_fAbsY;
	void initialize(seNBConfigure &t_eNBConfigure);


    //UNDONE
	std::list<int> m_RSUIdList;//该基站中的RSU容器（存储RSU的Id）
	std::list<int> m_VeUEIdList;//该基站中的VeUE容器（存储VeUE的Id）
	//UNDONE


	/*----------------------------------------------------
	*                   分布式资源管理
	*          DRA:Distributed Resource Allocation
	* ---------------------------------------------------*/
	bool m_UnassignedSubband[gc_RBNum];//所有资源块，标记为true指示未被分配的RB
	std::vector<sScheduleInfo> m_vecScheduleInfo;//调度信息

	std::string toString(int n);//用于打印基站信息
};
