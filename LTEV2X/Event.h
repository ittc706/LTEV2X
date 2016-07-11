#pragma once
#include<list>
#include"Message.h"

struct sEvent {//消息类
	/*数据成员*/
	int VeUEId;//用户ID
	int callSetupTTI;//呼叫发起时刻
	sMessage message;

	sEvent() {};
	sEvent(int id, int time, eMessageType messageType) { 
		VeUEId = id;
		callSetupTTI = time; 
		message = sMessage(messageType);
	};
};



