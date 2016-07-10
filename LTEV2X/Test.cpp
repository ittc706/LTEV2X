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

int TestVUENum = 20;
int TestRSUNum = 2;
int TesteNBNum = 1;


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
	cout << "\n\n-----------------------VEUE------------------------"<<endl;
	for (cVeUE &ve : m_VecVUE)
		ve.print();
	cout << "\n\n-----------------------eNB------------------------" << endl;
	for (ceNB &eNB : m_VeceNB)
		eNB.print();
	cout << "\n\n-----------------------RSU------------------------" << endl;
	for (cRSU &RSU : m_VecRSU)
		RSU.print();

	for (int i = 0;i < m_NTTI;i++) {
		cout << "TTI: " << i + m_STTI << endl;
		for (sEvent &e : m_CallSetupList[i])
			cout << "    [ VEId: " << e.VEId << " , " << "callSetupTTI: " << e.callSetupTTI << " , " << "eMessageType :" << (e.message.messageType == PERIOD ? "PEROID" : "ELSE") << " ] ,";
		cout << endl;
	}
}


void cSystem::configure() {//系统仿真参数配置
	m_NTTI = 50;//仿真TTI时间
}


void cSystem::initialization() {
	srand((unsigned)time(NULL));//iomanip
	m_STTI = abs(rand() % 1000);
	m_TTI = m_STTI;
	m_VeceNB = vector<ceNB>(TesteNBNum);
	m_VecVUE = vector<cVeUE>(TestVUENum);
	m_VecRSU = vector<cRSU>(TestRSUNum);
	m_CallSetupList = vector<list<sEvent>>(m_NTTI);

	/*填充基站包含的RSU以及VE*/
	m_VeceNB[0].m_VecRSU = Function::makeEqualInterverSequence(0, 1, TestRSUNum);
	m_VeceNB[0].m_VecVUE = Function::makeEqualInterverSequence(0, 1, TestVUENum);

	/*随机将车辆分配给RSU*/
	for (int VEId = 0;VEId < TestVUENum;VEId++)
		m_VecRSU[rand() % TestRSUNum].m_VecVUE.push_back(VEId);

	m_DRAMode = P123;


	/*生成事件链表*/
	
	/*首先给每辆车填充PERIOD事件*/
	for (int VEId = 0;VEId < TestVUENum;VEId++) {
		int curRelativeTTI = rand() % m_Config.periodicEventNTTI;//车辆周期性事件起始的相对TTI
		while (curRelativeTTI < m_NTTI) {
			int curAbsoluteTTI = curRelativeTTI + m_STTI;//绝对TTI时刻
			m_CallSetupList[curRelativeTTI].push_back(sEvent(VEId, curAbsoluteTTI, PERIOD));
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
	m_isHavingDataToTransmit = true;
}


void cVeUE::print() {
	cout << "  VeUE Id: " << m_VEId << endl;
	m_Message.print();
	cout << endl;
}


int ceNB::count = 0;

ceNB::ceNB() {
	m_eNBId = count++;
}

void ceNB::print() {
	cout << "  eNB Id: " <<m_eNBId<< endl;
	cout << "      VUE ID: " << endl;
	cout << "        ";
	for (int Id : m_VecVUE)
		cout << Id << " , ";
	cout << endl;
	cout << "      RSU ID: " << endl;
	cout << "        ";
	for (int Id : m_VecRSU)
		cout << Id << " , ";
	cout << endl;
}

int cRSU::count = 0;

cRSU::cRSU() :m_DRAClusterNum(4) {
	m_RSUId = count++;
	m_DRAVecCluster = vector<vector<int>>(m_DRAClusterNum);
	m_DRA_RBIsAvailable = vector<vector<int>>(m_DRAClusterNum, vector<int>(gc_DRA_FBNum, -1));
	m_DRACallList = vector<vector<int>>(m_DRAClusterNum);
	m_DRAScheduleList = vector<vector<list<sDRAScheduleInfo>>>(m_DRAClusterNum, vector<list<sDRAScheduleInfo>>(gc_DRA_FBNum));
}
void cRSU::print() {
	cout << "  RSU Id: " << m_RSUId << endl;
	for (int i = 0; i < m_DRAClusterNum; i++){
		cout << "      Cluster : " << i << endl;
		cout << "      ";
		for(int Id: m_DRAVecCluster[i])
			cout << Id << " , ";
		cout << endl;
	}
	
}

void cRSU::testCluster() {
	int num = m_VecVUE.size();
	for (int i = 0; i < num; i++) {
		int clusterIdx = i%m_DRAClusterNum;
		m_DRAVecCluster[clusterIdx].push_back(m_VecVUE[i]);
	}

	
}

void sMessage::print() {
	cout << "  MessageInfo:" << endl;
	cout << "    byteNum = " << byteNum << endl;
	cout << "    DRA_ONTTI = " << DRA_ONTTI << endl;
}