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
	/*--------------------------------------------------------------
	*                      �������˵�Ԫ
	* -------------------------------------------------------------*/

	/*------------------���ݳ�Ա------------------*/
	int m_RSUId;
	std::list<int> m_VeUEIdList;//��ǰRSU��Χ�ڵ�VeUEId�������
	int m_DRAClusterNum;//һ��RSU���Ƿ�Χ�ڵĴصĸ���
	std::vector<std::list<int>> m_DRAClusterVeUEIdList;//���ÿ���ص�VeUE��Id������,�±����صı��
	float m_fAbsX;
	float m_fAbsY;
	//cChannelModel *channelModel;
	cIMTA *imta;
	float m_fantennaAngle;

	/*------------------��Ա����------------------*/
	void initialize(sRSUConfigure &t_RSUConfigure);
};


