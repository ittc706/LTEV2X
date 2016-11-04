#pragma once
#include<vector>
#include"TMC.h"

//<TMC_B>: Traffic Model and Control Base

class TMC_B :public TMC_Basic {
public:
	TMC_B() = delete;
	TMC_B(int &t_TTI, 
		Configure& t_Config, 
		RSU* t_RSUAry, 
		VeUE* t_VeUEAry, 
		std::vector<Event>& t_EventVec, 
		std::vector<std::list<int>>& t_EventTTIList, 
		std::vector<std::vector<int>>& t_TTIRSUThroughput);

	/*------------------���ݳ�Ա------------------*/

	std::vector<int> m_VeUEEmergencyNum;//ÿ���������¼������Ĵ�����������֤���ɷֲ��������в����ô�
	std::vector<int> m_VeUEDataNum;//ÿ��������ҵ���¼������Ĵ�����������֤���ɷֲ��������в����ô�
	std::vector<int> m_TransimitSucceedEventNumPerEventType;//ÿ���¼��ɹ��������Ŀ������±�Ϊ�¼�����

	/*------------------��Ա����------------------*/
	void initialize()override;//��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	void buildEventList(std::ofstream& out) override;
	void processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog) override;
private:
	void writeEventListInfo(std::ofstream &out);//д���¼��б����Ϣ
	void writeEventLogInfo(std::ofstream &out);//д�����¼�����־��Ϣ
};