#include<vector>
#include<iostream>
#include<math.h>
#include<iomanip>
#include<initializer_list>
#include<list>
#include<fstream>

#include"Test.h"
#include"System.h"


using namespace std;



vector<int> Function::getVector(int size){
	return vector<int>(size, 0);
}

vector<vector<int>> Function::getVectorDim2(int size, int group) {
	srand((unsigned)time(NULL));//iomanip
	vector<vector<int>> v(group);
	int remain = size;
	for (int i = 0;i < group-1;i++) {
		int curSize = std::rand() % (remain - (group - i) + 1) + 1;
		v[i]=vector<int>(curSize, 0);
		remain -= curSize;
	}
	v[group - 1] = vector<int>(remain, 0);
	return v;
}


void Function::print1DimVector(std::vector<int>&v) {
	for (int t : v)
		cout << t << ", ";
	cout << endl;
}


void Function::printVectorTuple(const vector<tuple<int, int, int>>&v) {
	for (tuple<int, int, int> t : v) {
		cout << "Interval：[ " << get<0>(t) << " , " << get<1>(t) << " ]  , length: " << get<2>(t) << endl;
	}
}


vector<int> Function::makeEqualInterverSequence(int begin, int interval,int num) {
	vector<int> lst;
	int cur = begin;
	for (int i = 0; i < num; i++) {
		lst.push_back(cur);
		cur += interval;
	}
	return lst;
}


void cSystem::print() {
	g_OutBasicInfo << "\n\n-----------------------VEUE------------------------"<<endl;
	for (cVeUE &ve : m_VecVUE)
		ve.print();
	g_OutBasicInfo << "\n\n-----------------------eNB------------------------" << endl;
	for (ceNB &eNB : m_VeceNB)
		eNB.print();
	g_OutBasicInfo << "\n\n-----------------------RSU------------------------" << endl;
	for (cRSU &RSU : m_VecRSU)
		RSU.print();

	for (int i = 0;i < m_NTTI;i++) {
		g_OutBasicInfo << "TTI: " << i + m_STTI << endl;
		for (sEvent &e : m_EventList[i])
			g_OutBasicInfo << "    [ VEId: " << e.VEId << " , " << "callSetupTTI: " << e.callSetupTTI << " , " << "eMessageType :" << (e.message.messageType == PERIOD ? "PEROID" : "ELSE") << " ] ,";
		g_OutBasicInfo << endl;
	}
}


void cSystem::configure() {//系统仿真参数配置
	m_NTTI = 10;//仿真TTI时间
}


void cSystem::initialization() {
	srand((unsigned)time(NULL));//iomanip
	m_STTI = abs(rand() % 1000);
	m_TTI = m_STTI;
	m_VeceNB = vector<ceNB>(m_Config.eNBNum);
	m_VecRSU = vector<cRSU>(m_Config.RSUNum);
	m_VecVUE = vector<cVeUE>(m_Config.VUENum);
	m_EventList = vector<list<sEvent>>(m_NTTI);


	/*随机将车辆分配给RSU*/
	for (int VEId = 0;VEId < m_Config.VUENum;VEId++)
		m_VecRSU[rand() % m_Config.RSUNum].m_VUESet.insert(VEId);


	/*随机将RSU分给基站*/
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		int eNBId = rand() % m_Config.eNBNum;
		m_VeceNB[eNBId].m_RSUSet.insert(RSUId);
		m_VeceNB[eNBId].m_VUESet.insert(m_VecRSU[RSUId].m_VUESet.begin(), m_VecRSU[RSUId].m_VUESet.end());
	}

	
	
	m_DRAMode = P123;


	/*生成事件链表*/
	
	/*首先给每辆车填充PERIOD事件*/
	for (int VEId = 0;VEId < m_Config.VUENum;VEId++) {
		int curRelativeTTI = rand() % m_Config.periodicEventNTTI;//车辆周期性事件起始的相对TTI
		while (curRelativeTTI < m_NTTI) {
			int curAbsoluteTTI = curRelativeTTI + m_STTI;//绝对TTI时刻
			m_EventList[curRelativeTTI].push_back(sEvent(VEId, curAbsoluteTTI, PERIOD));
			curRelativeTTI += m_Config.periodicEventNTTI;
		}
	}
}




int cVeUE::count = 0;

cVeUE::cVeUE() {
	m_VEId = count++;
	switch (count % 3) {
	case 0:
		m_Message = sMessage(PERIOD);
		break;
	case 1:
		m_Message = sMessage(EMERGENCY);
		break;
	case 2:
		m_Message = sMessage(DATA);
		break;
	}
}


void cVeUE::print() {
	g_OutBasicInfo << "  VeUE Id: " << m_VEId << endl;
	m_Message.print();
	g_OutBasicInfo << endl;
}


int ceNB::count = 0;

ceNB::ceNB() {
	m_eNBId = count++;
}

void ceNB::print() {
	g_OutBasicInfo << "  eNB Id: " <<m_eNBId<< endl;
	g_OutBasicInfo << "      VUE ID: " << endl;
	g_OutBasicInfo << "        ";
	for (int Id : m_VUESet)
		g_OutBasicInfo << Id << " , ";
	g_OutBasicInfo << endl;
	g_OutBasicInfo << "      RSU ID: " << endl;
	g_OutBasicInfo << "        ";
	for (int Id : m_RSUSet)
		g_OutBasicInfo << Id << " , ";
	g_OutBasicInfo << endl;
}

int cRSU::count = 0;

cRSU::cRSU() :m_DRAClusterNum(4) {
	m_RSUId = count++;
	m_DRAClusterVUESet = vector<set<int>>(m_DRAClusterNum);
	m_DRA_RBIsAvailable = vector<vector<int>>(m_DRAClusterNum, vector<int>(gc_DRA_FBNum, -1));
	m_DRAScheduleList = vector<vector<list<sDRAScheduleInfo>>>(m_DRAClusterNum, vector<list<sDRAScheduleInfo>>(gc_DRA_FBNum));
}
void cRSU::print() {
	g_OutBasicInfo << "  RSU Id: " << m_RSUId << endl;
	for (int i = 0; i < m_DRAClusterNum; i++){
		g_OutBasicInfo << "      Cluster : " << i << endl;
		g_OutBasicInfo << "      ";
		for(int Id: m_DRAClusterVUESet[i])
			g_OutBasicInfo << Id << " , ";
		g_OutBasicInfo << endl;
	}
	
}

void cRSU::testCluster() {
	for (int VEId :m_VUESet) {
		int clusterIdx = rand()%m_DRAClusterNum;
		m_DRAClusterVUESet[clusterIdx].insert(VEId);
	}

	
}

void sMessage::print() {
	g_OutBasicInfo << "  MessageInfo:" << endl;
	g_OutBasicInfo << "    byteNum = " << byteNum << endl;
	g_OutBasicInfo << "    DRA_ONTTI = " << DRA_ONTTI << endl;
}