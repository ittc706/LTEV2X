#pragma once
#include<vector>
#include<list>
#include"RRM.h"
#include"Exception.h"
#include"Global.h"
//RRM_DRA:Radio Resource Management Round-Robin

class RRM_RR :public RRM_Basic {
public:
	RRM_RR() = delete;
	RRM_RR(int &systemTTI, Configure& systemConfig, RSU* systemRSUAry, VeUE* systemVeUEAry, std::vector<Event>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList, std::vector<std::vector<int>>& systemTTIRSUThroughput);

	/*------------------���ݳ�Ա------------------*/
	std::list<int> m_RRSwitchEventIdList;//���ڴ�Ž���RSU�л��ĳ�������ʱ���������

	int m_NewCount = 0;//��¼��̬�����Ķ���Ĵ���

	int m_DeleteCount = 0;//��¼ɾ����̬��������Ĵ���

	/*------------------��Ա����------------------*/
	void initialize() override;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	void cleanWhenLocationUpdate()override;//������λ�ø���ʱ���������ĵ��������Ϣ
	void schedule() override;//DRA�����ܿأ����ǻ�����麯��

	void RRInformationClean();//��Դ������Ϣ���

	void RRUpdateAdmitEventIdList(bool clusterFlag);//���½����
	void RRProcessEventList();
	void RRProcessWaitEventIdListWhenLocationUpdate();
	void RRProcessSwitchListWhenLocationUpdate();
	void RRProcessWaitEventIdList();

	void RRRoundRobin();//��ѯ���ȣ����䵱ǰTTI����Դ(���Ǹ���ScheduleTable)
	void RRTransimitPreparation();//ͳ�Ƹ�����Ϣ
	void RRTransimitStart();//ģ�⴫�俪ʼ�����µ�����Ϣ���ۼ�������
	void RRWriteScheduleInfo(std::ofstream& out);//��¼������Ϣ��־
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx);//��ʱ��Ϊ��λ��¼��־
	void RRDelaystatistics();//ʱ��ͳ��

	void RRTransimitEnd();//ģ�⴫���������ͳ��������(���Ǹ���ScheduleTable)
};



