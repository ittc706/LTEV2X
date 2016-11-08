/*
* =====================================================================================
*
*       Filename:  TMC_B.cpp
*
*    Description:  TMCģ��
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
#include<random>
#include<sstream>
#include"Exception.h"
#include"TMC_B.h"

using namespace std;

TMC_B::TMC_B(int &t_TTI, 
	Configure& t_Config, 
	RSU* t_RSUAry, 
	VeUE* t_VeUEAry, 
	vector<Event>& t_EventVec, 
	vector<list<int>>& t_EventTTIList, 
	vector<vector<int>>& t_TTIRSUThroughput) :
	TMC_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput) {
	
	//�¼�����������ʼ��
	m_EventTTIList = vector<list<int>>(m_Config.NTTI);

	//������������ʼ��
	m_TTIRSUThroughput = vector<vector<int>>(m_Config.NTTI, vector<int>(m_Config.RSUNum));
}


void TMC_B::initialize() {
	//��ʼ��VeUE�ĸ�ģ���������
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeTMC();
	}

	//��ʼ��RSU�ĸ�ģ���������
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeTMC();
	}
}


void TMC_B::buildEventList(ofstream& out) {
	/*��ʱ��˳���¼���Id��ʱ����أ�IdԽС���¼�������ʱ��ԽС�����¼�����*/

	default_random_engine dre;//���������
	dre.seed((unsigned)time(NULL));//iomanip
	uniform_real_distribution<double> urd(0, 1);//������ֲ�


	//�������ɸ����������������¼�����ʼʱ��(���ʱ�̣���[0 , m_Config.periodicEventNTTI)
	vector<list<int>> startTTIVec(m_Config.periodicEventNTTI, list<int>());
	uniform_int_distribution<int> uid(0, m_Config.periodicEventNTTI - 1);
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		int startTTI = uid(dre);
		startTTIVec[startTTI].push_back(VeUEId);
	}

	//���ɽ����¼��ķ���ʱ�̣�ÿ��ʱ��۴�Ÿ�ʱ�̷��������¼��ĳ���
	m_VeUEEmergencyNum = vector<int>(m_Config.VeUENum, 0);//��ʼ��ͳ����
	int countEmergency = 0;
	vector<list<int>> emergencyEventTriggerTTI(m_Config.NTTI);
	if (m_Config.emergencyLambda != 0) {
		for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
			//��������ÿ�������Ľ����¼�����ʱ��
			double T = 0;
			while (T < m_Config.NTTI) {
				double u = urd(dre);
				if (u == 0) throw Exp("uniform_real_distribution���ɷ�Χ�����߽�");
				T = T - (1 / m_Config.emergencyLambda)*log(u);
				int IntegerT = static_cast<int>(T);
				if (IntegerT < m_Config.NTTI) {
					emergencyEventTriggerTTI[IntegerT].push_back(VeUEId);
					++m_VeUEEmergencyNum[VeUEId];
					++countEmergency;
				}
			}
		}
	}
	cout << "countEmergency: " << countEmergency << endl;


	//��������ҵ���¼��ķ���ʱ�̣�ÿ��ʱ��۴�Ÿ�ʱ�̷�������ҵ���¼��ĳ���
	m_VeUEDataNum = vector<int>(m_Config.VeUENum, 0);//��ʼ��ͳ����
	int countData = 0;
	vector<list<int>> dataEventTriggerTTI(m_Config.NTTI);
	if (m_Config.dataLambda != 0) {
		for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
			//��������ÿ�������Ľ����¼�����ʱ��
			double T = 0;
			while (T < m_Config.NTTI) {
				double u = urd(dre);
				if (u == 0) throw Exp("uniform_real_distribution���ɷ�Χ�����߽�");
				T = T - (1 / m_Config.dataLambda)*log(u);
				int IntegerT = static_cast<int>(T);
				if (IntegerT < m_Config.NTTI) {
					dataEventTriggerTTI[IntegerT].push_back(VeUEId);
					++m_VeUEDataNum[VeUEId];
					++countData;
				}
			}
		}
	}
	cout << "countData: " << countData << endl;


	//����startTTIVec�������PERIOD�¼��������в�����Ӳ��ɷֲ��Ľ����¼�

	int startTTIOfEachPeriod = 0;//ÿ�����ڵ���ʼʱ��
	while (startTTIOfEachPeriod < m_Config.NTTI) {
		//TTIOffsetΪ�����startTTIOfEachPeriod��ƫ����
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			//ѹ������¼�
			if (startTTIOfEachPeriod + TTIOffset < m_Config.NTTI) {
				list<int> &emergencyList = emergencyEventTriggerTTI[startTTIOfEachPeriod + TTIOffset];
				for (int VeUEId : emergencyList) {
					/*-----------------------ATTENTION-----------------------
					* ����������sEvent�Ķ���Ȼ����ѹ��m_EventVec
					* ����Vector<T>.push_back��ѹ�봫�����ĸ���Ʒ����˻����sEvent�Ŀ������캯��
					* sEventĬ�ϵĿ������캯���ḳֵid��Ա������ǰ�ȫ�ģ�
					*sEvent����Զ��忽�����캯���������ڹ��캯���ĳ�ʼ�����ֿ���id��Ա
					*-----------------------ATTENTION-----------------------*/
					Event evt = Event(VeUEId, startTTIOfEachPeriod + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.eventId);
					--countEmergency;
				}
			}


			//ѹ������ҵ���¼�
			if (startTTIOfEachPeriod + TTIOffset < m_Config.NTTI) {
				list<int> &dataList = dataEventTriggerTTI[startTTIOfEachPeriod + TTIOffset];
				for (int VeUEId : dataList) {
					/*-----------------------ATTENTION-----------------------
					* ����������sEvent�Ķ���Ȼ����ѹ��m_EventVec
					* ����Vector<T>.push_back��ѹ�봫�����ĸ���Ʒ����˻����sEvent�Ŀ������캯��
					* sEventĬ�ϵĿ������캯���ḳֵid��Ա������ǰ�ȫ�ģ�
					*sEvent����Զ��忽�����캯���������ڹ��캯���ĳ�ʼ�����ֿ���id��Ա
					*-----------------------ATTENTION-----------------------*/
					Event evt = Event(VeUEId, startTTIOfEachPeriod + TTIOffset, DATA);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.eventId);
					--countData;
				}
			}


			//�����������¼�
			if (startTTIOfEachPeriod + TTIOffset < m_Config.NTTI) {
				list<int> &periodList = startTTIVec[TTIOffset];
				for (int VeUEId : periodList) {
					Event evt = Event(VeUEId, startTTIOfEachPeriod + TTIOffset, PERIOD);
					m_EventVec.push_back(evt);
					m_EventTTIList[startTTIOfEachPeriod + TTIOffset].push_back(evt.eventId);
				}
			}
		}
		startTTIOfEachPeriod += m_Config.periodicEventNTTI;
	}
	
	//��ӡ�¼�����
	writeEventListInfo(out);
}

void TMC_B::processStatistics(ofstream& outDelay, ofstream& outEmergencyPossion, ofstream& outDataPossion, ofstream& outConflict, ofstream& outEventLog) {
	stringstream ssPeriod;
	stringstream ssEmergency;
	stringstream ssData;

	/*-----------------------ATTENTION-----------------------
	* endl���ݷ�����Ҫ���������е�����ˢ�µ�����
	* ��������������ʱ������д�����ݣ��ļ����ǿյ�
	*-----------------------ATTENTION-----------------------*/


	//ͳ�Ƴɹ�������¼���Ŀ
	m_TransimitSucceedEventNumPerEventType = vector<int>(3);
	for (Event &event : m_EventVec) {
		if (event.isSuccessded) {
			switch (event.message.messageType) {
			case PERIOD:
				m_TransimitSucceedEventNumPerEventType[0]++;
				break;
			case EMERGENCY:
				m_TransimitSucceedEventNumPerEventType[1]++;
				break;
			case DATA:
				m_TransimitSucceedEventNumPerEventType[2]++;
				break;
			}
		}
			
	}
	cout << "�ɹ�����ͳ��" << endl;
	for (int num : m_TransimitSucceedEventNumPerEventType)
		cout << num << endl;

	
	//ͳ�Ƶȴ�ʱ��
	for (Event &event : m_EventVec)
		if (event.isSuccessded) {
			switch (event.message.messageType) {
			case PERIOD:
				ssPeriod << event.queuingDelay << " ";
				break;
			case EMERGENCY:
				ssEmergency << event.queuingDelay << " ";
				break;
			case DATA:
				ssData << event.queuingDelay << " ";
				break;
			default:
				throw Exp("�Ƿ���Ϣ����");
			}
		}
	outDelay << ssPeriod.str() << endl;
	outDelay << ssEmergency.str() << endl;
	outDelay << ssData.str() << endl;

	
	//ͳ�ƴ���ʱ��
	ssPeriod.str("");
	ssEmergency.str("");
	ssData.str("");
	for (Event &event : m_EventVec)
		if (event.isSuccessded) {
			switch (event.message.messageType) {
			case PERIOD:
				ssPeriod << event.sendDelay << " ";
				break;
			case EMERGENCY:
				ssEmergency << event.sendDelay << " ";
				break;
			case DATA:
				ssData << event.sendDelay << " ";
				break;
			default:
				throw Exp("�Ƿ���Ϣ����");
			}
		}
	outDelay << ssPeriod.str() << endl;
	outDelay << ssEmergency.str() << endl;
	outDelay << ssData.str() << endl;

	//ͳ�ƽ����¼��ֲ����
	for (int num : m_VeUEEmergencyNum)
		outEmergencyPossion << num << " ";
	outEmergencyPossion << endl;//����ܹؼ�����������������ˢ�µ�����

	//ͳ������ҵ���¼��ֲ����
	for (int num : m_VeUEDataNum)
		outDataPossion << num << " ";
	outDataPossion << endl;//����ܹؼ�����������������ˢ�µ�����

	//ͳ�Ƴ�ͻ���
	ssPeriod.str("");
	ssEmergency.str("");
	ssData.str("");
	for (Event &event : m_EventVec) {
		switch (event.message.messageType) {
		case PERIOD:
			ssPeriod << event.conflictNum << " ";
			break;
		case EMERGENCY:
			ssEmergency << event.conflictNum << " ";
			break;
		case DATA:
			ssData << event.conflictNum << " ";
			break;
		default:
			throw Exp("�Ƿ���Ϣ����");
		}
	}
	outConflict << ssPeriod.str() << endl;
	outConflict << ssEmergency.str() << endl;
	outConflict << ssData.str() << endl;
	writeEventLogInfo(outEventLog);

	//ͳ��������
	vector<int> tmpTTIThroughput(m_Config.NTTI);
	vector<int> tmpRSUThroughput(m_Config.RSUNum);
	for (int tmpTTI = 0; tmpTTI < m_Config.NTTI; tmpTTI++) {
		for (int tmpRSUId = 0; tmpRSUId < m_Config.RSUNum; tmpRSUId++) {
			tmpTTIThroughput[tmpTTI] += m_TTIRSUThroughput[tmpTTI][tmpRSUId];
			tmpRSUThroughput[tmpRSUId]+= m_TTIRSUThroughput[tmpTTI][tmpRSUId];
		}
	}

	//��TTIΪ��λͳ��������
	for (int throughput : tmpTTIThroughput) 
		g_FileTTIThroughput << throughput << " ";
	g_FileTTIThroughput << endl;

	//��RSUΪ��λͳ��������
	for (int throughput : tmpRSUThroughput) 
		g_FileRSUThroughput<< throughput << " ";
	g_FileRSUThroughput << endl;
}

void TMC_B::writeEventListInfo(ofstream &out) {
	for (int i = 0; i < m_Config.NTTI; i++) {
		out << "[ TTI = " << left << setw(3) << i << " ]" << endl;
		out << "{" << endl;
		for (int eventId : m_EventTTIList[i]) {
			Event& e = m_EventVec[eventId];
			out << "    " << e.toString() << endl;
		}
		out << "}\n\n" << endl;
	}
}
void TMC_B::writeEventLogInfo(ofstream &out) {
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
		out << "EventId = " << eventId << endl;
		out << "{" << endl;
		out << "    " << "VeUEId = " << m_EventVec[eventId].VeUEId << endl;
		out << "    " << "MessageType = " << s << endl;
		out << "    " << "SendDelay = " << m_EventVec[eventId].sendDelay << "(TTI)" << endl;
		out << "    " << "QueuingDelay = " << m_EventVec[eventId].queuingDelay << "(TTI)" << endl;
		out << m_EventVec[eventId].toLogString(1);
		out << "}" << endl;
	}
}

