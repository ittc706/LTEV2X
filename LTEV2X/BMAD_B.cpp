/*
* =====================================================================================
*
*       Filename:  BMAD_ONE.cpp
*
*    Description:  BMADģ��
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
#include"BMAD_B.h"

using namespace std;


void BMAD_B::buildEventList(std::ofstream& out) {
	/*��ʱ��˳���¼���Id��ʱ����أ�IdԽС���¼�������ʱ��ԽС�����¼�����*/

	srand((unsigned)time(NULL));//iomanip
								//�������ɸ����������������¼�����ʼʱ��
	vector<list<int>> startTTIVec(m_Config.periodicEventNTTI, list<int>());
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		int startTTI = rand() % m_Config.periodicEventNTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	//���ɽ����¼��ķ���ʱ�̣�ÿ��ʱ��۴�Ÿ�ʱ�̷��������¼��ĳ���
	m_VeUEEmergencyNum = vector<int>(m_Config.VeUENum, 0);//��ʼ��ͳ����
	int countEmergency = 0;
	vector<list<int>> emergencyEventTriggerTTI(m_Config.m_NTTI);
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		//��������ÿ�������Ľ����¼�����ʱ��
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


	//����startTTIVec�������PERIOD�¼��������в�����Ӳ��ɷֲ��Ľ����¼�

	int CTTI = 0;
	while (CTTI < m_Config.m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			//ѹ������¼�
			if (CTTI + TTIOffset < m_Config.m_NTTI) {
				list<int> &emergencyList = emergencyEventTriggerTTI[CTTI + TTIOffset];
				for (int VeUEId : emergencyList) {
					/*-----------------------ATTENTION-----------------------
					* ����������sEvent�Ķ���Ȼ����ѹ��m_EventVec
					* ����Vector<T>.push_back��ѹ�봫�����ĸ���Ʒ����˻����sEvent�Ŀ������캯��
					* sEventĬ�ϵĿ������캯���ḳֵid��Ա������ǰ�ȫ�ģ�
					*sEvent����Զ��忽�����캯���������ڹ��캯���ĳ�ʼ�����ֿ���id��Ա
					*-----------------------ATTENTION-----------------------*/
					sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
					--countEmergency;
				}
			}

			//�����������¼�
			if (CTTI + TTIOffset < m_Config.m_NTTI) {
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
	
	//��ӡ�¼�����
	writeEventListInfo(out);
}

void BMAD_B::processStatistics(std::ofstream& outDelay, std::ofstream& outPossion, std::ofstream& outConflict, std::ofstream& outEventLog) {
	//ͳ�Ƶȴ�ʱ��
	for (int eventId = 0; eventId < static_cast<int>(m_EventVec.size()); eventId++)
		if (m_EventVec[eventId].isSuccessded)
			outDelay << m_EventVec[eventId].queuingDelay << " ";
	outDelay << endl;//����ܹؼ�����������������ˢ�µ�����

	//ͳ�ƴ���ʱ��
	for (int eventId = 0; eventId < static_cast<int>(m_EventVec.size()); eventId++)
		if (m_EventVec[eventId].isSuccessded)
			outDelay << m_EventVec[eventId].sendDelay << " ";
	outDelay << endl;//����ܹؼ�����������������ˢ�µ�����

								 //ͳ�ƽ����¼��ֲ����
	for (int num : m_VeUEEmergencyNum)
		outPossion << num << " ";
	outPossion << endl;//����ܹؼ�����������������ˢ�µ�����

								  //ͳ�Ƴ�ͻ���
	for (sEvent &event : m_EventVec)
		outConflict << event.conflictNum << " ";
	outConflict << endl;
	writeEventLogInfo(outEventLog);
}

void BMAD_B::writeEventListInfo(std::ofstream &out) {
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
void BMAD_B::writeEventLogInfo(std::ofstream &out) {
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


