#include<iostream>
#include"Enumeration.h"
#include"Numerical.h"
#include"Schedule.h"
#include"System.h"
using namespace std;

int main() {
	Matrix m(1,1);
	m.hermitian();

	vector<PFInfo> v;
	for (int i = 0; i < 100; i++) {
		v.push_back(PFInfo(0, 0, (double)i*gc_PI));
	}

	cSystem sys;
	for (int k = 1; k <= 100; k++) {
		PFInfo res = sys.selectKthPF(v, k, 0, v.size() - 1);
		cout << "Num " << k << " : " << res.FactorPF/gc_PI << endl;
	}
	system("pause");
}