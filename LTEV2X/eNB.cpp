#include"eNB.h"
#include"Config.h"
using namespace std;


void ceNB::initialize(seNBConfigure &t_eNBConfigure)
{
	m_wRoadID = t_eNBConfigure.wRoadID;
	m_eNBId = t_eNBConfigure.weNBID;
	m_fX = t_eNBConfigure.fX;
	m_fY = t_eNBConfigure.fY;
	m_fAbsX = t_eNBConfigure.fAbsX;
	m_fAbsY = t_eNBConfigure.fAbsY;
	printf("»ùÕ¾£º");
	printf("m_fAbsX=%f,m_fAbsY=%f\n", m_fAbsX, m_fAbsY);
}