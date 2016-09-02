#pragma once
#include<vector>
#include"BMAD.h"

class BMAD_B :public BMAD_Basic {
public:
	BMAD_B() = delete;
	BMAD_B(int &systemTTI, sConfigure& systemConfig, cRSU* systemRSUAry, cVeUE* systemVeUEAry, std::vector<sEvent>& systemEventVec, std::vector<std::list<int>>& systemEventTTIList) :
		BMAD_Basic(systemTTI, systemConfig, systemRSUAry, systemVeUEAry, systemEventVec, systemEventTTIList) {}

	/*------------------���ݳ�Ա------------------*/

	std::vector<int> m_VeUEEmergencyNum;//ÿ���������¼������Ĵ�����������֤���ɷֲ��������в����ô�

	/*------------------��Ա����------------------*/

	void buildEventList(std::ofstream& out) override;
	void processStatistics(std::ofstream& outDelay, std::ofstream& outPossion, std::ofstream& outConflict, std::ofstream& outEventLog) override;
private:
	void writeEventListInfo(std::ofstream &out);//д���¼��б����Ϣ
	void writeEventLogInfo(std::ofstream &out);//д�����¼�����־��Ϣ
};