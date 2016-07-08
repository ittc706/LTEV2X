#pragma once
#include<vector>
#include"Sector.h"
#include"RSU.h"

class ceNB {
public:
	int m_CellId;//小区ID
	int m_eNBId;//基站ID
	float m_X, m_Y;//相对坐标
	float m_AbsX, m_AbsY;//绝对坐标
	int m_AntennaNum;//天线数量
	int m_SectorNum;//扇区数量
	std::vector<int> m_VecSector;//该基站中扇区的容器(存储扇区的ID)
	std::vector<int> m_VecRSU;//该基站中的RSU容器（存储RSU的ID）
	std::vector<int> m_VecVeUE;

	/***************************************************************
	------------------------调度模块--------------------------------
	****************************************************************/
	bool m_UnassignedSubband[gc_RBNum];//所有资源块，标记为true指示未被分配的RB
	std::vector<sScheduleInfo> m_vecScheduleInfo;//调度信息
};