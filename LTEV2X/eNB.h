#pragma once
#include<vector>
#include<string>
#include"RSU.h"

class ceNB {
	//-----------------------TEST-----------------------
public:
	static int count;
	ceNB();
	//-----------------------TEST-----------------------
public:
	int m_eNBId;//基站ID

	std::list<int> m_RSUIdList;//该基站中的RSU容器（存储RSU的ID）
	std::list<int> m_VeUEList;//该基站中的VeUE容器（存储VeUE的ID）

	/***************************************************************
	------------------------调度模块--------------------------------
	****************************************************************/
	bool m_UnassignedSubband[gc_RBNum];//所有资源块，标记为true指示未被分配的RB
	std::vector<sScheduleInfo> m_vecScheduleInfo;//调度信息

	std::string toString();//用于打印基站信息
};