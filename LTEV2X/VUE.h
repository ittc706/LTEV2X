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
	double m_fX;
	double m_fY;
	double m_fAbsX;
	double m_fAbsY;
	double m_fv;
	double m_fvAngle;
	double m_fantennaAngle;
	//cChannelModel *channelModel;
	cIMTA *imta;

	int m_Nt;//����������Ŀ
	int m_Nr;//����������Ŀ
	double m_Ploss;//·�����
	double *m_H;//�ŵ���Ӧ����
	unsigned short m_InterVeUENum;//ͬƵ��������
	std::vector<int> m_InterVeUEVec;//ͬƵ���ų���ID����������ǰ����
	std::vector<double> m_InterferencePloss;//����·�����
	double *m_InterferenceH = nullptr;//�����ŵ���Ӧ����
	int  m_PreModulation;//��һ�εĵ��Ʒ�ʽ



	/* HighSpeed */
	unsigned short m_wLaneID;


	std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//����λ�ø�����־��Ϣ
};



