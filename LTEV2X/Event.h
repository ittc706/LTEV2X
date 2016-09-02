#pragma once
#include<list>
#include<string>
#include"Global.h"
#include"Enumeration.h"

struct sMessage {//消息类
	/*数据成员*/
	eMessageType messageType;//该消息的类型
	int bitNum; //该消息的比特数量
	int remainBitNum;//剩余待传输的bit数

	/*构造函数*/
	sMessage() = delete;
	sMessage(eMessageType messageType);

	/*功能函数*/
	std::string toString();
	void resetRemainBitNum() { remainBitNum = bitNum; }
};

struct sEvent {//事件类
public:
	static int s_EventCount;

	/*数据成员*/
	const int eventId = s_EventCount++;//每个事件都有唯一的Id，Id从0开始编号
	int VeUEId;//该事件对应的用户的Id
	int TTI;//事件触发的TTI时刻
	bool isSuccessded;//是否传输成功
	/*
	* 传播时延
	* 第一个比特从发送节点到接收节点在传输链路上经历的时间
	* <<计算出来的？（还是仿真真实测得的时延？)>>
	*/
	int propagationDelay;

	/*
	* 发送时延
	* 发送节点在传输链路上开始发送第一个比特至发送完该组最后一个比特所需要的时间
	*/
	int sendDelay;

	/*
	* 处理时延
	* 从一个节点的输入端到达该节点的输出端所经历的时延
	* <<我好像不明白这个含义>>
	* <<暂时理解为，在信道上占用的时间段>>
	*/
	int processingDelay;


	/*
	* 排队时延
	* 传输或处理前等待的时间
	*/
	int queuingDelay;


	int conflictNum;//冲突次数


private:
	std::list<std::string> logTrackList;//记录该事件的所有日志
public:
	sMessage message;

	/*构造函数*/
	sEvent() = delete;
	sEvent(int VeUEId, int TTI, eMessageType messageType);

	/*功能函数*/
	std::string toString();//输出string类型的事件消息
	std::string toLogString(int n);
	void addEventLog(int TTI, eEventLogType type, int RSUId, int clusterIdx, int patternIdx);//压入新的日志
};





