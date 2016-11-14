#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include"Global.h"
#include"Enumeration.h"

class Message {
	/*------------------��------------------*/
private:
	const MessageType m_MessageType;//����Ϣ������
	const int m_PackageNum;//��Ϣ�����ݰ�����
	const std::vector<int> m_BitNumPerPackage;//ÿ������bit����
	bool m_IsFinished;//������а��Ƿ������
	int m_CurrentPackageIdx;//��Ǽ���Ҫ�����bit���ڵİ����
	int m_RemainBitNum;//currentPackageIdx��ָ���packageʣ��������bit��
	std::vector<bool> m_PackageIsLoss;//��¼ÿ�����ݰ��Ƿ񶪰�
	
	/*------------------����------------------*/
public:/*---�ӿ�---*/
	Message() = delete;
	Message(MessageType t_MessageType);

	std::string toString();
	void reset();//���·��͸���Ϣʱ��������Ϣ����״̬

	/*
	* ������Ϣ״̬
	* transimitMaxBitNumΪ���θ�ʱƵ��Դ�ɴ�������bit��
	* �����δ����ʵ��bit������С�ڸ�ֵ��������ʵ�ʴ����bit����
	*/
	int transimit(int transimitMaxBitNum);

	/*
	* ��¼��ǰ����İ���������
	*/
	void packetLoss() { m_PackageIsLoss[m_CurrentPackageIdx] = true; }

	/*
	* �ж��Ƿ�����¼��Ĵ��䣬�������¼�״̬
	*/
	bool isFinished();

	/*
	* ������Ϣ����
	*/
	MessageType getMessageType() { return m_MessageType; }

	/*
	* ����ʣ��bit����
	*/
	int getRemainBitNum() { return m_RemainBitNum; }

	/*
	* ���ص�ǰ�����
	*/
	int getCurrentPackageIdx() { return m_CurrentPackageIdx; }

	/*
	* ���ض�������
	*/
	int getPacketLossCnt() { 
		return [=] {
			int res = 0;
			for (bool isLoss : m_PackageIsLoss)
				res += isLoss ? 1 : 0;
			return res;
		}();
	}
private:/*---ʵ��---*/
	/*
	* ���캯����ʼ���б���õĺ��������ڳ�ʼ��const��Ա
	*/
	std::pair<int, std::vector<int>> constMemberInitialize(MessageType t_MessageType);
};

class Event {
	/*------------------��̬------------------*/
public:
	static int s_EventCount;

	/*------------------��------------------*/
public:
	const int eventId = s_EventCount++;//ÿ���¼�����Ψһ��Id��Id��0��ʼ���
	Message message;//��Ϣ����
	int VeUEId;//���¼���Ӧ���û���Id
	int TTI;//�¼�������TTIʱ��
	bool isSuccessded;//�Ƿ���ɹ�
	int conflictNum;//��ͻ����
	/*
	* ����ʱ��
	* ��һ�����شӷ��ͽڵ㵽���սڵ��ڴ�����·�Ͼ�����ʱ��
	* <<��������ģ������Ƿ�����ʵ��õ�ʱ�ӣ�)>>
	*/
	int propagationDelay;

	/*
	* ����ʱ��
	* ���ͽڵ��ڴ�����·�Ͽ�ʼ���͵�һ��������������������һ����������Ҫ��ʱ��
	*/
	int sendDelay;

	/*
	* ����ʱ��
	* ��һ���ڵ������˵���ýڵ���������������ʱ��
	* <<�Һ��������������>>
	* <<��ʱ���Ϊ�����ŵ���ռ�õ�ʱ���>>
	*/
	int processingDelay;


	/*
	* �Ŷ�ʱ��
	* �������ǰ�ȴ���ʱ��
	*/
	int queuingDelay;
private:
	std::list<std::string> logTrackList;//��¼���¼���������־

    /*------------------����------------------*/
public:
	/*---���캯��---*/
	Event() = delete;
	Event(int t_VeUEId, int t_TTI, MessageType t_MessageType);

	/*---���ܺ���---*/
	std::string toString();//���string���͵��¼���Ϣ
	std::string toLogString(int n);
	void addEventLog(int t_TTI, EventLogType t_EventLogType, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);//ѹ���µ���־
};





