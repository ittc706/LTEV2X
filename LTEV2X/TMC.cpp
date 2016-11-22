#include"TMC.h"
#include"Function.h"

TMC_Basic::~TMC_Basic() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++)
		Delete::safeDelete(m_VeUEAry[VeUEId]);
	Delete::safeDelete(m_VeUEAry, true);
}