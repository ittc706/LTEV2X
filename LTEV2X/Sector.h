#pragma once
#include<list>
#include<vector>
#include"Schedule.h"

class Sector {
public:
	int m_CellId;//小区ID
	int m_eNBId;//基站ID
	int m_SectorId;//扇区ID
	float m_AntennaAngle;//天线角
	int m_AntennaTxNumber; //发送时扇区天线数
	int m_AntennaRxNumber; //接受时扇区天线数
	float m_X, m_Y;//相对坐在小区的坐标
	float m_AbsX, m_AbsY;//绝对坐标
	float m_RSPower;//导频功率
	float m_DataPower;//数据发送功率
	float m_SumPower;//总功率


	/***************************************************************
	------------------------调度模块--------------------------------
	****************************************************************/

	std::list<int> admitUserIdListDL;//接纳用户链表，含切换(存储用户的ID)
	std::list<int> admitUserIdListUL;//接纳用户链表，含切换(存储用户的ID)

	std::vector<sScheduleInfo> scheduleInfoPerHDL;//每频段下行调度信息（存储当前一次的调度信息）
	std::vector<sScheduleInfo> scheduleInfoPerHUL;//每频段上行调度信息（存储当前一次的调度信息）

	std::vector<std::vector<sScheduleInfo>> vecScheduleInfoUL;//累计下行调度信息
	std::vector<std::vector<sScheduleInfo>> vecScheduleInfoDL;//累计上行调度信息


	/***************************************************************
	---------------------分布式资源管理-----------------------------
	****************************************************************/


};