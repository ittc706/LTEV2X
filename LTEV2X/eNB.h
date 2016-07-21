#pragma once
#include<vector>
#include<string>
#include"Global.h"
#include"Schedule.h"


//class ceNB {
//	/*LK*/
//public:
//	ceNB(void);
//	~ceNB(void);
//	void Initialize(seNBConfigure &t_eNBConfigure);
//	void Destroy(void);
//public:
//	//unsigned short m_wCellID;
//	//unsigned short m_wBSID;
//	unsigned short m_wRoadID;
//	unsigned short m_eNBId;
//	float m_fX;
//	float m_fY;
//	float m_fAbsX;
//	float m_fAbsY;
//	//cSector *m_pSector;
//	//unsigned char m_wSecNum;
//	//eBSType m_BSType;
//	//unsigned char m_wAntNum;
//	/*LK*/
//
//
//
//
//
//
//	/*  TEST  */
//	static int s_eNBCount;
//	/*  TEST  */
//public:
//	//const int m_eNBId=s_eNBCount++;//基站Id
//
//	std::list<int> m_RSUIdList;//该基站中的RSU容器（存储RSU的Id）
//	std::list<int> m_VeUEIdList;//该基站中的VeUE容器（存储VeUE的Id）
//
//	/***************************************************************
//	------------------------调度模块--------------------------------
//	****************************************************************/
//	bool m_UnassignedSubband[gc_RBNum];//所有资源块，标记为true指示未被分配的RB
//	std::vector<sScheduleInfo> m_vecScheduleInfo;//调度信息
//
//	std::string toString(int n);//用于打印基站信息
//};