#pragma once
#include<list>

enum eEventType {//事件类型
	EVENT_CALL_SETUP,
	EVENT_CALL_END,
	EVENT_CAC
};

struct sEvent {//消息类
	/*数据成员*/
	int m_UserId;//用户ID
	int m_ResponseTime;//消息响应的时间

	sEvent() {};
	sEvent(int id, int time) { m_UserId = id; m_ResponseTime = time; };
	bool operator ==(const sEvent& other) { return m_UserId == other.m_UserId&&m_ResponseTime == other.m_ResponseTime; }
};

class EventList :public std::list<sEvent> {//消息事件链表
public:
	void Insert(const sEvent&m) { push_back(m); }
};

