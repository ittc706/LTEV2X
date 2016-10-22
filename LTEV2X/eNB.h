#pragma once
#include<vector>
#include<string>
#include<list>
#include"Global.h"
#include"Config.h"

class eNB{
public:
	/*--------------------------------------------------------------
	*                      地理拓扑单元
	* -------------------------------------------------------------*/
	int m_RoadId;
	int m_eNBId;
	double m_X;
	double m_Y;
	double m_AbsX;
	double m_AbsY;
	void initializeUrban(seNBConfigure &t_eNBConfigure);
	void initializeHighSpeed(seNBConfigure &t_eNBConfigure);

    //UNDONE
	std::list<int> m_RSUIdList;//该基站中的RSU容器（存储RSU的Id）
	std::list<int> m_VeUEIdList;//该基站中的VeUE容器（存储VeUE的Id）
	//UNDONE


	std::string toString(int n);//用于打印基站信息
};
