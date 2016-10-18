#include<vector>
#include<iomanip>
#include<math.h>
#include"VUE.h"
#include"RSU.h"

using namespace std;

int cVeUE::m_VeUECount = 0;

void cVeUE::initialize(sUEConfigure &t_UEConfigure)
{
	m_wRoadID = t_UEConfigure.wRoadID;
	m_locationID = t_UEConfigure.locationID;
	m_fX = t_UEConfigure.fX;
	m_fY = t_UEConfigure.fY;
	m_fAbsX = t_UEConfigure.fAbsX;
	m_fAbsY = t_UEConfigure.fAbsY;
	//printf("m_fAbsX=%f,m_fAbsY=%f\n",m_fAbsX,m_fAbsY);
	m_fv = t_UEConfigure.fv;
	if ((0<m_locationID) && (m_locationID <= 61))
	{
		m_fvAngle = 90;
	}
	else if ((61<m_locationID) && (m_locationID <= 96))
	{
		m_fvAngle = 0;
	}
	else if ((96<m_locationID) && (m_locationID <= 157))
	{
		m_fvAngle = -90;
	}
	else
		m_fvAngle = -180;
	RandomUniform(&m_fantennaAngle, 1, 180.0f, -180.0f, false);

	m_Nt = 1;
	m_Nr = 2;
	m_Mol = 4;
	m_H = new float[2 * 1024 * 2];

}

cVeUE::~cVeUE() {
	delete m_H;
	delete m_interH;
}
