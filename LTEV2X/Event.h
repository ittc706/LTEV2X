#pragma once
#include<vector>
#include<list>
#include<string>
#include"Global.h"
#include"Enumeration.h"

struct Message {//消息类
	/*数据成员*/
	MessageType messageType;//该消息的类型
	int packageNum;//消息的数据包总数
	std::vector<int> bitNumPerPackage;

private:
	bool isDone;
	int currentPackageIdx;
	int remainBitNum;//currentPackageIdx所指向的package剩余待传输的bit数

public:
	/*构造函数*/
	Message() = delete;
	Message(MessageType messageType);

	/*功能函数*/
	std::string toString();
	void reset();//重新发送该消息时，重置信息各个状态
	/*
	* 更新信息状态
	* transimitMaxBitNum为本次该时频资源可传输的最大bit数
	* 但本次传输的实际bit数可以小于该值，并返回实际传输的bit数量
	*/
	int transimit(int transimitMaxBitNum);
	bool isFinished();//判断是否完成事件的传输，并更新事件状态
	int getRemainBitNum() { return remainBitNum; }
	int getCurrentPackageIdx() { return currentPackageIdx; }
};

struct Event {//事件类
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
	Message message;

	/*构造函数*/
	Event() = delete;
	Event(int t_VeUEId, int t_TTI, MessageType t_MessageType);

	/*功能函数*/
	std::string toString();//输出string类型的事件消息
	std::string toLogString(int n);
	void addEventLog(int t_TTI, EventLogType t_EventLogType, int t_RSUId, int t_ClusterIdx, int t_PatternIdx, std::string t_Description);//压入新的日志
};





