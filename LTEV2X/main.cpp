#include<iostream>
#include"Enumeration.h"
#include"Numerical.h"
#include"Schedule.h"
#include"System.h"
#include"Test.h"
using namespace std;

int main() {
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
	rsu.m_Cluster = vector<vector<int>>{ Function::getVector(1), Function::getVector(24) ,Function::getVector(1) ,Function::getVector(74) };
    rsu.DRAPerformCluster();
	Function::print1DimVector(rsu.m_DRA_ETI);


	system("pause");
}