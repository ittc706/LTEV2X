#pragma once
#include<vector>
#include<list>
#include<string>
#include<tuple>
#include"Global.h"
#include"Schedule.h"
#include"Enumeration.h"


class cVeUE {
	/*  TEST  */
public:
	static int m_VeUECount;
	/*  TEST  */
public:
	const int m_VeUEId=m_VeUECount++;//用户Id
	int m_RSUId;//所在的RSU的Id
	int m_ClusterIdx;//所在簇的编号
	std::tuple<int,int> m_ScheduleInterval;//该VeUE在当前簇内当前一轮调度区间
	std::list<std::tuple<int,int>> m_LocationUpdateLogInfoList;

	/*--------------------------------------------------------------
	*                      上行调度
	* -------------------------------------------------------------*/

	bool m_IsScheduledUL;    //UpLink是否在被调度
	sFeedbackInfo m_FeedbackUL;//将要发送给基站端的反馈信息

	std::vector<double> m_CQIPredictIdeal;
	std::vector<double> m_CQIPredictRealistic;

	/*--------------------------------------------------------------
	*                      分布式资源管理
	*            DRA:Distributed Resource Allocation
	* -------------------------------------------------------------*/

	int RBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType);
	int RBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx);

	std::string toString(int n);//用于打印VeUE信息
};

inline
int cVeUE::RBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType) {
	int size = static_cast<int>(curAvaliablePatternIdx[messageType].size());
	if (size == 0) return -1;
	return curAvaliablePatternIdx[messageType][rand() % size];
}

inline
int cVeUE::RBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx) {
	int size = static_cast<int>(curAvaliableEmergencyPatternIdx.size());
	if (size == 0) return -1;
	return curAvaliableEmergencyPatternIdx[rand() % size];
}