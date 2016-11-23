#pragma once
#include<vector>
#include"TMC.h"

//<TMC_B>: Traffic Model and Control Base

class TMC_B :public TMC {
	/*------------------域------------------*/
public:
	/*
	* 每辆车紧急事件触发的次数
	* 用于验证泊松分布，仿真中并无用处
	*/
	std::vector<int> m_VeUEEmergencyNum;

	/*
	* 每辆车数据业务事件触发的次数
	* 用于验证泊松分布，仿真中并无用处
	*/
	std::vector<int> m_VeUEDataNum;

	/*
	* 每类事件成功传输的数目
	* 外层下标为事件种类
	*/
	std::vector<int> m_TransimitSucceedEventNumPerEventType;

	/*------------------方法------------------*/
public:
	/*
	* 默认构造函数定义为删除
	*/
	TMC_B() = delete;

	/*
	* 构造函数
	*/
	TMC_B(System* t_Context);

	/*
	* 初始化RSU VeUE内该单元的内部类
	*/
	void initialize()override;

	/*
	* 生成事件链表
	*/
	void buildEventList(std::ofstream& t_File) override;

	/*
	* 仿真结束后统计各种数据
	*/
	void processStatistics(
		std::ofstream& t_FileStatisticsDescription,
		std::ofstream& t_FileEmergencyDelay, std::ofstream& t_FilePeriodDelay, std::ofstream& t_FileDataDelay,
		std::ofstream& t_FileEmergencyPossion, std::ofstream& t_FileDataPossion,
		std::ofstream& t_FileEmergencyConflict, std::ofstream& t_FilePeriodConflict, std::ofstream& t_FileDataConflict,
		std::ofstream& t_FilePackageLoss, std::ofstream& t_FilePackageTransimit,
		std::ofstream& t_FileEventLog
	) override;

private:
	/*
	* 写入事件列表的信息
	*/
	void writeEventListInfo(std::ofstream &t_File);

	/*
	* 写入以事件的日志信息
	*/
	void writeEventLogInfo(std::ofstream &t_File);
};