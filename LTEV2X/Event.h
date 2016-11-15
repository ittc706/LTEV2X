#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include"Global.h"
#include"Enumeration.h"

class Message {
	/*------------------域------------------*/
private:
	/*
	* 该消息的类型
	*/
	const MessageType m_MessageType;

	/*
	* 消息的数据包总数
	*/
	const int m_PackageNum;

	/*
	* 每个包的bit数量
	*/
	const std::vector<int> m_BitNumPerPackage;

	/*
	* 标记所有包是否传输完毕
	*/
	bool m_IsFinished;

	/*
	* 标记即将要传输的bit所在的包序号
	*/
	int m_CurrentPackageIdx;

	/*
	* currentPackageIdx所指向的package剩余待传输的bit数
	*/
	int m_RemainBitNum;

	/*
	* 记录每个数据包是否丢包
	* 一个包只能被丢一次，因此用布尔值来标记，而不是用丢包次数来标记
	*/
	std::vector<bool> m_PackageIsLoss;

	/*------------------方法------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	Message() = delete;

	/*
	* 接受消息类型为参数的构造函数
	* 禁止隐式转换的发生
	*/
	explicit Message(MessageType t_MessageType);

	/*
	* 生成格式化字符串
	*/
	std::string toString();

	/*
	* 重新发送该消息时，重置信息各个状态
	*/
	void reset();

	/*
	* 更新信息状态
	* transimitMaxBitNum为本次该时频资源可传输的最大bit数
	* 但本次传输的实际bit数可以小于该值，并返回实际传输的bit数量
	*/
	int transimit(int t_TransimitMaxBitNum);

	/*
	* 记录当前传输的包发生丢包
	*/
	void packetLoss() { m_PackageIsLoss[m_CurrentPackageIdx] = true; }

	/*
	* 判断是否完成事件的传输
	*/
	bool isFinished() { return m_IsFinished; }

	/*
	* 返回消息类型
	*/
	MessageType getMessageType() { return m_MessageType; }

	/*
	* 返回剩余bit数量
	*/
	int getRemainBitNum() { return m_RemainBitNum; }

	/*
	* 返回当前包编号
	*/
	int getCurrentPackageIdx() { return m_CurrentPackageIdx; }

	/*
	* 返回丢包总数
	*/
	int getPacketLossCnt() {
		return [=] {
			int res = 0;
			for (bool isLoss : m_PackageIsLoss)
				res += isLoss ? 1 : 0;
			return res;
		}();
	}
private:/*---实现---*/
		/*
		* 构造函数初始化列表调用的函数，用于初始化const成员
		*/
	std::pair<int, std::vector<int>> constMemberInitialize(MessageType t_MessageType);
};

class Event {
	/*------------------静态------------------*/
public:
	/*
	* 用于自动分配EventId的静态计数
	*/
	static int s_EventCount;

	/*------------------域------------------*/
public:
	/*
	* 每个事件都有唯一的Id，Id从0开始编号
	*/
	const int eventId = s_EventCount++;

	/*
	* 消息对象
	*/
	Message message;

	/*
	* 该事件对应的用户的Id
	*/
	int VeUEId;

	/*
	* 事件触发的TTI时刻
	*/
	int TTI;

	/*
	* 是否传输成功
	*/
	bool isSuccessded;

	/*
	* 冲突次数
	*/
	int conflictNum;

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
private:
	/*
	* 记录该事件的所有日志
	*/
	std::list<std::string> logTrackList;

	/*------------------方法------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	Event() = delete;

	/*
	* 构造函数，接受参数分别为
	*	车辆Id，事件触发事件，消息类型
	*/
	Event(int t_VeUEId, int t_TTI, MessageType t_MessageType);

	/*
	* 生成格式化的字符串
	*/
	std::string toString();

	/*
	* 生成用于日志输出的格式化的字符串
	*/
	std::string toLogString(int t_NumTab);

	/*
	* 添加日志
	*/
	void addEventLog(int t_TTI, EventLogType t_EventLogType, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);//压入新的日志
};





