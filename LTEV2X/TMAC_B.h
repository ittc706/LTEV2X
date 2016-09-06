#pragma once
#include<vector>
#include"TMAC.h"

class TMAC_B :public TMAC_Basic {
public:
	TMAC_B() = delete;
	TMAC_B(int &systemTTI, 
		sConfigure& systemConfig, 
		cRSU* systemRSUAry, 
		cVeUE* systemVeUEAry, 
		std::vector<sEvent>& systemEventVec, 
		std::vector<std::list<int>>& systemEventTTIList, 
		std::vector<std::vector<int>>& systemTTIRSUThroughput);

	/*------------------���ݳ�Ա------------------*/

	std::vector<int> m_VeUEEmergencyNum;//ÿ���������¼������Ĵ�����������֤���ɷֲ��������в����ô�
	std::vector<int> m_VeUEDataNum;//ÿ��������ҵ���¼������Ĵ�����������֤���ɷֲ��������в����ô�

	/*------------------��Ա����------------------*/

	void buildEventList(std::ofstream& out) override;
	void processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog) override;
private:
	void writeEventListInfo(std::ofstream &out);//д���¼��б�����Ϣ
	void writeEventLogInfo(std::ofstream &out);//д�����¼�����־��Ϣ
};