#pragma once
#include<list>
#include<string>
#include"Message.h"

struct sMessage {//消息类
	/*数据成员*/
	eMessageType messageType;//该消息的类型
	int byteNum; //该消息的比特数量
	int DRA_ONTTI;  //在DRA方式下，传输该消息需要占用多少个TTI

	/*构造函数*/
	sMessage() {}
	sMessage(eMessageType messageType);

	/*功能函数*/
	std::string toString();
};

struct sEvent {//事件类
public:
	static int s_EventCount;

	/*数据成员*/
	const int eventId = s_EventCount++;//事件ID
	int VeUEId;//用户ID
	int TTI;//事件触发的TTI时刻

private :
	std::list<std::string> logTrackList;//记录该事件的所有日志
public:
	sMessage message;

	/*构造函数*/
	sEvent(){}
	sEvent(int VeUEId, int TTI, eMessageType messageType);

	/*功能函数*/
	std::string toString();//输出string类型的事件消息
	std::string toLogString(int n);
	void addLog(std::string s);//压入新的日志
};





