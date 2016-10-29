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
	void schedule() override;//DRA�����ܿأ����ǻ�����麯��

	void RRInformationClean();//��Դ������Ϣ���

	void RRUpdateAdmitEventIdList(bool clusterFlag);//���½����
	void RRProcessEventList();
	void RRProcessWaitEventIdListWhenLocationUpdate();
	void RRProcessSwitchListWhenLocationUpdate();
	void RRProcessWaitEventIdList();

	void RRTransimitBegin();//��ʼ����(���Ǹ���ScheduleTable)

	void RRWriteScheduleInfo(std::ofstream& out);//��¼������Ϣ��־
	void RRWriteTTILogInfo(std::ofstream& out, int TTI, int type, int eventId, int RSUId, int patternIdx);//��ʱ��Ϊ��λ��¼��־
	void RRDelaystatistics();//ʱ��ͳ��

	void RRTransimitEnd();//�������(���Ǹ���ScheduleTable)
};



