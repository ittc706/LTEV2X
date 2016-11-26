/*
* =====================================================================================
*
*       Filename:  TMC.cpp
*
*    Description:  TMCÄ£¿é
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include"TMC.h"
#include"Function.h"
#include"System.h"

using namespace std;

const std::vector<int> TMC::s_MESSAGE_PACKAGE_NUM = { 4,4,4 };

const std::vector<std::vector<int>> TMC::s_MESSAGE_BIT_NUM_PER_PACKAGE{
	{ 1520,1520,1520,2400 },
	{ 1520,1520,1520,2400 },
	{ 1520,1520,1520,2400 },
};

const std::vector<int> TMC::s_INITIAL_WINDOW_SIZE = { 5,5,5 };

const std::vector<int> TMC::s_MAX_WINDOW_SIZE = { 20,20,20 };

TMC::~TMC() {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++)
		Delete::safeDelete(m_VeUEAry[VeUEId]);
	Delete::safeDelete(m_VeUEAry, true);

	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++)
		Delete::safeDelete(m_RSUAry[RSUId]);
	Delete::safeDelete(m_RSUAry, true);
}