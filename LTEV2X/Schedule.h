#pragma once
#include<vector>
#include<utility>
#include<iostream>
#include<tuple>
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


struct sDRAScheduleInfo {
	int VeUEId;//VeUEID
	int FBIdx;//频域块编号
	std::vector<std::tuple<int, int>> occupiedInterval;//当前VeUE进行传输的实际TTI区间（闭区间）

	sDRAScheduleInfo() {}
	sDRAScheduleInfo(int TTI,int VeUEId,int FBIdx, std::tuple<int, int, int>ClasterTTI, int occupiedTTI) :occupiedInterval(std::vector<std::tuple<int, int>>(0)) {
		this->VeUEId = VeUEId;
		this->FBIdx = FBIdx;
		int begin = std::get<0>(ClasterTTI),
			end = std::get<1>(ClasterTTI),
			len = std::get<2>(ClasterTTI);
		int relativeTTI = TTI%gc_DRA_NTTI;
		int nextTurnBeginTTI = TTI - relativeTTI + gc_DRA_NTTI;//该RSU下一轮调度的起始TTI（第一个簇的开始时刻）
		int remainTTI = end - relativeTTI + 1;//当前一轮调度中剩余可用的时隙数量
		int overTTI = occupiedTTI - remainTTI;//超出当前一轮调度可用时隙数量的部分
		if (overTTI <= 0) occupiedInterval.push_back(std::tuple<int, int>(TTI, TTI + occupiedTTI - 1));
		else {
			occupiedInterval.push_back(std::tuple<int, int>(TTI, TTI + remainTTI - 1));
			int n = overTTI / len;
			for (int i = 0; i < n; i++) occupiedInterval.push_back(std::tuple<int, int>(nextTurnBeginTTI + i*gc_DRA_NTTI + begin, nextTurnBeginTTI + begin + len - 1 + i*gc_DRA_NTTI));
			if (overTTI%len != 0) occupiedInterval.push_back(std::tuple<int, int>(nextTurnBeginTTI + n*gc_DRA_NTTI + begin, nextTurnBeginTTI + begin + n*gc_DRA_NTTI + overTTI%len - 1));
		}
	}
	
	std::string toLogString();

	/*------------------测试用的函数------------------*/
	void print() {
		std::cout << "OccupiedInterval: ";
		for (const std::tuple<int, int> &t : occupiedInterval)
			std::cout << "[ " << std::get<0>(t) << " , " << std::get<1>(t) << " ] , ";
		std::cout << std::endl;
	}
};