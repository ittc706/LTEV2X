#pragma once
#include<vector>
#include<list>
#include<string>
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
	void initialize(sUEConfigure &t_UEConfigure);
	unsigned short m_wRoadID;
	int m_locationID;
	const int m_VeUEId = m_VeUECount++;
	unsigned short m_RSUId;
	unsigned short m_ClusterIdx;
	unsigned short m_interUEnum;
	float m_fX;
	float m_fY;
	float m_fAbsX;
	float m_fAbsY;
	float m_fv;
	float m_fvAngle;
	float m_fantennaAngle;
	//cChannelModel *channelModel;
	cIMTA *imta;

	int m_Nt;
	int m_Nr;
	float *m_H;
	int   *m_interUEArray;
	float *m_interH;

	float m_Pl;
	float *m_interPl;
	int  m_Mol;//上一次的调制方式


	std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//地理位置更新日志信息
};



