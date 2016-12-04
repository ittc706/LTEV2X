/*
* =====================================================================================
*
*       Filename:  GTT_VeUE.cpp
*
*    Description:  GTTģ���VeUE��ͼ�����в����ɱ�����ģ�����
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  LK,WYB
*            LIB:  ITTC
*
* =====================================================================================
*/

#include"GTT_VeUE.h"

#include"IMTA.h"
#include"Function.h"

using namespace std;

int GTT_VeUE::s_VeUECount = 0;

GTT_VeUE::~GTT_VeUE() {
	Delete::safeDelete(m_IMTA, true);
	Delete::safeDelete(m_H, true);
	for (double*& p : m_InterferenceH) {
		Delete::safeDelete(p);
	}
	Delete::safeDelete(m_Distance, true);
}


GTT_Urban_VeUE::GTT_Urban_VeUE(VeUEConfig &t_VeUEConfig) {

	m_RoadId = t_VeUEConfig.roadId;
	m_X = t_VeUEConfig.X;
	m_Y = t_VeUEConfig.Y;
	m_AbsX = t_VeUEConfig.AbsX;
	m_AbsY = t_VeUEConfig.AbsY;
	m_V = t_VeUEConfig.V / 3.6;//����Ϊm/s
	m_VAngle = t_VeUEConfig.angle;

	IMTA::randomUniform(&m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_Nt = 1;
	m_Nr = 2;
	m_H = new double[2 * 1024 * 2];
	m_InterferencePloss = vector<double>(t_VeUEConfig.VeUENum, 0);
	m_InterferenceH = vector<double*>(t_VeUEConfig.VeUENum, nullptr);
}


GTT_HighSpeed_VeUE::GTT_HighSpeed_VeUE(VeUEConfig &t_VeUEConfig) {
	m_RoadId = t_VeUEConfig.roadId;
	m_X = t_VeUEConfig.X;
	m_Y = t_VeUEConfig.Y;
	m_AbsX = t_VeUEConfig.AbsX;
	m_AbsY = t_VeUEConfig.AbsY;
	m_V = t_VeUEConfig.V / 3.6f;//��km/h����Ϊm/s�����ڳ�����λ�ø���

	if (m_RoadId <= 2)
		m_VAngle = 0;
	else
		m_VAngle = 180;

	IMTA::randomUniform(&m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_Nt = 1;
	m_Nr = 2;
	m_H = new double[2 * 1024 * 2];
	m_InterferencePloss = vector<double>(t_VeUEConfig.VeUENum, 0);
	m_InterferenceH = vector<double*>(t_VeUEConfig.VeUENum, nullptr);
}
