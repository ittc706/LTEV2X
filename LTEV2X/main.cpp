#include<iostream>
#include<fstream>
#include<iomanip>
#include"Enumeration.h"
#include"Numerical.h"
#include"Schedule.h"
#include"System.h"
using namespace std;

extern int newCount;
extern int deleteCount;

int main() {
	/*
	Matrix m(1,1);
	m.hermitian();

	vector<sPFInfo> v;
	for (int i = 0; i < 100; i++) {
		v.push_back(sPFInfo(0, 0, (double)i*gc_PI));
	}
    */


	try {
		cSystem _system;
		_system.process();
	}
	catch (Exp e) {
		cout << e.what() << endl;
	}

	cout << "newCount: " << newCount << " , deleteCount: " << deleteCount << endl;



	g_OutDRAScheduleInfo.close();
	g_OutClasterPerformInfo.close();
	g_OutEventListInfo.close();
	g_OutTTILogInfo.close();
	g_OutEventLogInfo.close();
	g_OutVeUELocationUpdateLogInfo.close();


	system("pause");
}