#include<iostream>
#include<fstream>
#include<iomanip>
#include"Enumeration.h"
#include"System.h"
#include"Exception.h"
using namespace std;


int main() {
	long double start = clock();

	try {
		cSystem _system;
		_system.process();
	}
	catch (Exp e) {
		cout << e.what() << endl;
	}

	long double end = clock();

	cout.setf(ios::fixed);
	cout << "\nRunning Time :" << setprecision(1) << (end - start) / 1000.0L << " s\n" << endl;
	cout.unsetf(ios::fixed);

	g_FileTemp.close();
	g_FileRRScheduleInfo.close();
	g_FileDRAScheduleInfo.close();
	g_FileClasterPerformInfo.close();
	g_FileEventListInfo.close();
	g_FileTTILogInfo.close();
	g_FileEventLogInfo.close();
	g_FileVeUELocationUpdateLogInfo.close();
	g_FileDelayStatistics.close();
    g_FileEmergencyPossion.close();

	system("pause");
}