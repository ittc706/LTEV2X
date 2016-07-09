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

extern ofstream out;


vector<int> Function::getVector(int size){
	return vector<int>(size, 0);
}

vector<vector<int>> Function::getVectorDim2(int size, int group) {
	std::srand((unsigned)std::time(NULL));//iomanip
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

}


void cSystem::configure() {//系统仿真参数配置
	m_VeceNB = vector<ceNB>(1);
	m_VecVUE = vector<cVeUE>(40);
	m_VecRSU = vector<cRSU>(2);

	m_VeceNB[0].m_VecRSU= Function::makeEqualInterverSequence(0, 1, 2);
	m_VeceNB[0].m_VecVUE= Function::makeEqualInterverSequence(0, 1, 40);

	m_VecRSU[0].m_VecVUE = Function::makeEqualInterverSequence(0, 2, 20);
	m_VecRSU[1].m_VecVUE = Function::makeEqualInterverSequence(1, 2, 20);

	m_DRAMode = P123;
}


int cVeUE::count = 0;

cVeUE::cVeUE() {
	m_VEId = count++;
	switch (count % 3) {
	case 0:
		m_Message.setMessageType(PERIOD);
		break;
	case 1:
		m_Message.setMessageType(EMERGENCY);
		break;
	case 2:
		m_Message.setMessageType(DATA);
		break;
	}
}


void cVeUE::print() {
	cout << "  VeUe Id: " << m_VEId << endl;
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

cRSU::cRSU() :m_ClusterNum(4) {
	m_RSUId = count++;
	m_Cluster = vector<vector<int>>(m_ClusterNum);
	m_DRA_RBIsAvailable = vector<vector<int>>(m_ClusterNum, vector<int>(gc_DRA_FBNum, 0));
	m_CallList = vector<vector<int>>(m_ClusterNum);
	m_DRAScheduleList = vector<vector<sDRAScheduleInfo>>(m_ClusterNum, vector<sDRAScheduleInfo>(gc_DRA_FBNum));

}
void cRSU::print() {
	cout << "  RSU Id: " << m_RSUId << endl;
	for (int i = 0; i < m_Cluster.size(); i++){
		cout << "      Cluster : " << i << endl;
		cout << "      ";
		for(int Id: m_Cluster[i])
			cout << Id << " , ";
		cout << endl;
	}
	
}

void cRSU::Cluster() {
	int num = m_VecVUE.size();
	for (int i = 0; i < num; i++)
		m_Cluster[i%m_ClusterNum].push_back(m_VecVUE[i]);
}

void sMessage::print() {
	cout << "  MessageInfo:" << endl;
	cout << "    byteNum = " << byteNum << endl;
	cout << "    DRA_ONTTI = " << DRA_ONTTI << endl;
}