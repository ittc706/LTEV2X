/*
* =====================================================================================
*
*       Filename:  WT_B.cpp
*
*    Description:  WT模块
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  LN
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<fstream>
#include<iterator>
#include<limits>
#include<random>
#include<math.h>
#include"WT_B.h"



using namespace std;

default_random_engine WT_B::s_Engine(0);

double WT_B::s_RBBandwidth = 180000;

void WT_B::buildTestFile(int t_Nt, int t_Nr, int subNum) {
	uniform_real_distribution<double> u(1, 10);
	int n = 2 * t_Nt * t_Nr *  subNum;
	ofstream outTestH("testH.txt");
	for (int i = 0; i < n; i++)
		outTestH << u(s_Engine) << " ";
	outTestH << endl;
}

WT_B::WT_B(cVeUE* systemVeUEAry):WT_Basic(systemVeUEAry){
	initialize();
}

void WT_B::configuration(){
	/*m_SubCarrierNum = t_SubCarrierNum;
	m_Pt = t_Pt;
	m_Ploss = t_Ploss;
	m_Sigma = t_Sigma;
	m_Mol = t_Mol;*/

	loadH();
	m_PlossInter = vector<double>(2, 1);
}




void WT_B::initialize() {
	//读取m_MCSLevelTable
	std::ifstream in("WT\\MCSLevelTable.txt");
	istream_iterator<int> inIter(in), eof;
	m_MCSLevelTable.assign(inIter, eof);
	in.close();


	//读入信噪比和互信息的对应表，m_mol=2是QPSK，m_mol=4是16QAM，m_mol=6=64QAM
	//维度是1*95

	in.open("WT\\QPSK_MI.txt");
	istream_iterator<double> inIter2(in), eof2;
	m_QPSK_MI.assign(inIter2, eof2);
	in.close();

	in.open("WT\\QAM_MI16.txt");
	inIter2 = istream_iterator<double>(in);
	m_QAM_MI16.assign(inIter2, eof2);
	in.close();

	in.open("WT\\QAM_MI64.txt");
	inIter2 = istream_iterator<double>(in);
	m_QAM_MI64.assign(inIter2, eof2);
	in.close();
}

void WT_B::loadH() {
	//读取信道响应矩阵
	std::ifstream in("WT\\H.txt");
	m_H = Matrix(m_Nr, m_Nt);
	
	for (int row = 0; row < m_Nr; row++) {
		for (int col = 0; col < m_Nt; col++) {
			in >> m_H[row][col].real;
			in >> m_H[row][col].imag;
		}
	}
	in.close();

	//读取干扰信道响应矩阵
	in.open("WT\\HInter.txt");
	while (in) {
		Matrix tmp(m_Nr, m_Nt);
		for (int row = 0; row < m_Nr; row++) {
			for (int col = 0; col < m_Nt; col++) {
				in >> tmp[row][col].real;
				in >> tmp[row][col].imag;
			}
		}
		if (in) {
			m_HInter.push_back(tmp);
		}
	}
	in.close();

	
}




void WT_B::SINRCalculate(int VeUEId) {
	/*****求每个RB上的信噪比****/
	RowVector Sinr(m_SubCarrierNum);//每个子载波上的信噪比，维度为Nt的向量
	for (int subCarrierIdx = 0; subCarrierIdx < m_SubCarrierNum; subCarrierIdx++) {


		Matrix H_her(m_Nt, m_Nr);
		H_her = m_H.hermitian();//求信道矩阵的hermitian
		Matrix Temp1 = m_H * H_her;//Temp中存放运算的临时矩阵，temp中存放运算的临时数值
		double temp1 = m_Pt*m_Ploss;
		Matrix Temp2 = Temp1*temp1;
		Temp2.print();
		Matrix Inter1(m_Nr, m_Nr);

		for (int i = 0; i < (int)m_HInter.size(); i++)
		{
			cout << (int)m_HInter.size() << endl;
			double tep1 = m_Pt*m_PlossInter[i];
			Matrix tep2 = m_HInter[i] * tep1;
			Matrix H_inter_her = m_HInter[i].hermitian();
			Matrix tep3 = tep2*H_inter_her;

			Inter1 = Inter1 + tep3;
		}
		Matrix sigma_p = Inter1 + m_Sigma;//sigma上带曲线

		Matrix Temp3 = Temp2 + sigma_p;
		Temp3.print();
		Matrix Temp4 = Temp3.inverse();
		double temp2 = sqrt(m_Pt*m_Ploss);
		Matrix Temp5 = Temp4*temp2;
		Matrix W(m_Nr, m_Nt); //权重矩阵，大小与信道矩阵相同
		W = Temp5*m_H;//权重矩阵
		Matrix W_her = W.hermitian();
		double temp3 = sqrt(m_Ploss*m_Pt);
		Matrix Temp6 = W_her*temp3;
		Matrix Temp7 = Temp6*m_H;
		Matrix D = Temp7;//目标信号分量D
		Matrix D_her = D.hermitian();
		Matrix Temp11 = D_her*D;
		Matrix Temp12 = Temp11;
		Matrix Temp13 = Temp12;//SINR运算的分子，1*1的矩阵
		Matrix Temp8 = W_her*W;
		Matrix Temp9 = Temp8*m_Sigma;//SINR运算的分母中的第一项

		Matrix Temp14 = W_her*m_H;
		Matrix Temp15 = Temp14*sqrt(m_Pt*m_Ploss);
		Matrix Iself = Temp15 - D;
		Matrix Iself_her = Iself.hermitian();
		Matrix Temp16 = Iself*Iself_her;
		Matrix Temp17 = Temp16; //SINR运算的分母中的第二项

		/*以下计算公式中的干扰项,即公式中的第三项*/
		Matrix Inter2(m_Nr, m_Nr);
		for (int i = 0; i < (int)m_HInter.size(); i++)
		{
			double tmp1 = m_Pt;
			Matrix tmp2 = W_her*tmp1;
			Matrix H_inter_her = m_HInter[i].hermitian();
			Matrix tmp3 = tmp2*m_HInter[i];
			Matrix tmp4 = tmp3*H_inter_her;
			Matrix tmp5 = tmp4*W;
			Inter2 = Inter2 + tmp5;
		}

		Matrix Temp18 = Temp9 + Temp17 + Inter2;//SINR运算的分母

		Sinr[subCarrierIdx] = Temp13[0][0] / Temp18[0][0];
	}
	
	/*****对数方法求有效的SINR,即Sinreff*****/
	//double Sinreff;
	//int num;
	//num = m_Nr*m_Nt*m_SubCarrierNum;
	//double sum = 0;
	//for (int subCarrierIdx; subCarrierIdx < m_SubCarrierNum; subCarrierIdx) {
	//	for (int row = 0; row < m_Nt; row++) {
	//		for (int col = 0; col < m_Nr; col++) {
	//			sum += 10 * log10(Complex::abs(Sinr[subCarrierIdx][row][col]));
	//		}
	//	}
	//}
	//
	//double temp4;
	//temp4 = sum / num / 10;
	//Sinreff = pow(10,temp4);//10的指数函数
	


	//cout << MCS << endl;

	/******互信息方法求有效信噪比Sinreff*****/
	double sum_MI = 0, ave_MI = 0;
	double Sinreff = 0;

	if (m_Mol == 2) {
		for (int k = 0; k < 95; k++) {
			sum_MI = sum_MI + m_QPSK_MI[(int)ceil(Complex::abs(Sinr[k])*2 + 40-0.5)];
		}
		ave_MI = sum_MI / m_SubCarrierNum;
		

		int SNRIndex = closest(m_QPSK_MI, ave_MI);
		Sinreff = 0.5*(SNRIndex - 40);
	}
	
	if (m_Mol == 4) {
		for (int k = 0; k < 95; k++) {
			sum_MI = sum_MI + m_QAM_MI16[(int)ceil(Complex::abs(Sinr[k]) * 2 + 40 - 0.5)];
		}
		ave_MI = sum_MI / m_SubCarrierNum;

		
		int SNRIndex = closest(m_QAM_MI16, ave_MI);
		Sinreff = 0.5*(SNRIndex - 40);
	}
		
	if (m_Mol == 6) {
		for (int k = 0; k < 95; k++) {
			sum_MI = sum_MI + m_QAM_MI64[(int)ceil(Complex::abs(Sinr[k]) * 2 + 40 - 0.5)];
		}
		ave_MI = sum_MI / m_SubCarrierNum;

		
		int SNRIndex = closest(m_QAM_MI64, ave_MI);
		Sinreff = 0.5*(SNRIndex - 40);
	}
	
	int MCS = 0;
	MCS = searchMCSLevelTable(Sinreff);
}



/*****查找MCS等级曲线*****/
int WT_B::searchMCSLevelTable(double SINR) {
	double SINR_dB = 10 * log10(SINR);
	if (SINR_dB > -10 && SINR_dB < 30) {
		return m_MCSLevelTable[static_cast<int>(ceil((SINR_dB+10)*1000))];
	}
	throw Exp("SINR越界");
}


int WT_B::closest(std::vector<double> v, int target) {
	int leftIndex = 0;
	int rightIndex = static_cast<int>(v.size() - 1);
	double leftDiff = v[leftIndex] - target;
	double rightDiff = v[rightIndex] - target;
	
	while (leftIndex <= rightIndex) {
		if (rightDiff <= 0) return v[rightIndex];
		if (leftDiff >= 0) return v[leftIndex];

		int midIndex = rightIndex + (rightIndex - leftIndex >> 1);
		double midDiff = v[midIndex] - target;
		if (midDiff == 0) return midIndex;
		else if (midDiff < 0) {
			leftIndex = midIndex + 1;
			leftDiff = v[leftIndex] - target;
		}
		else {
			rightIndex = midIndex - 1;
			rightDiff = v[rightIndex] - target;
		}
	}
	return abs(v[leftDiff] - target) < abs(v[leftDiff - 1] - target) ? leftDiff : leftDiff - 1;
}


int WT_B::closest2(std::vector<double> v, int target) {
	int res = -1;
	double minimum = (numeric_limits<double>::max)();
	for (int i = 0; i < static_cast<int>(v.size()); i++) {
		if (abs(v[i] - target) < minimum) {
			minimum = v[i] - target;
			res = i;
		}
	}
	return res;
}


void WT_B::test() {
	default_random_engine e;
	uniform_real_distribution<double> u(0,1);
	for (int i = 0; i < 100000; i++) {
		double d = u(e);
		int index1=closest(m_QPSK_MI, d);
		int index2=closest2(m_QPSK_MI, d);
		if (index1 != index2) {
			
			cout << "index1: " << index1 << endl;
			cout << "index2: " << index2 << endl;
			throw Exp("hehe");
		}
	}
}