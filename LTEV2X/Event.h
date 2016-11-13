#pragma once
#include<vector>
#include<list>
#include<string>
#include"Global.h"
#include"Enumeration.h"

struct Message {//��Ϣ��
	/*���ݳ�Ա*/
	MessageType messageType;//����Ϣ������
	int packageNum;//��Ϣ�����ݰ�����
	std::vector<int> bitNumPerPackage;

private:
	bool isDone;
	int currentPackageIdx;
	int remainBitNum;//currentPackageIdx��ָ���packageʣ��������bit��
	int packetLossCnt;

public:
	/*���캯��*/
	Message() = delete;
	Message(MessageType messageType);

	/*���ܺ���*/
	std::string toString();
	void reset();//���·��͸���Ϣʱ��������Ϣ����״̬

	/*
	* ������Ϣ״̬
	* transimitMaxBitNumΪ���θ�ʱƵ��Դ�ɴ�������bit��
	* �����δ����ʵ��bit������С�ڸ�ֵ��������ʵ�ʴ����bit����
	*/
	int transimit(int transimitMaxBitNum);
	bool isFinished();//�ж��Ƿ�����¼��Ĵ��䣬�������¼�״̬
	int getRemainBitNum() { return remainBitNum; }
	int getCurrentPackageIdx() { return currentPackageIdx; }
	void packetLoss() { ++packetLossCnt; }
	int getPacketLossCnt() { return packetLossCnt; }
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





