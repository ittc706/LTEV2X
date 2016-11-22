/*
* =====================================================================================
*
*       Filename:  WT.cpp
*
*    Description:  WTÄ£¿é
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  LN
*            LIB:  ITTC
*
* =====================================================================================
*/

#include"WT.h"
#include"Function.h"

using namespace std;

WT::~WT() {
	if (m_VeUEAry != nullptr) {
		for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++)
			Delete::safeDelete(m_VeUEAry[VeUEId]);
		Delete::safeDelete(m_VeUEAry, true);
	}
}