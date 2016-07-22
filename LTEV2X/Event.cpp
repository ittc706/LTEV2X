#include"Event.h"
#include"Enumeration.h"
#include"Global.h"

using namespace std;


int sEvent::s_EventCount = 0;



sMessage::sMessage(eMessageType messageType) {
	this->messageType = messageType;
	switch (messageType) {
	case PERIOD:
		bitNum = gc_PeriodMessageBitNum;
		break;
	case EMERGENCY:
		bitNum = gc_EmergencyMessageBitNum;
		break;
	case DATA:
		bitNum = gc_DataMessageBitNum;
		break;
	}
}



sEvent::sEvent(int VeUEId, int TTI, eMessageType messageType) :propagationDelay(0), sendDelay(0), processingDelay(0), queuingDelay(0) {
	this->VeUEId = VeUEId;
	this->TTI = TTI;
	message = sMessage(messageType);
};

