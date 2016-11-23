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
#include"System.h"

using namespace std;

WT::WT(System* t_Context) : m_Context(t_Context) {
	m_SINRMode = m_Context->m_Config._WTMode;
}

WT::~WT() {
	if (m_VeUEAry != nullptr) {
		for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++)
			Delete::safeDelete(m_VeUEAry[VeUEId]);
		Delete::safeDelete(m_VeUEAry, true);
	}

	if (m_RSUAry != nullptr) {
		for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++)
			Delete::safeDelete(m_RSUAry[RSUId]);
		Delete::safeDelete(m_RSUAry, true);
	}
}