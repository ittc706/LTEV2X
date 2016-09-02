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

	g_OutTemp.close();
	g_OutRRScheduleInfo.close();
	g_OutDRAScheduleInfo.close();
	g_OutClasterPerformInfo.close();
	g_OutEventListInfo.close();
	g_OutTTILogInfo.close();
	g_OutEventLogInfo.close();
	g_OutVeUELocationUpdateLogInfo.close();
	g_OutDelayStatistics.close();
    g_OutEmergencyPossion.close();

	system("pause");
}