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
	srand((unsigned)time(NULL));

	System* _system = new System();;
	_system->process();


	long double end = clock();

	cout.setf(ios::fixed);
	cout << "\nRunning Time :" << setprecision(1) << (end - start) / 1000.0L << " s\n" << endl;
	cout.unsetf(ios::fixed);

	g_FileTemp.close();

	g_FileVeUELocationUpdateLogInfo.close();
	g_FileVeUENumPerRSULogInfo.close();
	g_FileLocationInfo.close();

	g_FileScheduleInfo.close();
	g_FileClasterPerformInfo.close();
	g_FileEventListInfo.close();
	g_FileTTILogInfo.close();
	g_FileEventLogInfo.close();
	
	g_FileDelayStatistics.close();
	g_FileEmergencyPossion.close();
	g_FileDataPossion.close();
	g_FileConflictNum.close();
	g_FileTTIThroughput.close();
	g_FileRSUThroughput.close();

	delete _system;
	system("pause");
	return 1;
}


//int main() {
//	try {
//		Matrix m{
//			{{1,2},{2,2},{3,3}},
//			{{4,4},{5,3},{6,6}},
//			{{7,9},{8,8},{9,3}},
//		};
//
//		RowVector rv{
//			{ 1,2 },{ 2,2 },{ 3,3 }
//		};
//
//
//		cout << "\n\n-------------RowVector(������ֵ)-------------" << endl;
//		cout << "=====RowVector(���Ը�ֵ)=====" << endl;
//		RowVector rvCopyAssign;
//		rvCopyAssign = rv;
//		rv.print(); cout << endl;
//		rv[1] = Complex{ 0,0 };
//		rv.print(); cout << endl;
//		rvCopyAssign.print(); cout << endl;
//		cout << "=====RowVector(�Ը�ֵ)=====" << endl;
//		rvCopyAssign = rvCopyAssign;
//		rvCopyAssign.print(); cout << endl;
//
//		cout << "\n\n-------------RowVector(�ƶ���ֵ)-------------" << endl;
//		rv=RowVector{
//			{ 1,2 },{ 2,2 },{ 3,3 }
//		};
//		cout << "=====RowVector(���Ը�ֵ)=====" << endl;
//		RowVector rvMoveAssign;
//		rvMoveAssign = std::move(rv);
//		rvMoveAssign.print(); cout << endl;
//		cout << "=====RowVector(�Ը�ֵ)=====" << endl;
//		rvMoveAssign = std::move(rvMoveAssign);
//		rvMoveAssign.print(); cout << endl;
//
//
//		//���ȼ��Matrix������ֵǰ���Ƿ��й�ϵ
//		cout << "\n\n-------------Matrix(������ֵ)-------------" << endl;
//		cout << "=====Matrix(���Ը�ֵ)=====" << endl;
//		m = Matrix{
//			{ { 1,2 },{ 2,2 },{ 3,3 } },
//			{ { 4,4 },{ 5,3 },{ 6,6 } },
//			{ { 7,9 },{ 8,8 },{ 9,3 } },
//		};
//		Matrix mCopyAssign;
//		mCopyAssign = m;
//		m.print(); cout << endl;
//		m[1][1] = Complex{ 0,0 };
//		m.print(); cout << endl;
//		mCopyAssign.print(); cout << endl;
//		cout << "=====Matrix(�Ը�ֵ)=====" << endl;
//		mCopyAssign = mCopyAssign;
//		mCopyAssign.print(); cout << endl;
//
//		cout << "\n\n-------------Matrix(�ƶ���ֵ)-------------" << endl;
//		cout << "=====Matrix(���Ը�ֵ)=====" << endl;
//		m = Matrix{
//			{ { 1,2 },{ 2,2 },{ 3,3 } },
//			{ { 4,4 },{ 5,3 },{ 6,6 } },
//			{ { 7,9 },{ 8,8 },{ 9,3 } },
//		};
//		Matrix mMoveAssign;
//		mMoveAssign = std::move(m);
//		mMoveAssign.print(); cout << endl;
//		cout << "=====Matrix(�Ը�ֵ)=====" << endl;
//		mMoveAssign = std::move(mMoveAssign);
//		mMoveAssign.print(); cout << endl;
//
//
//
//		cout << "\n\n-------------Matrix(��������)-------------" << endl;
//		m = Matrix{
//			{ { 1,2 },{ 2,2 },{ 3,3 } },
//			{ { 4,4 },{ 5,3 },{ 6,6 } },
//			{ { 7,9 },{ 8,8 },{ 9,3 } },
//		};
//		Matrix mCopyConstruct(m);
//		mCopyConstruct.print(); cout << endl;
//		m[1][1] = Complex(-100, -100);
//		mCopyConstruct.print(); cout << endl;
//
//	}
//	catch (Exp e) {
//		cout << e.what() << endl;
//	}
//
//	system("pause");
//	return 1;
//}



//int main() {
//	WT_B* p = new WT_B(nullptr);
//	p->testCloest();
//	system("pause");
//	return 1;
//}



//int main() {
//	
//
//	Matrix m{
//		{{1,2}, },
//	{ {2,3},{3,4},}
//	};
//	Matrix m1 = std::move(-m);
//	
//	system("pause");
//	return 1;
//}



//int main() {
//	int testMain1();
//	testMain1();
//	system("pause");
//	return 1;
//}


//int main() {
//	WT_B *w = new WT_B(Configure(), nullptr, nullptr);
//	w->testSINR();
//	system("pause");
//	return 1;
//}