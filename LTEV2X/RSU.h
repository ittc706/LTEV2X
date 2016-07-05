#pragma once

#include<vector>
#include<list>
#include"Schedule.h"
#include"Global.h"

class cRSU {
public:
	/***************************************************************
	------------------------上行调度--------------------------------
	****************************************************************/
	double m_AccumulateThroughput;   //累计吞吐量
	bool m_IsScheduledUL;    //UpLink是否在被调度
	bool m_IsScheduledDL;    //DownLink是否在被调度
	double m_FactorPF[gc_SubbandNum];
	double m_SINR[gc_SubbandNum];
	std::list<int> m_HIndicatorUL; //传输数据位置
	std::list<int> m_HIndicatorDL; //传输数据位置
	int  m_RBs[gc_MaxCodewords];  //频域*空间
	cServingSector m_ServingSet;
	sFeedbackInfo m_FeedbackDL;//将要发送给基站端的反馈信息
	sFeedbackInfo m_FeedbackUL;//将要发送给基站端的反馈信息

	std::vector<double> m_CQIPredictIdeal;
	std::vector<double> m_CQIPredictRealistic;
	int m_ServingSectorId; //用户属于扇区的ID = 小区ID*每小区扇区数+小区内扇区编号 主服务小区

	bool isScheduled(eLinkType);//当前TTI该用户是否被调度
};