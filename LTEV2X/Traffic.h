#pragma once
#include<list>
#include<string>
#include"Global.h"
#include"Enumeration.h"

struct sMessage {//��Ϣ��
				 /*���ݳ�Ա*/
	eMessageType messageType;//����Ϣ������
	int bitNum; //����Ϣ�ı�������

				/*���캯��*/
	sMessage() {}
	sMessage(eMessageType messageType);

	/*���ܺ���*/
	std::string toString();
};

struct sEvent {//�¼���
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


private:
	std::list<std::string> logTrackList;//��¼���¼���������־
public:
	sMessage message;

	/*���캯��*/
	sEvent() :isSuccessded(false), propagationDelay(0), sendDelay(0), processingDelay(0), queuingDelay(0) {}
	sEvent(int VeUEId, int TTI, eMessageType messageType);

	/*���ܺ���*/
	std::string toString();//���string���͵��¼���Ϣ
	std::string toLogString(int n);
	void addEventLog(int TTI, int type, int RSUId, int clusterIdx, int patternIdx);//ѹ���µ���־
};





