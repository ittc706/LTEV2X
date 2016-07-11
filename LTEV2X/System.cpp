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

	writeEventListInfo(g_OutEventListInfo);

	for (int count = 0;count < m_NTTI;count++) {
		cout << "Current RTTI = " << m_TTI - m_STTI << endl;
		DRASchedule();
		m_TTI++;
	}
}


void cSystem::writeClusterPerformInfo(ofstream &out) {
	out << "ATTI = " << left << setw(6) << m_TTI << "RTTI = " << left << setw(6) << m_TTI - m_STTI << endl;
	out << "{" << endl;
	//打印VeUE信息
	out << "    VUE Info: " << endl;
	out << "    {" << endl;
	for (cVeUE &_VeUE : m_VeUEVec) 
		out << "        " << _VeUE.toString() << endl;
	out << "    }" << endl;

	out << "\n\n\n";

	//打印基站信息
	out << "    eNB Info: " << endl;
	out << "    {" << endl;
	for (ceNB &_eNB : m_eNBVec)
		out << "        " << _eNB.toString() << endl;
	out << "    }" << endl;

	//打印RSU信息
	out << "    RSU Info: " << endl;
	out << "    {" << endl;
	for (cRSU &_RSU : m_RSUVec)
		out << _RSU.toString() << endl;
	out << "    }" << endl;

	out << "\n\n\n";

	//打印System级Switch链表
	out << "    RSUSwitchList Info: " << endl;
	out << "    {" << endl;
	out << "        [ ";
	for (int VeUEId : m_DRA_RSUSwitchVeUEIdList)
		out << VeUEId << " , ";
	out << "] " << endl;
	out << "    }" << endl;

	out << "}\n\n" << endl;
}




void cSystem::writeEventListInfo(ofstream &out) {
	//打印事件链表信息
	for (int i = 0;i < m_NTTI;i++) {
		out << "ATTI = " << left << setw(6) << m_STTI+i << "RTTI = " << left << setw(6) << i << endl;
		out << "{" << endl;
		for (sEvent &e : m_EventList[i])
			out << "    [ VeUEId = " << left << setw(5) << e.VeUEId << "callSetupTTI = [ " << left << setw(4) << e.callSetupTTI <<" , " << left << setw(4) << e.callSetupTTI-m_STTI<< " ]   eMessageType = " << left << setw(12) << (e.message.messageType == PERIOD ? "PEROID" : "ELSE") << " ] " << endl;
		out << "}\n\n" << endl;
	}
}