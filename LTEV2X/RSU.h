#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include<tuple>
#include"Global.h"
#include"VUE.h"
#include"Event.h"
#include"IMTA.h"

class cRSU {
public:
	cRSU() {}
	/*------------------数据成员------------------*/
	int m_RSUId;
	std::list<int> m_VeUEIdList;//当前RSU范围内的VeUEId编号容器,RRM_DRA模块需要
	int m_DRAClusterNum;//一个RSU覆盖范围内的簇的个数,RRM_DRA模块需要
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;//存放每个簇的VeUE的Id的容器,下标代表簇的编号
	float m_fAbsX;
	float m_fAbsY;

	cIMTA *imta;
	float m_fantennaAngle;

	/*------------------成员函数------------------*/
	void initializeUrban(sRSUConfigure &t_RSUConfigure);
	void initializeHigh(sRSUConfigure &t_RSUConfigure);
};


