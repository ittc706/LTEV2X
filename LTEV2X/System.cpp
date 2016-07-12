#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"

using namespace std;

void cSystem::process() {

	/*参数配置*/
	configure();

	/*仿真初始化*/
	initialization();

	/*打印事件链表*/
	writeEventListInfo(g_OutEventListInfo);

	for (int count = 0;count < m_NTTI;count++) {
		cout << "Current RTTI = " << m_ATTI - m_STTI << endl;
		DRASchedule();
		m_ATTI++;
	}
}

void cSystem::configure() {//系统仿真参数配置
	m_NTTI = 200;//仿真TTI时间
	m_Config.periodicEventNTTI = 20;
	m_Config.locationUpdateNTTI = 50;

	m_Config.VUENum = 10;
	m_Config.RSUNum = 4;
	m_Config.eNBNum = 1;
}


void cSystem::initialization() {
	srand((unsigned)time(NULL));//iomanip
	m_STTI = 0;
	//m_STTI = abs(rand() % 1000);
	m_ATTI = m_STTI;
	Log::ATTI = &m_ATTI;
	Log::STTI = &m_STTI;

	m_eNBVec = vector<ceNB>(m_Config.eNBNum);
	m_RSUVec = vector<cRSU>(m_Config.RSUNum);
	m_VeUEVec = vector<cVeUE>(m_Config.VUENum);
	m_EventTTIList = vector<list<int>>(m_NTTI);



	//由于RSU和基站位置固定，随机将RSU撒给基站进行初始化即可
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		int eNBId = rand() % m_Config.eNBNum;
		m_eNBVec[eNBId].m_RSUIdList.push_back(RSUId);
	}

	m_DRAMode = P123;

	buildEventList();

}

void cSystem::buildEventList() {
	/*按时间顺序（事件的ID与时间相关，ID越小，事件发生的时间越小生成事件链表*/

	//首先生成各个车辆的周期性事件的起始时刻
	vector<list<int>> startTTIVec(gc_DRA_NTTI, list<int>({ -1 }));
	for (int VeUEId = 0; VeUEId < m_Config.VUENum; VeUEId++) {
		int startTTI = rand() % gc_DRA_NTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	/*根据startTTIVec依次填充PERIOD事件*/
	int RTTI = 0;
	while (RTTI < m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < gc_DRA_NTTI; TTIOffset++) {
			list<int>lst = startTTIVec[TTIOffset];
			for (int VeUEId : lst) {
				if (VeUEId == -1) {//非法ID，可在此插入随机性事件
								   /*-----------------------WARN-----------------------
								   * 这里可以插入随机性事件，为了保证始终能运行到这里（列表不为空即可），
								   * 在初始化lst的时候插入了非法的VeUEId=-1
								   *-----------------------WARN-----------------------*/
				}
				else {//合法ID，添加该事件
					if (RTTI + TTIOffset < m_NTTI) {
						sEvent evt = sEvent(VeUEId, RTTI + TTIOffset + m_STTI, RTTI + TTIOffset, PERIOD);
						m_EventVec.push_back(evt);
						m_EventTTIList[RTTI + TTIOffset].push_back(evt.eventId);
					}
				}
			}
		}
		RTTI += m_Config.periodicEventNTTI;
	}
}

void cSystem::writeClusterPerformInfo(ofstream &out) {
	out << "ATTI = " << left << setw(6) << m_ATTI << "RTTI = " << left << setw(6) << m_ATTI - m_STTI << endl;
	out << "{" << endl;
	//打印VeUE信息
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (cVeUE &_VeUE : m_VeUEVec) 
		out << _VeUE.toString(2) << endl;
	out << "    }" << endl;

	out << "\n\n\n";

	//打印基站信息
	out << "    eNB Info: " << endl;
	out << "    {" << endl;
	for (ceNB &_eNB : m_eNBVec)
		out << _eNB.toString(2) << endl;
	out << "    }" << endl;

	//打印RSU信息
	out << "    RSU Info: " << endl;
	out << "    {" << endl;
	for (cRSU &_RSU : m_RSUVec)
		out << _RSU.toString(2) << endl;
	out << "    }" << endl;

	out << "\n\n\n";

	//打印System级Switch链表
	out << "    RSUSwitchList Info: " << endl;
	out << "    {" << endl;
	out << "        [ ";
	for (int VeUEId : m_DRA_RSUSwitchEventIdList)
		out << VeUEId << " , ";
	out << "] " << endl;
	out << "    }" << endl;

	out << "}\n\n" << endl;
}




void cSystem::writeEventListInfo(ofstream &out) {
	//打印事件链表信息
	for (int i = 0; i < m_NTTI; i++) {
		out << "ATTI = " << left << setw(6) << m_STTI + i << "RTTI = " << left << setw(6) << i << endl;
		out << "{" << endl;
		for (int eventId : m_EventTTIList[i]) {
			sEvent& e = m_EventVec[eventId];
			out << "    " << e.toString() << " }" << endl;
		}
		out << "}\n\n" << endl;
	}
}