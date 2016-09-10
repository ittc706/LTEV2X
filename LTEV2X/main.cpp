#include<iostream>
#include<fstream>
#include<iomanip>
#include"Enumeration.h"
#include"System.h"
#include"Exception.h"
#include"Matrix.h"
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
	return 1;
}


//int main() {
//	try {
//		Matrix m{
//			{{1,1}},
//			{{0,0},{3,3}},
//			{{1,5},{0,0},{2,2}},
//		};
//		Matrix n{
//			{{1,2},{2,2},{3,3}},
//			{{4,4},{5,3},{6,6}},
//			{{7,9},{8,8},{9,3}},
//		};
//		double a = 5.6;
//		(1/n).print();
//		cout << endl;
//		elementDivide(m, n).print();
//		cout << endl;
//		n.diag().print();
//	}
//	catch (Exp e) {
//		cout << e.what() << endl;
//	}
//	
//
//
//	system("pause");
//	return 1;
//}