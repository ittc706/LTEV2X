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
	*                      �������˵�Ԫ
	* -------------------------------------------------------------*/
	~cVeUE();
	void initializeUrban(sUEConfigure &t_UEConfigure);
	void initializeHigh(sUEConfigure &t_UEConfigure);
	unsigned short m_wRoadID;
	int m_locationID;
	const int m_VeUEId = m_VeUECount++;
	unsigned short m_RSUId;//RRM_DRAģ����Ҫ
	unsigned short m_ClusterIdx;//RRM_DRAģ����Ҫ
	float m_fX;
	float m_fY;
	float m_fAbsX;
	float m_fAbsY;
	float m_fv;
	float m_fvAngle;
	float m_fantennaAngle;
	//cChannelModel *channelModel;
	cIMTA *imta;

	int m_Nt;//����������Ŀ
	int m_Nr;//����������Ŀ
	float m_Ploss;//·�����
	float *m_H;//�ŵ���Ӧ����
	unsigned short m_InterVeUENum;//ͬƵ��������
	std::vector<int> m_InterVeUEVec;//ͬƵ���ų���ID����������ǰ����
	std::vector<double> m_InterferencePloss;//����·�����
	float *m_InterferenceH = nullptr;//�����ŵ���Ӧ����
	int  m_PreModulation;//��һ�εĵ��Ʒ�ʽ



	/* HighSpeed */
	unsigned short m_wLaneID;


	std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//����λ�ø�����־��Ϣ
};



