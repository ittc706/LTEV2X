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
	/*
	* ����Ϣ������
	*/
	const MessageType m_MessageType;

	/*
	* ��Ϣ�����ݰ�����
	*/
	const int m_PackageNum;

	/*
	* ÿ������bit����
	*/
	const std::vector<int> m_BitNumPerPackage;

	/*
	* ������а��Ƿ������
	*/
	bool m_IsFinished;

	/*
	* ��Ǽ���Ҫ�����bit���ڵİ����
	*/
	int m_CurrentPackageIdx;

	/*
	* currentPackageIdx��ָ���packageʣ��������bit��
	*/
	int m_RemainBitNum;

	/*
	* ��¼ÿ�����ݰ��Ƿ񶪰�
	* һ����ֻ�ܱ���һ�Σ�����ò���ֵ����ǣ��������ö������������
	*/
	std::vector<bool> m_PackageIsLoss;

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	Message() = delete;

	/*
	* ������Ϣ����Ϊ�����Ĺ��캯��
	* ��ֹ��ʽת���ķ���
	*/
	explicit Message(MessageType t_MessageType);

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString();

	/*
	* ���·��͸���Ϣʱ��������Ϣ����״̬
	*/
	void reset();

	/*
	* ������Ϣ״̬
	* transimitMaxBitNumΪ���θ�ʱƵ��Դ�ɴ�������bit��
	* �����δ����ʵ��bit������С�ڸ�ֵ��������ʵ�ʴ����bit����
	*/
	int transimit(int t_TransimitMaxBitNum);

	/*
	* ��¼��ǰ����İ���������
	*/
	void packetLoss() { m_PackageIsLoss[m_CurrentPackageIdx] = true; }

	/*
	* �ж��Ƿ�����¼��Ĵ���
	*/
	bool isFinished() { return m_IsFinished; }

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
	/*
	* �����Զ�����EventId�ľ�̬����
	*/
	static int s_EventCount;

	/*------------------��------------------*/
public:
	/*
	* ÿ���¼�����Ψһ��Id��Id��0��ʼ���
	*/
	const int eventId = s_EventCount++;

	/*
	* ��Ϣ����
	*/
	Message message;

	/*
	* ���¼���Ӧ���û���Id
	*/
	int VeUEId;

	/*
	* �¼�������TTIʱ��
	*/
	int TTI;

	/*
	* �Ƿ���ɹ�
	*/
	bool isSuccessded;

	/*
	* ��ͻ����
	*/
	int conflictNum;

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
	/*
	* ��¼���¼���������־
	*/
	std::list<std::string> logTrackList;

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	Event() = delete;

	/*
	* ���캯�������ܲ����ֱ�Ϊ
	*	����Id���¼������¼�����Ϣ����
	*/
	Event(int t_VeUEId, int t_TTI, MessageType t_MessageType);

	/*
	* ���ɸ�ʽ�����ַ���
	*/
	std::string toString();

	/*
	* ����������־����ĸ�ʽ�����ַ���
	*/
	std::string toLogString(int t_NumTab);

	/*
	* �����־
	*/
	void addEventLog(int t_TTI, EventLogType t_EventLogType, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);//ѹ���µ���־
};





