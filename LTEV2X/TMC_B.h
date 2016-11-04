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

	/*------------------数据成员------------------*/

	std::vector<int> m_VeUEEmergencyNum;//每辆车紧急事件触发的次数，用于验证泊松分布，仿真中并无用处
	std::vector<int> m_VeUEDataNum;//每辆车数据业务事件触发的次数，用于验证泊松分布，仿真中并无用处
	std::vector<int> m_TransimitSucceedEventNumPerEventType;//每类事件成功传输的数目，外层下标为事件种类

	/*------------------成员函数------------------*/
	void initialize()override;//初始化RSU VeUE内该单元的内部类
	void buildEventList(std::ofstream& out) override;
	void processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog) override;
private:
	void writeEventListInfo(std::ofstream &out);//写入事件列表的信息
	void writeEventLogInfo(std::ofstream &out);//写入以事件的日志信息
};