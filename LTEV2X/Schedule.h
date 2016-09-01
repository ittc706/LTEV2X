#pragma once
#include<vector>
#include<list>
#include<utility>
#include<iostream>
#include<tuple>
#include "Enumeration.h"
#include"Global.h"

struct sScheduleInfo {
	int userId;//当前小区在该RB上调度到的用户数
	std::vector<int> assignedSubbandId;//该用户所分配的子带的编号[0-gc_SubbandNum)
	sScheduleInfo() :userId(-1), assignedSubbandId(std::vector<int>(0)) {}
};


struct sFeedbackInfo {
public:
	std::vector<float> CQIWideband;//宽带CQI
	std::vector<float> CQISubband;//窄带CQI

	sFeedbackInfo& operator=(const sFeedbackInfo&f) {//拷贝赋值运算符
		CQIWideband = f.CQIWideband;
		CQISubband = f.CQISubband;
		return *this;
	}
};





struct sPFInfo {//仅用于PF上行调度算法的数据类型
	int RSUId;
	int SubbandId;
	double FactorPF;
	sPFInfo(int RSUId, int SubbandId, double FactorPF) {
		this->RSUId = RSUId;
		this->SubbandId = SubbandId;
		this->FactorPF = FactorPF;
	}
	sPFInfo(const sPFInfo& t) {
		this->RSUId = t.RSUId;
		this->SubbandId = t.SubbandId;
		this->FactorPF = t.FactorPF;
	}
	sPFInfo& operator=(const sPFInfo& t) {
		this->RSUId = t.RSUId;
		this->SubbandId = t.SubbandId;
		this->FactorPF = t.FactorPF;
		return *this;
	}
};


struct sRRScheduleInfo {
	int eventId;//事件编号
	eMessageType messageType;//事件类型
	int VeUEId;//车辆编号
	int RSUId;//RSU编号
	int patternIdx;//频域块编号
	std::tuple<int, int> occupiedInterval;//当前VeUE进行传输的实际TTI区间（闭区间）

	sRRScheduleInfo() {}
	sRRScheduleInfo(int eventId, eMessageType messageType, int VeUEId, int RSUId, int patternIdx,const std::tuple<int, int> &occupiedInterval) {
		this->eventId = eventId;
		this->messageType = messageType;
		this->VeUEId = VeUEId;
		this->RSUId = RSUId;
		this->patternIdx = patternIdx;
		this->occupiedInterval = occupiedInterval;
	}

	std::string toLogString(int n);

	/*
	* 生成表示调度信息的string对象
	* 包括事件的Id，车辆的Id，以及要传输该事件所占用的TTI区间
	*/
	std::string toScheduleString(int n);

};


