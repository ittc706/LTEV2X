#include"VUE.h"


bool cVeUE::isScheduled(eLinkType link) {
	if (link == DOWNLINK)
		return this->m_IsScheduledDL;
	else
		return this->m_IsScheduledUL;
}
