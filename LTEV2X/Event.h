#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<random>
#include"Global.h"
#include"Enumeration.h"


class Event {
	/*------------------静态------------------*/
public:
	/*
	* 用于自动分配EventId的静态计数
	*/
	static int s_EventCount;

	/*
	* 用于生成随机退避时间的随机数种子，该类共享
	*/
	static std::default_random_engine s_Engine;
	/*------------------域------------------*/
private:
	/*
	* 每个事件都有唯一的Id，Id从0开始编号
	*/
	const int m_EventId;

	/*
	* 该事件对应的用户的Id
	*/
	const int m_VeUEId;

	/*
	* 事件触发的TTI时刻
	*/
	const int m_TriggerTTI;
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

	/*
	* 回退窗初始大小
	*/
	const int m_InitialWindowSize;

	/*
	* 回退窗最大初始大小
	*/
	const int m_MaxWindowSize;

	/*
	* 回退窗当前大小
	* 初始化为初始窗大小，调用一次退避后变为两倍或者最大窗大小
	*/
	int m_CurWindowSize;

	/*
	* 还需退避多少TTI
	*/
	int m_WithdrawalTime;

	/*
	* 记录该事件的所有日志
	*/
	std::list<std::string> m_LogTrackList;

	/*
	* 冲突次数
	*/
	int m_ConflictNum;

	/*
	* 发送时延
	* 发送节点在传输链路上开始发送第一个比特至发送完该组最后一个比特所需要的时间
	*/
	int m_SendDelay;

	/*
	* 排队时延
	* 传输或处理前等待的时间
	*/
	int m_QueueDelay;

	/*
	* 处理时延
	* 从一个节点的输入端到达该节点的输出端所经历的时延
	* <<我好像不明白这个含义>>
	* <<暂时理解为，在信道上占用的时间段>>
	*/
	int m_ProcessDelay;

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

	/*
	* 判断是否完成事件的传输
	*/
	bool isFinished() { return m_IsFinished; }

	/*
	* 返回事件Id
	*/
	int getEventId() { return m_EventId; }

	/*
	* 返回车辆Id
	*/
	int getVeUEId() { return m_VeUEId; }

	/*
	* 返回事件触发时间
	*/
	int getTriggerTTI() { return m_TriggerTTI; }

	/*
	* 返回冲突次数
	*/
	int getConflictNum() { return m_ConflictNum; }

	/*
	* 返回传输时延
	*/
	int getSendDelay() { return m_SendDelay; }

	/*
	* 递增传输时延
	*/
	void increaseSendDelay() { ++m_SendDelay; ++m_ProcessDelay; }

	/*
	* 返回等待时延
	*/
	int getQueueDelay() { return m_QueueDelay; }

	/*
	* 递增等待时延
	*/
	void increaseQueueDelay() { ++m_QueueDelay;  ++m_ProcessDelay; }

	/*
	* 返回处理时延
	*/
	int getProcessDelay() { return m_ProcessDelay; }

	/*
	* 重新发送该消息时，重置信息各个状态
	*/
	void reset();

	/*
	* 尝试接入，并更新状态
	*/
	bool tryAcccess();

	/*
	* 更新信息状态
	* transimitMaxBitNum为本次该时频资源可传输的最大bit数
	* 但本次传输的实际bit数可以小于该值，并返回实际传输的bit数量
	*/
	int transimit(int t_TransimitMaxBitNum);

	/*
	* 冲突之后更新信息状态
	*/
	void conflict();

	/*
	* 记录当前传输的包发生丢包
	*/
	void packetLoss() { m_PackageIsLoss[m_CurrentPackageIdx] = true; }

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





