#include<vector>
#include<iostream>
#include<math.h>
#include<iomanip>
#include<initializer_list>
#include<list>
#include<fstream>
#include<string>

#include"Test.h"
#include"System.h"
#include"Utility.h"


using namespace std;

int sEvent::count = 0;
int cVeUE::count = 0;
int ceNB::count = 0;
int cRSU::count = 0;

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
	for (const tuple<int, int, int> &t : v) {
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



void cSystem::configure() {//系统仿真参数配置
	m_NTTI = 200;//仿真TTI时间
	m_Config.periodicEventNTTI = 20;
	m_Config.locationUpdateNTTI = 50;

	m_Config.VUENum = 20;
	m_Config.RSUNum = 2;
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






cVeUE::cVeUE() {
	m_VeUEId = count++;
}






ceNB::ceNB() {
	m_eNBId = count++;
}





cRSU::cRSU() :m_DRAClusterNum(4) {
	m_RSUId = count++;
	m_DRAClusterVeUEIdList = vector<list<int>>(m_DRAClusterNum);
	m_DRA_RBIsAvailable = vector<vector<int>>(m_DRAClusterNum, vector<int>(gc_DRA_FBNum, -1));
	m_DRAScheduleList = vector<vector<list<sDRAScheduleInfo>>>(m_DRAClusterNum, vector<list<sDRAScheduleInfo>>(gc_DRA_FBNum));
}



