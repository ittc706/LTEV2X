#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"
#include"RSU.h"

using namespace std;

void cSystem::process() {

	/*参数配置*/
	configure();

	/*仿真初始化*/
	initialization();

	/*打印事件链表*/
	writeEventListInfo(g_OutEventListInfo);

	for (int count = 0;count < m_NTTI;count++) {
		cout << "Current TTI = " << m_TTI << endl;
		DRASchedule();
		m_TTI++;
	}

	/*打印事件日志信息*/
	writeEventLogInfo(g_OutEventLogInfo);


	/*打印车辆地理位置更新日志信息*/
	writeVeUELocationUpdateLogInfo(g_OutVeUELocationUpdateLogInfo);
}

void cSystem::configure() {//系统仿真参数配置
	m_NTTI = 50;//仿真TTI时间
	m_Config.periodicEventNTTI = 20;
	m_Config.locationUpdateNTTI = 30;

	m_Config.VUENum = 30;
	m_Config.RSUNum = 2;
	m_Config.eNBNum = 1;
}


void cSystem::initialization() {
	srand((unsigned)time(NULL));//iomanip
	m_TTI = 0;
	Log::TTI = &m_TTI;

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
	vector<list<int>> startTTIVec(m_Config.periodicEventNTTI, list<int>({ -1 }));
	for (int VeUEId = 0; VeUEId < m_Config.VUENum; VeUEId++) {
		int startTTI = rand() % m_Config.periodicEventNTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	/*根据startTTIVec依次填充PERIOD事件*/
	int CTTI = 0;
	while (CTTI < m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			list<int>lst = startTTIVec[TTIOffset];
			for (int VeUEId : lst) {
				if (VeUEId == -1) {//非法ID，可在此插入随机性事件
				/*-----------------------WARN-----------------------
				* 这里可以插入随机性事件，为了保证始终能运行到这里（列表不为空即可），
				* 在初始化lst的时候插入了非法的VeUEId=-1
				*-----------------------WARN-----------------------*/
				}
				else {//合法ID，添加该事件
					if (CTTI + TTIOffset < m_NTTI) {
						/*-----------------------WARN-----------------------
						* 这里先生成sEvent的对象，然后将其压入m_EventVec
						* 由于Vector<T>.push_back是压入传入对象的复制品，因此会调用sEvent的拷贝构造函数
						* sEvent默认的拷贝构造函数会赋值id成员（因此是安全的）
						*sEvent如果自定义拷贝构造函数，必须在构造函数的初始化部分拷贝id成员
						*-----------------------WARN-----------------------*/
						sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, PERIOD);
						m_EventVec.push_back(evt);
						m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
					}
				}
			}
		}
		CTTI += m_Config.periodicEventNTTI;
	}
}

