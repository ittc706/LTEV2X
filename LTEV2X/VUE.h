#pragma once
#include<vector>
#include<list>
#include<string>
#include"Global.h"
#include"Schedule.h"
#include"Message.h"

class cVeUE {
	//-----------------------TEST-----------------------
public:
	static int m_VeUECount;
	//-----------------------TEST-----------------------
public:
	const int m_VeUEId=m_VeUECount++;//用户ID
	int m_RSUId;//所在的RSU的Id
	int m_ClusterIdx;//所在簇de编号

	/***************************************************************
	------------------------调度模块--------------------------------
	****************************************************************/

	bool m_IsScheduledUL;    //UpLink是否在被调度
	sFeedbackInfo m_FeedbackUL;//将要发送给基站端的反馈信息

	std::vector<double> m_CQIPredictIdeal;
	std::vector<double> m_CQIPredictRealistic;

	/***************************************************************
	---------------------分布式资源管理-----------------------------
	****************************************************************/

	int RBSelectBasedOnP2(const std::vector<int> &v);


	std::string toString(int n);//用于打印VeUE信息
};