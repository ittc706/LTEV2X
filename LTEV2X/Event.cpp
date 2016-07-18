#include"Event.h"
#include"Enumeration.h"
#include"Global.h"

using namespace std;


int sEvent::s_EventCount = 0;



sMessage::sMessage(eMessageType messageType) {
	this->messageType = messageType;
	switch (messageType) {
	case PERIOD:
		byteNum = 300;
		break;
	case EMERGENCY:
		byteNum = 200;
		break;
	case DATA:
		byteNum = 1000;
		break;
	}
	DRA_ONTTIPerFB = static_cast<int>(std::ceil(static_cast<double>(byteNum) / static_cast<double>(gc_ByteNumPerRB)));
}



sEvent::sEvent(int VeUEId, int TTI, eMessageType messageType) :propagationDelay(0), sendDelay(0), processingDelay(0), queuingDelay(0) {
	this->VeUEId = VeUEId;
	this->TTI = TTI;
	message = sMessage(messageType);
};

