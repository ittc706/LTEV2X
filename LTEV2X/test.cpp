#include<random>
#include<thread>
#include<iomanip>
#include"WT_B.h"

using namespace std;

void testThread(int count) {
	default_random_engine e;
	uniform_real_distribution<double> u(1, 3);
	for (int iter1 = 0; iter1 <= count; iter1++) {


		Matrix m_H{
			{ Complex(u(e),u(e)) },
			{ Complex(u(e),u(e)) },
		};

		/* 下面开始计算W */

		Matrix HHermit(1, 2);
		HHermit = m_H.hermitian();//求信道矩阵的hermitian

		Matrix inverseExpLeft = 2 * m_H * HHermit;//求逆表达式左边那项

												  //计算干扰项
		Matrix Interference1(2, 2);
		Matrix m_HInterference{
			{ Complex(u(e),u(e)) },
			{ Complex(u(e),u(e)) },
		};
		for (int i = 0; i < 5; i++) {
			Interference1 = Interference1 + 3 * m_HInterference * m_HInterference.hermitian();
		}

		//求逆表达式右边那项
		Matrix inverseExpRight = 2 * Matrix::eye(2) + Interference1;//sigma上带曲线

		Matrix W = (inverseExpLeft + inverseExpRight).inverse(true)*sqrt(2)*m_H;//权重矩阵


																				/* 下面开始计算D */
																				//先计算分子
		Matrix WHer = W.hermitian();
		Matrix D = sqrt(2)*WHer*m_H;
		Matrix DHer = D.hermitian();
		Matrix molecular = D*DHer;//SINR运算的分子，1*1的矩阵


								  //然后计算分母
		Matrix denominatorLeft = WHer*W * 2;//SINR运算的分母中的第一项
		Matrix Iself = WHer*m_H*sqrt(2) - D;
		Matrix IselfHer = Iself.hermitian();
		Matrix denominatorMiddle = Iself*IselfHer; //SINR运算的分母中的第二项

												   /*以下计算公式中的干扰项,即公式中的第三项*/
		Matrix denominatorRight(1, 1);
		for (int i = 0; i < 5; i++) {
			denominatorRight = denominatorRight + 2 * WHer*m_HInterference * m_HInterference.hermitian()*W;
		}

		Matrix denominator = denominatorLeft + denominatorMiddle + denominatorRight;//SINR运算的分母

	}
}



int testMain1() {
	long double start = clock();
	testThread(2500);
	long double end = clock();

	cout.setf(ios::fixed);
	cout << "\nRunning Time :" << setprecision(1) << (end - start) / 1000.0L << " s\n" << endl;
	cout.unsetf(ios::fixed);

	start = clock();
	int threadNum = 2;
	thread* threads = new thread[threadNum];
	for (int i = 0; i < threadNum; i++) {
		threads[i] = thread(testThread, 2500 / threadNum);
	}

	for (int i = 0; i < threadNum; i++) {
		threads[i].join();
	}
	end = clock();

	cout.setf(ios::fixed);
	cout << "\nRunning Time :" << setprecision(1) << (end - start) / 1000.0L << " s\n" << endl;
	cout.unsetf(ios::fixed);

	system("pause");
	return 1;
}