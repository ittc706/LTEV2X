/*
* =====================================================================================
*
*       Filename:  GTT.cpp
*
*    Description:  TMCÄ£¿é
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

#include"GTT.h"
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


GTT::~GTT(){
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++)
		Delete::safeDelete(m_VeUEAry[VeUEId]);
	Delete::safeDelete(m_VeUEAry, true);
}
