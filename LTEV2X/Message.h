#pragma once
#include<math.h>
#include"Global.h"
#include"Enumeration.h"

struct sMessage {
	eMessageType messageType;
	int byteNum; //该消息的比特数量
	int DRA_ONTI;  //在DRA方式下，传输该消息需要占用多少个DRA_MTI

	void setMessageType(eMessageType messageType) {
		this->messageType = messageType;
		switch (messageType) {
		case PERIOD:
			byteNum = 200;
			break;
		case EMERGENCY:
			byteNum = 100;
			break;
		case DATA:
			byteNum = 1000;
			break;
		}
		DRA_ONTI = std::ceil(static_cast<double>(byteNum) / static_cast<double>(gc_ByteNumPerRB));
	}
};