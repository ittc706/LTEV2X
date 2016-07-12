#pragma once
#include<list>
#include<string>
#include"Message.h"

struct sEvent {//事件类
	static int count;
	/*数据成员*/
	const int eventId = count++;//事件ID
	int VeUEId;//用户ID
	int ATTI;//事件触发绝对TTI
	int RTTI;//事件触发相对TTI
	sMessage message;

	sEvent(){}

	sEvent(int VeUEId, int ATTI, int RTTI,eMessageType messageType) { 
		this->VeUEId = VeUEId;
		this->ATTI = ATTI; 
		this->RTTI = RTTI;
		message = sMessage(messageType);
	};

	std::string toString();
};



