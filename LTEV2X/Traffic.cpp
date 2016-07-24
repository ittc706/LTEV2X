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
	/*��ʱ��˳���¼���Id��ʱ����أ�IdԽС���¼�������ʱ��ԽС�����¼�����*/

	srand((unsigned)time(NULL));//iomanip
								//�������ɸ����������������¼�����ʼʱ��
	vector<list<int>> startTTIVec(m_Config.periodicEventNTTI, list<int>());
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		int startTTI = rand() % m_Config.periodicEventNTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	srand((unsigned)time(NULL));//iomanip
								//����startTTIVec�������PERIOD�¼��������в�����Ӳ��ɷֲ��Ľ����¼�
	int countEmergency = 0;
	int CTTI = 0;
	while (CTTI < m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			list<int>lst = startTTIVec[TTIOffset];
			for (int VeUEId : lst) {
				//----------------WRONG--------------------
				//�������p�����Ǵ������ʣ����ǵ�λ�¼��ڴ����Ĵ���
				//�������Ӳ��ɷֲ��Ľ����¼�
				double p = m_Config.emergencyLamda / m_NTTI;//�����ĸ���
				double r = static_cast<double>(rand()) / RAND_MAX;//����[0-1)�ֲ��������
				if (r < p&&CTTI + TTIOffset < m_NTTI) {//�������С�ڸ���p������Ϊ���¼�����
					sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
					countEmergency++;
				}
				//----------------WRONG--------------------

				//�����������¼�
				if (CTTI + TTIOffset < m_NTTI) {
					/*-----------------------ATTENTION-----------------------
					* ����������sEvent�Ķ���Ȼ����ѹ��m_EventVec
					* ����Vector<T>.push_back��ѹ�봫�����ĸ���Ʒ����˻����sEvent�Ŀ������캯��
					* sEventĬ�ϵĿ������캯���ḳֵid��Ա������ǰ�ȫ�ģ�
					*sEvent����Զ��忽�����캯���������ڹ��캯���ĳ�ʼ�����ֿ���id��Ա
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
	//������ȴ�ʱ��
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++)
		if (m_EventVec[eventId].isSuccessded)
			g_OutDelayStatistics << m_EventVec[eventId].queuingDelay << " ";
	g_OutDelayStatistics << endl;

	//Ȼ���������ʱ��
	for (int eventId = 0;eventId < static_cast<int>(m_EventVec.size());eventId++)
		if (m_EventVec[eventId].isSuccessded)
			g_OutDelayStatistics << m_EventVec[eventId].sendDelay << " ";
	g_OutDelayStatistics << endl;
}