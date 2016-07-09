#include<iostream>
#include<fstream>
#include<iomanip>
#include"Enumeration.h"
#include"Numerical.h"
#include"Schedule.h"
#include"System.h"
#include"Test.h"
using namespace std;

ofstream g_OutDRAScheduleInfo("F:\\LTEV2X\\output\\DRAScheduleInfo.txt");


int main() {
	/*
	Matrix m(1,1);
	m.hermitian();

	vector<sPFInfo> v;
	for (int i = 0; i < 100; i++) {
		v.push_back(sPFInfo(0, 0, (double)i*gc_PI));
	}

	cSystem sys;
	for (int k = 1; k <= 100; k++) {
		sPFInfo res = sys.selectKthPF(v, k, 0, v.size() - 1);
		cout << "Num " << k << " : " << res.FactorPF/gc_PI << endl;
	}


	cRSU rsu;
	rsu.m_VecVUE = Function::getVector(100);
	cout << "\n\n-------------------------------------\n\n";
	rsu.m_Cluster = Function::getVectorDim2(100, 4);
	int count = 0;
	for (vector<int>&v : rsu.m_Cluster) {
		cout << v.size() << ", ";
		count += v.size();
	}
	cout << "\ncount: " << count << endl;
	rsu.DRAPerformCluster();
	Function::printVectorTuple(rsu.m_DRAClusterTTI);
	g_TTI = rand()%1000;
	cout << rsu.getDRAClusterIdx() << endl;
	*/
	/*²âÊÔ---DRABasedOnP123()---*/
	
	cSystem _system;
	_system.process();
	//_system.print();


	g_OutDRAScheduleInfo.close();
	system("pause");
}