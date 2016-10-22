#include<vector>
#include<math.h>
#include<iostream>
#include<sstream>
#include<utility>
#include<tuple>
#include<iomanip>
#include"RSU.h"
#include"Exception.h"
#include"Event.h"
#include"Global.h"


using namespace std;


cRSU::cRSU() {
	m_GTAT = new GTAT();
	m_GTATUrban = new GTATUrban();
	m_GTATHigh = new GTATHigh();
	m_RRMDRA = new RRMDRA();
	m_RRMRR = new RRMRR();
	m_WT = new WT();
	m_TMAC = new TMAC();
}


void cRSU::initializeUrban(sRSUConfigure &t_RSUConfigure){
	m_GTAT->m_RSUId = t_RSUConfigure.wRSUID;
	m_GTATUrban->m_fAbsX = ns_GTAT_Urban::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 0] * ns_GTAT_Urban::c_wide;
	m_GTATUrban->m_fAbsY = ns_GTAT_Urban::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 1] * ns_GTAT_Urban::c_length;
	RandomUniform(&m_GTATUrban->m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU£º");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_GTAT->m_RSUId, m_GTATUrban->m_fAbsX, m_GTATUrban->m_fAbsY);

	m_GTAT->m_DRAClusterNum = ns_GTAT_Urban::c_RSUClusterNum[m_GTAT->m_RSUId];
	m_GTAT->m_DRAClusterVeUEIdList = vector<list<int>>(m_GTAT->m_DRAClusterNum);
}


void cRSU::initializeHigh(sRSUConfigure &t_RSUConfigure) {
	m_GTAT->m_RSUId = t_RSUConfigure.wRSUID;
	m_GTATHigh->m_fAbsX = ns_GTAT_High::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 0] * 100;
	m_GTATHigh->m_fAbsY = ns_GTAT_High::c_RSUTopoRatio[m_GTAT->m_RSUId * 2 + 1];
	RandomUniform(&m_GTATHigh->m_fantennaAngle, 1, 180.0f, -180.0f, false);
	printf("RSU£º");
	printf("m_wRSUID=%d,m_fAbsX=%f,m_fAbsY=%f\n", m_GTAT->m_RSUId, m_GTATHigh->m_fAbsX, m_GTATHigh->m_fAbsY);

	m_GTAT->m_DRAClusterNum = ns_GTAT_High::c_RSUClusterNum;
	m_GTAT->m_DRAClusterVeUEIdList = vector<list<int>>(m_GTAT->m_DRAClusterNum);
}


cRSU::~cRSU() {
	delete m_GTAT;
	delete m_GTATUrban;
	delete m_GTATHigh;
	delete m_RRM;
	delete m_RRMDRA;
	delete m_RRMRR;
	delete m_WT;
	delete m_TMAC;
}
