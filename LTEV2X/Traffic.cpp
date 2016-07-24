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

	srand((unsigned)time(NULL));//iomanip
								//根据startTTIVec依次填充PERIOD事件并在其中插入服从泊松分布的紧急事件
	int countEmergency = 0;
	int CTTI = 0;
	while (CTTI < m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			list<int>lst = startTTIVec[TTIOffset];
			for (int VeUEId : lst) {
				//----------------WRONG--------------------
				//下面算的p好像不是触发概率，而是单位事件内触发的次数
				//产生服从泊松分布的紧急事件
				double p = m_Config.emergencyLamda / m_NTTI;//触发的概率
				double r = static_cast<double>(rand()) / RAND_MAX;//产生[0-1)分布的随机数
				if (r < p&&CTTI + TTIOffset < m_NTTI) {//若随机数小于概率p，即认为该事件触发
					sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
					countEmergency++;
				}
				//----------------WRONG--------------------

				//产生周期性事件
				if (CTTI + TTIOffset < m_NTTI) {
					/*-----------------------ATTENTION-----------------------
					* 这里先生成sEvent的对象，然后将其压入m_EventVec
					* 由于Vector<T>.push_back是压入传入对象的复制品，因此会调用sEvent的拷贝构造函数
					* sEvent默认的拷贝构造函数会赋值id成员（因此是安全的）
					*sEvent如果自定义拷贝构造函数，必须在构造函数的初始化部分拷贝id成员
					*-----------------------ATTENTION-----------------------*/
					sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, PERIOD);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
				}
			}
		}
		CTTI += m_Config.periodicEventNTTI;
	}

	cout << "countEmergency: " << countEmergency << endl;
}


void cSystem::delayStatistics() {
	//先输出等待时延
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++)
		if (m_EventVec[eventId].isSuccessded)
			g_OutDelayStatistics << m_EventVec[eventId].queuingDelay << " ";
	g_OutDelayStatistics << endl;

	//然后输出传输时延
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++)
		if (m_EventVec[eventId].isSuccessded)
			g_OutDelayStatistics << m_EventVec[eventId].sendDelay << " ";
	g_OutDelayStatistics << endl;
}