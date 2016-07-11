#pragma once
#include<vector>
#include<string>
#include<set>
#include"RSU.h"

class ceNB {
	//-----------------------TEST-----------------------
public:
	static int count;
	ceNB();
	//-----------------------TEST-----------------------
public:
	int m_eNBId;//基站ID

	std::set<int> m_RSUSet;//该基站中的RSU容器（存储RSU的ID）
	std::set<int> m_VUESet;//该基站中的VE容器（存储VE的ID）

	/***************************************************************
	------------------------调度模块--------------------------------
	****************************************************************/
	bool m_UnassignedSubband[gc_RBNum];//所有资源块，标记为true指示未被分配的RB
	std::vector<sScheduleInfo> m_vecScheduleInfo;//调度信息

	std::string toString();//用于打印基站信息
};