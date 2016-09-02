#include <iostream>
#include <fstream>
#include <iomanip>
#include "Enumeration.h"
#include "System.h"
#include "Exception.h"
using namespace std;

extern int newCount;
extern int deleteCount;

int main() {
	
	/*
	vector<sPFInfo> v;
	for (int i = 0; i < 100; i++) {
		v.push_back(sPFInfo(0, 0, (double)i*gc_PI));
	}
    */
	
	long double start = clock();

	try {
		cSystem _system;
		_system.process();
	}
	catch (Exp e) {
		cout << e.what() << endl;
	}

	cout << "newCount: " << newCount << " , deleteCount: " << deleteCount << endl;

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
	
	/*list<int> l{ 6,6,6 };
	auto it = l.begin();
	it = l.insert(it, 1);
	it = l.insert(it, 3);
	for (auto c : l)
		cout << c << " ";
*/
	system("pause");

}