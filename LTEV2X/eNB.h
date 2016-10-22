#pragma once
#include<vector>
#include<string>
#include<list>
#include"Global.h"
#include"Config.h"

class ceNB{
public:
	/*--------------------------------------------------------------
	*                      地理拓扑单元
	* -------------------------------------------------------------*/
	int m_wRoadID;
	int m_eNBId;
	double m_fX;
	double m_fY;
	double m_fAbsX;
	double m_fAbsY;
	void initializeUrban(seNBConfigure &t_eNBConfigure);
	void initializeHigh(seNBConfigure &t_eNBConfigure);

    //UNDONE
	std::list<int> m_RSUIdList;//该基站中的RSU容器（存储RSU的Id）
	std::list<int> m_VeUEIdList;//该基站中的VeUE容器（存储VeUE的Id）
	//UNDONE


	std::string toString(int n);//用于打印基站信息
};
