#pragma once
#include<vector>
#include<list>
#include"Global.h"
#include"Schedule.h"

class cVeUE {
public:
	int m_AntennaTxNum;//发送天线数
	int m_AntennaRxNum;//接收天线数
	float m_X, m_Y;//相对所在小区的坐标
	float m_AbsX, m_AbsY;//绝对坐标
	int m_UserId;//用户ID
	std::list<int> m_HandoverSectorList;//用户的切换小区链表
	bool m_IsCall;//用户呼叫标志
	bool m_IsCac;//用户被接纳标志
	std::vector<float> m_LSFadingFromUE2eNB;//	用户到所有基站的大中尺度


	/***************************************************************
	------------------------调度模块--------------------------------
	****************************************************************/

	bool m_IsScheduledUL;    //UpLink是否在被调度
	bool m_IsScheduledDL;    //DownLink是否在被调度
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