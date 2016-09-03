/*
* =====================================================================================
*
*       Filename:  TMAC_B.cpp
*
*    Description:  TMAC模块
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<iomanip>
#include<iostream>
#include"TMAC_B.h"

using namespace std;


void TMAC_B::buildEventList(std::ofstream& out) {
	/*按时间顺序（事件的Id与时间相关，Id越小，事件发生的时间越小生成事件链表*/

	srand((unsigned)time(NULL));//iomanip
	//首先生成各个车辆的周期性事件的起始时刻(相对时刻，即[0 , m_Config.periodicEventNTTI)
	vector<list<int>> startTTIVec(m_Config.periodicEventNTTI, list<int>());
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		int startTTI = rand() % m_Config.periodicEventNTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	//生成紧急事件的发生时刻，每个时间槽存放该时刻发生紧急事件的车辆
	m_VeUEEmergencyNum = vector<int>(m_Config.VeUENum, 0);//初始化统计量
	int countEmergency = 0;
	vector<list<int>> emergencyEventTriggerTTI(m_Config.m_NTTI);
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		//依次生成每个车辆的紧急事件到达时刻
		double T = 0;
		while (T < m_Config.m_NTTI) {
			double u = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
			if (u == 0) break;
			T = T - (1 / m_Config.emergencyLamda)*log(u);
			int IntegerT = static_cast<int>(T);
			if (IntegerT < m_Config.m_NTTI) {
				emergencyEventTriggerTTI[IntegerT].push_back(VeUEId);
				++m_VeUEEmergencyNum[VeUEId];
				++countEmergency;
			}
		}
	}
	cout << "countEmergency: " << countEmergency << endl;


	//生成数据业务事件的发生时刻，每个时间槽存放该时刻发生数据业务事件的车辆
	m_VeUEDataNum = vector<int>(m_Config.VeUENum, 0);//初始化统计量
	int countData = 0;
	vector<list<int>> dataEventTriggerTTI(m_Config.m_NTTI);
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		//依次生成每个车辆的紧急事件到达时刻
		double T = 0;
		while (T < m_Config.m_NTTI) {
			double u = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
			if (u == 0) break;
			T = T - (1 / m_Config.dataLamda)*log(u);
			int IntegerT = static_cast<int>(T);
			if (IntegerT < m_Config.m_NTTI) {
				dataEventTriggerTTI[IntegerT].push_back(VeUEId);
				++m_VeUEDataNum[VeUEId];
				++countData;
			}
		}
	}
	cout << "countData: " << countData << endl;


	//根据startTTIVec依次填充PERIOD事件并在其中插入服从泊松分布的紧急事件

	int startTTIOfEachPeriod = 0;//每个周期的起始时刻
	while (startTTIOfEachPeriod < m_Config.m_NTTI) {
		//TTIOffset为相对于startTTIOfEachPeriod的偏移量
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			//压入紧急事件
			if (startTTIOfEachPeriod + TTIOffset < m_Config.m_NTTI) {
				list<int> &emergencyList = emergencyEventTriggerTTI[startTTIOfEachPeriod + TTIOffset];
				for (int VeUEId : emergencyList) {
					/*-----------------------ATTENTION-----------------------
					* 这里先生成sEvent的对象，然后将其压入m_EventVec
					* 由于Vector<T>.push_back是压入传入对象的复制品，因此会调用sEvent的拷贝构造函数
					* sEvent默认的拷贝构造函数会赋值id成员（因此是安全的）
					*sEvent如果自定义拷贝构造函数，必须在构造函数的初始化部分拷贝id成员
					*-----------------------ATTENTION-----------------------*/
					sEvent evt = sEvent(VeUEId, startTTIOfEachPeriod + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.eventId);
					--countEmergency;
				}
			}


			//压入数据业务事件
			if (startTTIOfEachPeriod + TTIOffset < m_Config.m_NTTI) {
				list<int> &dataList = dataEventTriggerTTI[startTTIOfEachPeriod + TTIOffset];
				for (int VeUEId : dataList) {
					/*-----------------------ATTENTION-----------------------
					* 这里先生成sEvent的对象，然后将其压入m_EventVec
					* 由于Vector<T>.push_back是压入传入对象的复制品，因此会调用sEvent的拷贝构造函数
					* sEvent默认的拷贝构造函数会赋值id成员（因此是安全的）
					*sEvent如果自定义拷贝构造函数，必须在构造函数的初始化部分拷贝id成员
					*-----------------------ATTENTION-----------------------*/
					sEvent evt = sEvent(VeUEId, startTTIOfEachPeriod + TTIOffset, DATA);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.eventId);
					--countData;
				}
			}


			//产生周期性事件
			if (startTTIOfEachPeriod + TTIOffset < m_Config.m_NTTI) {
				list<int> &periodList = startTTIVec[TTIOffset];
				for (int VeUEId : periodList) {
					sEvent evt = sEvent(VeUEId, startTTIOfEachPeriod + TTIOffset, PERIOD);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.eventId);
				}
			}
		}
		startTTIOfEachPeriod += m_Config.periodicEventNTTI;
	}
	
	//打印事件链表
	writeEventListInfo(out);
}

void TMAC_B::processStatistics(std::ofstream& outDelay, std::ofstream& outEmergencyPossion, std::ofstream& outDataPossion, std::ofstream& outConflict, std::ofstream& outEventLog) {
	//统计等待时延
	for (int eventId = 0; eventId < static_cast<int>(m_EventVec.size()); eventId++)
		if (m_EventVec[eventId].isSuccessded)
			outDelay << m_EventVec[eventId].queuingDelay << " ";
	outDelay << endl;//这里很关键，将缓存区的数据刷新到流中

	//统计传输时延
	for (int eventId = 0; eventId < static_cast<int>(m_EventVec.size()); eventId++)
		if (m_EventVec[eventId].isSuccessded)
			outDelay << m_EventVec[eventId].sendDelay << " ";
	outDelay << endl;//这里很关键，将缓存区的数据刷新到流中

	//统计紧急事件分布情况
	for (int num : m_VeUEEmergencyNum)
		outEmergencyPossion << num << " ";
	outEmergencyPossion << endl;//这里很关键，将缓存区的数据刷新到流中

	//统计数据业务事件分布情况
	for (int num : m_VeUEDataNum)
		outDataPossion << num << " ";
	outDataPossion << endl;//这里很关键，将缓存区的数据刷新到流中

	//统计冲突情况
	for (sEvent &event : m_EventVec)
		outConflict << event.conflictNum << " ";
	outConflict << endl;
	writeEventLogInfo(outEventLog);
}

void TMAC_B::writeEventListInfo(std::ofstream &out) {
	for (int i = 0; i < m_Config.m_NTTI; i++) {
		out << "[ TTI = " << left << setw(3) << i << " ]" << endl;
		out << "{" << endl;
		for (int eventId : m_EventTTIList[i]) {
			sEvent& e = m_EventVec[eventId];
			out << "    " << e.toString() << endl;
		}
		out << "}\n\n" << endl;
	}
}
void TMAC_B::writeEventLogInfo(std::ofstream &out) {
	for (int eventId = 0; eventId < static_cast<int>(m_EventVec.size()); eventId++) {
		string s;
		switch (m_EventVec[eventId].message.messageType) {
		case PERIOD:
			s = "PERIOD";
			break;
		case EMERGENCY:
			s = "EMERGENCY";
			break;
		case DATA:
			s = "DATA";
			break;
		}
		out << "Event[" << eventId << "]";
		out << "{" << endl;
		out << "    " << "VeUEId = " << m_EventVec[eventId].VeUEId << endl;
		out << "    " << "MessageType = " << s << endl;
		out << "    " << "sendDelay = " << m_EventVec[eventId].sendDelay << "(TTI)" << endl;
		out << "    " << "queuingDelay = " << m_EventVec[eventId].queuingDelay << "(TTI)" << endl;
		out << m_EventVec[eventId].toLogString(1);
		out << "}" << endl;
	}
}


