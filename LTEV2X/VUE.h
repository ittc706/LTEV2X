#pragma once
#include<vector>
#include<list>
#include<string>
#include"Global.h"
#include"Schedule.h"
#include"Enumeration.h"


class cVeUE {
	//-----------------------TEST-----------------------
public:
	static int m_VeUECount;
	//-----------------------TEST-----------------------
public:
	const int m_VeUEId=m_VeUECount++;//用户ID
	int m_RSUId;//所在的RSU的Id
	int m_ClusterIdx;//所在簇de编号

	std::list<std::tuple<int,int>> m_LocationUpdateLogInfoList;

	/***************************************************************
	---------------------集中式资源管理-----------------------------
	****************************************************************/

	bool m_IsScheduledUL;    //UpLink是否在被调度
	sFeedbackInfo m_FeedbackUL;//将要发送给基站端的反馈信息

	std::vector<double> m_CQIPredictIdeal;
	std::vector<double> m_CQIPredictRealistic;

	/***************************************************************
	---------------------分布式资源管理-----------------------------
	****************************************************************/

	//int RBSelectBasedOnP2(const std::vector<int>(&curAvaliablePatternIdx)[cRSU::s_DRAPatternTypeNum], eMessageType messageType);
	int RBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType);


	std::string toString(int n);//用于打印VeUE信息
};

inline
int cVeUE::RBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, eMessageType messageType) {
	int size = static_cast<int>(curAvaliablePatternIdx[messageType].size());
	return curAvaliablePatternIdx[messageType][rand() % size];
}