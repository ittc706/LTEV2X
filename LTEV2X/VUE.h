#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"


class cVeUE {
public:
	static int m_VeUECount;

	/*--------------------------------------------------------------
	*                      地理拓扑单元
	* -------------------------------------------------------------*/
	~cVeUE();
	void initializeUrban(sUEConfigure &t_UEConfigure);
	void initializeHigh(sUEConfigure &t_UEConfigure);
	unsigned short m_wRoadID;
	int m_locationID;
	const int m_VeUEId = m_VeUECount++;
	unsigned short m_RSUId;//RRM_DRA模块需要
	unsigned short m_ClusterIdx;//RRM_DRA模块需要
	double m_fX;
	double m_fY;
	double m_fAbsX;
	double m_fAbsY;
	double m_fv;
	double m_fvAngle;
	double m_fantennaAngle;
	//cChannelModel *channelModel;
	cIMTA *imta;

	int m_Nt;//发送天线数目
	int m_Nr;//接收天线数目
	double m_Ploss;//路径损耗
	double *m_H;//信道响应矩阵
	unsigned short m_InterVeUENum;//同频干扰数量
	std::vector<int> m_InterVeUEVec;//同频干扰车辆ID，不包含当前车辆
	std::vector<double> m_InterferencePloss;//干扰路径损耗
	double *m_InterferenceH = nullptr;//干扰信道响应矩阵
	int  m_PreModulation;//上一次的调制方式



	/* HighSpeed */
	unsigned short m_wLaneID;


	std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//地理位置更新日志信息
};



