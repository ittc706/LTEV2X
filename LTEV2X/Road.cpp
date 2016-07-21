#include "Road.h"

ceNB::ceNB(void)
{
}

ceNB::~ceNB(void)
{
}

void ceNB::Initialize(seNBConfigure &t_eNBConfigure)
{
	m_wRoadID = t_eNBConfigure.wRoadID;
	m_eNBId = t_eNBConfigure.weNBID;
	m_fX = t_eNBConfigure.fX;
	m_fY = t_eNBConfigure.fY;
	m_fAbsX = t_eNBConfigure.fAbsX;
	m_fAbsY = t_eNBConfigure.fAbsY;
	printf("»ùÕ¾£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n",m_fAbsX,m_fAbsY);
}


cRoad::cRoad(void)
{
	m_peNB = NULL;
}


cRoad::~cRoad(void)
{
	;
}


void cRoad::Initialize(sRoadConfigure &t_RoadConfigure)
{
	m_wRoadID = t_RoadConfigure.wRoadID;
	m_fAbsX = c_roadTopoRatio[m_wRoadID * 2 + 0]*c_wide;
	m_fAbsY = c_roadTopoRatio[m_wRoadID * 2 + 1]*c_length;
	printf("Road£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n",m_fAbsX,m_fAbsY);

	m_weNBNum = t_RoadConfigure.weNBNum;
    if(m_weNBNum==1)
	{
	m_peNB = (ceNB *)t_RoadConfigure.peNB + t_RoadConfigure.weNBOffset;
	seNBConfigure eNBConfigure;
	eNBConfigure.sys_config=t_RoadConfigure.sys_config;
	eNBConfigure.wRoadID=m_wRoadID;
	eNBConfigure.fX = 42.0f;
	eNBConfigure.fY = -88.5f;
	eNBConfigure.fAbsX = m_fAbsX+eNBConfigure.fX;
	eNBConfigure.fAbsY = m_fAbsY+eNBConfigure.fY;
	eNBConfigure.weNBID = t_RoadConfigure.weNBOffset;
	m_peNB->Initialize(eNBConfigure);
	}


	//sBSConfigure BSConfigure;
	//BSConfigure.sys_config = t_CellConfigure.sys_config;
	//BSConfigure.wCellID = m_wCellID;
	//BSConfigure.fX = 0.0f;
	//BSConfigure.fY = 0.0f;
	//BSConfigure.fAbsX = m_fX;
	//BSConfigure.fAbsY = m_fY;
	//BSConfigure.pSector = sector;
	//BSConfigure.wSecNum = m_wENBSecNum;
	//BSConfigure.BSType = MACRO;
	//BSConfigure.wAntNum = t_CellConfigure.wENBAntNum;
	//BSConfigure.fAntAngle = t_CellConfigure.fENBSecAngle;
	//BSConfigure.wBSID = 0;
	//m_pBS[0].Initialize(BSConfigure);

	//float fPicoX;
	//float fPicoY;
	//float fTune;
	//float fXX;
	//float fYY;
	//float fVX;
	//float fVY;
	//float fDis2;
	//unsigned short wTempBS;
	//cBS *pBS = (cBS *)t_CellConfigure.pBS;
	//for (unsigned short wTempPico = 0; wTempPico != m_wPicoNum; ++ wTempPico)
	//{
	//	do
	//	{
	//		RandomScatter(&fPicoX, &fPicoY, c_cellRadius * sqrt(3.0f) / 3.0f);
	//		fTune = c_PI / 6.0f + c_PI2 / 3.0f * (wTempPico / (m_wPicoNum / m_wENBSecNum));
	//		fXX = fPicoX * cos(fTune) - fPicoY * sin(fTune);
	//		fYY = fPicoX * sin(fTune) + fPicoY * cos(fTune);
	//		fVX = c_cellRadius * sqrt(3.0f) / 3.0f * cos(fTune);
	//		fVY = c_cellRadius * sqrt(3.0f) / 3.0f * sin(fTune);
	//		fPicoX = m_fX + fVX + fXX;
	//		fPicoY = m_fY + fVY + fYY;
	//		for(wTempBS = 0; wTempBS != t_CellConfigure.wBSOffset + 1 + wTempPico; ++ wTempBS)
	//		{
	//			fDis2 = pow(fPicoX - pBS[wTempBS].m_fAbsX, 2.0f) + pow(fPicoY - pBS[wTempBS].m_fAbsY, 2.0f);
	//			if(pBS[wTempBS].m_BSType == MACRO)
	//			{
	//				if(fDis2 < c_cell_heNetMindist * c_cell_heNetMindist)
	//				{
	//					break;
	//				}
	//			}
	//			else
	//			{
	//				if(fDis2 < c_heNet_heNetMindist * c_heNet_heNetMindist)
	//				{
	//					break;
	//				}
	//			}

	//		}
	//		if(wTempBS == t_CellConfigure.wBSOffset + 1 + wTempPico)
	//		{
	//			break;
	//		}
	//	}
	//	while(true);

	//	BSConfigure.fX = fPicoX - m_fX;
	//	BSConfigure.fY = fPicoY - m_fY;
	//	BSConfigure.fAbsX = fPicoX;
	//	BSConfigure.fAbsY = fPicoY;
	//	BSConfigure.pSector = sector + m_wENBSecNum + wTempPico;
	//	BSConfigure.wSecNum = 1;
	//	BSConfigure.BSType = PICO;
	//	BSConfigure.wAntNum = t_CellConfigure.wPicoAntNum;
	//	BSConfigure.fAntAngle = 0.0f;
	//	BSConfigure.wBSID = 1 + wTempPico;
	//	m_pBS[1 + wTempPico].Initialize(BSConfigure);
	//}
}

void cRoad::Destroy()
{
	;
}
