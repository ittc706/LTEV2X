#include<iomanip>
#include"Traffic.h"
#include"Enumeration.h"
#include"Global.h"
#include"System.h"

using namespace std;


int sEvent::s_EventCount = 0;



sMessage::sMessage(eMessageType messageType) {
	this->messageType = messageType;
	switch (messageType) {
	case PERIOD:
		bitNum = gc_PeriodMessageBitNum;
		break;
	case EMERGENCY:
		bitNum = gc_EmergencyMessageBitNum;
		break;
	case DATA:
		bitNum = gc_DataMessageBitNum;
		break;
	}
}



sEvent::sEvent(int VeUEId, int TTI, eMessageType messageType) :isSuccessded(false), propagationDelay(0), sendDelay(0), processingDelay(0), queuingDelay(0) {
	this->VeUEId = VeUEId;
	this->TTI = TTI;
	message = sMessage(messageType);
}



void cSystem::buildEventList() {
	/*按时间顺序（事件的Id与时间相关，Id越小，事件发生的时间越小生成事件链表*/

	srand((unsigned)time(NULL));//iomanip
	//首先生成各个车辆的周期性事件的起始时刻
	vector<list<int>> startTTIVec(m_Config.periodicEventNTTI, list<int>());
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		int startTTI = rand() % m_Config.periodicEventNTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	//生成紧急事件的发生时刻，每个时间槽存放该时刻发生紧急事件的车辆
	m_VeUEEmergencyNum = vector<int>(m_Config.VeUENum,0);//初始化统计量
	int countEmergency = 0;
	vector<list<int>> emergencyEventTriggerTTI(m_NTTI);
	for (int VeUEId = 0;VeUEId < m_Config.VeUENum;VeUEId++) {
		//依次生成每个车辆的紧急事件到达时刻
		double T = 0;
		while (T < m_NTTI) {
			double u = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
			if (u == 0) break;
			T = T - (1 / m_Config.emergencyLamda)*log(u);
			int IntegerT = static_cast<int>(T);
			if (IntegerT < m_NTTI) {
				emergencyEventTriggerTTI[IntegerT].push_back(VeUEId);
				++m_VeUEEmergencyNum[VeUEId];
				++countEmergency;
			}
		}
	}

	//将m_VeUEEmergencyNum写入文件用于分析
	cout << m_VeUEEmergencyNum.size() << endl;
	for (int num : m_VeUEEmergencyNum)
		g_OutEmergencyPossion << num << " ";
	g_OutEmergencyPossion << endl;//这里很关键，将缓存区的数据刷新到流中
	cout << "countEmergency: " << countEmergency << endl;


	//根据startTTIVec依次填充PERIOD事件并在其中插入服从泊松分布的紧急事件

	int CTTI = 0;
	while (CTTI < m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			//压入紧急事件
			if (CTTI + TTIOffset < m_NTTI) {
				list<int> &emergencyList = emergencyEventTriggerTTI[CTTI + TTIOffset];
				for (int VeUEId : emergencyList) {
					/*-----------------------ATTENTION-----------------------
					* 这里先生成sEvent的对象，然后将其压入m_EventVec
					* 由于Vector<T>.push_back是压入传入对象的复制品，因此会调用sEvent的拷贝构造函数
					* sEvent默认的拷贝构造函数会赋值id成员（因此是安全的）
					*sEvent如果自定义拷贝构造函数，必须在构造函数的初始化部分拷贝id成员
					*-----------------------ATTENTION-----------------------*/
					sEvent evt= sEvent(VeUEId, CTTI + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
					--countEmergency;
				}
			}

			//产生周期性事件
			if (CTTI + TTIOffset < m_NTTI) {
				list<int> &periodList = startTTIVec[TTIOffset];
				for (int VeUEId : periodList) {
					sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, PERIOD);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
				}
			}
		}
		CTTI += m_Config.periodicEventNTTI;
	}
}


void cSystem::delayStatistics() {
	//先输出等待时延
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++)
		if (m_EventVec[eventId].isSuccessded)
			g_OutDelayStatistics << m_EventVec[eventId].queuingDelay << " ";
	g_OutDelayStatistics << endl;//这里很关键，将缓存区的数据刷新到流中

	//然后输出传输时延
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++)
		if (m_EventVec[eventId].isSuccessded)
			g_OutDelayStatistics << m_EventVec[eventId].sendDelay << " ";
	g_OutDelayStatistics << endl;//这里很关键，将缓存区的数据刷新到流中
}