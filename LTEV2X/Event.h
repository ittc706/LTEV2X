#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include"Global.h"
#include"Enumeration.h"

struct Message {//��Ϣ��
	/*ʵ��*/
private:
	const MessageType messageType;//����Ϣ������
	const int packageNum;//��Ϣ�����ݰ�����
	const std::vector<int> bitNumPerPackage;


	bool isDone;
	int currentPackageIdx;
	int remainBitNum;//currentPackageIdx��ָ���packageʣ��������bit��
	std::vector<bool> packageIsLoss;//��¼ÿ�����ݰ��Ƿ񶪰�

	std::pair<int, std::vector<int>> constMemberInitialize(MessageType t_MessageType);
public:
	/*�ӿ�*/
	Message() = delete;
	Message(MessageType t_MessageType);

	/*���ܺ���*/
	std::string toString();
	void reset();//���·��͸���Ϣʱ��������Ϣ����״̬

	/*
	* ������Ϣ״̬
	* transimitMaxBitNumΪ���θ�ʱƵ��Դ�ɴ�������bit��
	* �����δ����ʵ��bit������С�ڸ�ֵ��������ʵ�ʴ����bit����
	*/
	int transimit(int transimitMaxBitNum);
	void packetLoss() { packageIsLoss[currentPackageIdx] = true; }
	bool isFinished();//�ж��Ƿ�����¼��Ĵ��䣬�������¼�״̬
	MessageType getMessageType() { return messageType; }
	int getRemainBitNum() { return remainBitNum; }
	int getCurrentPackageIdx() { return currentPackageIdx; }
	int getPacketLossCnt() { 
		return [=] {
			int res = 0;
			for (bool isLoss : packageIsLoss)
				res += isLoss ? 1 : 0;
			return res;
		}();
	}
};

struct Event {//�¼���
public:
	static int s_EventCount;

	/*���ݳ�Ա*/
	const int eventId = s_EventCount++;//ÿ���¼�����Ψһ��Id��Id��0��ʼ���
	int VeUEId;//���¼���Ӧ���û���Id
	int TTI;//�¼�������TTIʱ��
	bool isSuccessded;//�Ƿ���ɹ�
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


	int conflictNum;//��ͻ����


private:
	std::list<std::string> logTrackList;//��¼���¼���������־
public:
	Message message;

	/*���캯��*/
	Event() = delete;
	Event(int t_VeUEId, int t_TTI, MessageType t_MessageType);

	/*���ܺ���*/
	std::string toString();//���string���͵��¼���Ϣ
	std::string toLogString(int n);
	void addEventLog(int t_TTI, EventLogType t_EventLogType, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description);//ѹ���µ���־
};





