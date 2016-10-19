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


WT_B::WT_B(cVeUE* systemVeUEAry):WT_Basic(systemVeUEAry){
	initialize();
}


void WT_B::SINRCalculate(int VeUEId,int subCarrierIdxStart,int subCarrierIdxEnd) {
	//配置本次函数调用的参数
	configuration(VeUEId);

	//子载波数量
	int m_SubCarrierNum = subCarrierIdxEnd - subCarrierIdxStart + 1;

	/*****求每个子载波上的信噪比****/
	RowVector Sinr(m_SubCarrierNum);//每个子载波上的信噪比，维度为Nt的向量
	for (int subCarrierIdx = subCarrierIdxStart; subCarrierIdx <= subCarrierIdxEnd; subCarrierIdx++) {
		
		int relativeSubCarrierIdx = subCarrierIdx - subCarrierIdxStart;//相对的子载波下标

		m_H=readH(VeUEId, subCarrierIdx);//读入当前子载波的信道响应矩阵
		m_HInter = readInterH(VeUEId, subCarrierIdx);//读入当前子载波干扰相应矩阵数组

		/*cout << "m_H" << endl;
		m_H.print(cout, 1);*/
		Matrix H_her(m_Nt, m_Nr);
		H_her = m_H.hermitian();//求信道矩阵的hermitian
		Matrix Temp1 = m_H * H_her;//Temp中存放运算的临时矩阵，temp中存放运算的临时数值
		double temp1 = m_Pt*m_Ploss;


		Matrix Temp2 = temp1*Temp1;
		/*cout << "Temp2" << endl;
		Temp2.print(cout, 1);*/
		Matrix Inter1(m_Nr, m_Nr);

		for (int i = 0; i < (int)m_HInter.size(); i++){

			double tep1 = m_Pt*m_PlossInter[i];
			Matrix tep2 = m_HInter[i] * tep1;
			Matrix H_inter_her = m_HInter[i].hermitian();
			Matrix tep3 = tep2*H_inter_her;

			Inter1 = Inter1 + tep3;
		}
		Matrix sigma_p = m_Sigma*Matrix::eye(m_Nr) + Inter1;//sigma上带曲线
		

		Matrix Temp3 = Temp2 + sigma_p;

		/*Temp2.print(cout, 1);*/
		
		Matrix Temp4 = Temp3.inverse(true);

		/*cout << "origin: " << endl;
		Temp3.print(cout,1);
		cout << "pinv: " << endl;
		Temp4.print(cout, 1);
		cout << "reOriginal " << endl;
		(Temp3*Temp4*Temp3).print(cout,2);*/

		double temp2 = sqrt(m_Pt*m_Ploss);
		Matrix Temp5 = Temp4*temp2;
		Matrix W = Temp5*m_H;//权重矩阵
		Matrix W_her = W.hermitian();
		double temp3 = sqrt(m_Ploss*m_Pt);
		Matrix Temp6 = W_her*temp3;
		Matrix D = Temp6*m_H;
		Matrix D_her = D.hermitian();
		Matrix Temp11 = D*D_her;
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
		for (int i = 0; i < (int)m_HInter.size(); i++) {
			double tmp1 = m_Pt;
			Matrix tmp2 = W_her*tmp1;
			Matrix H_inter_her = m_HInter[i].hermitian();
			Matrix tmp3 = tmp2*m_HInter[i];
			Matrix tmp4 = tmp3*H_inter_her;
			Matrix tmp5 = tmp4*W;
			Inter2 = Inter2 + tmp5;
		}

		Matrix Temp18 = Temp9 + Temp17 + Inter2;//SINR运算的分母

		Sinr[relativeSubCarrierIdx] = Temp13[0][0] / Temp18[0][0];
	}

	/******互信息方法求有效信噪比Sinreff*****/
	double sum_MI = 0, ave_MI = 0;
	double Sinreff = 0;

	for (int i = 0; i < Sinr.col; i++) {
		Sinr[i] = 10 * log10(Complex::abs(Sinr[i]));
	}

	if (m_Mol == 2) {
		for (int k = 0; k < m_SubCarrierNum; k++) {
			sum_MI = sum_MI + getMutualInformation(m_QPSK_MI,(int)ceil(Complex::abs(Sinr[k]) * 2 + 40 - 0.5));
		}
		ave_MI = sum_MI / m_SubCarrierNum;

		int SNRIndex = closest(m_QPSK_MI, ave_MI);
		Sinreff = 0.5*(SNRIndex - 40);
	}else if (m_Mol == 4) {
		for (int k = 0; k < m_SubCarrierNum; k++) {
			sum_MI = sum_MI + getMutualInformation(m_QAM_MI16,(int)ceil(Complex::abs(Sinr[k]) * 2 + 40 - 0.5));
		}
		ave_MI = sum_MI / m_SubCarrierNum;

		int SNRIndex = closest(m_QAM_MI16, ave_MI);
		Sinreff = 0.5*(SNRIndex - 40);
	}else if (m_Mol == 6) {
		for (int k = 0; k < m_SubCarrierNum; k++) {
			sum_MI = sum_MI + getMutualInformation(m_QAM_MI64,(int)ceil(Complex::abs(Sinr[k]) * 2 + 40 - 0.5));
		}
		ave_MI = sum_MI / m_SubCarrierNum;

		int SNRIndex = closest(m_QAM_MI64, ave_MI);
		Sinreff = 0.5*(SNRIndex - 40);
	}
	else {
		throw Exp("m_Mol Error");
	}

	int MCS = 0;
	MCS = searchMCSLevelTable(Sinreff);
	g_FileTemp << "MCS: " << MCS << endl;
}


void WT_B::testCloest() {
	default_random_engine e;
	uniform_real_distribution<double> u(0, 1);
	cout << "Inside testCloest()" << endl;

	for (int i = 0; i < 100000; i++) {
		double d = u(e);
		int index1 = closest(m_QAM_MI16, d);
		int index2 = closest2(m_QAM_MI16, d);
		if (index1 != index2) {

			cout << "index1: " << index1 << endl;
			cout << "index2: " << index2 << endl;
			if (m_QAM_MI16[index1] != m_QAM_MI16[index2]) {
				cout << "d: " << d << endl;
				cout << m_QAM_MI16[index1] << " , " << m_QAM_MI16[index2] << endl;
				cout << abs(d - m_QAM_MI16[index1]) << " , " << abs(d - m_QAM_MI16[index2]) << endl;
				throw Exp("hehe");
			}
			    
		}
	}
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


void WT_B::configuration(int VeUEId){
	m_Nr = m_VeUEAry[VeUEId].m_Nr;
	m_Nt = m_VeUEAry[VeUEId].m_Nt;
	m_Mol = m_VeUEAry[VeUEId].m_Mol;
	m_Ploss = m_VeUEAry[VeUEId].m_Pl;
	m_Pt = pow(10,-4.7);//23dbm-70dbm
	m_Sigma = pow(10,-17.4);

	m_PlossInter = m_VeUEAry[VeUEId].m_interPl;
}




Matrix WT_B::readH(int VeUEIdx,int subCarrierIdx) {
	Matrix res(m_Nr, m_Nt);
	for (int row = 0; row < m_Nr; row++) {
		for (int col = 0; col < m_Nt; col++) {
			res[row][col] = Complex(m_VeUEAry[VeUEIdx].m_H[row * subCarrierIdx], m_VeUEAry[VeUEIdx].m_H[row * subCarrierIdx + 1]);
		}
	}
	return res;
}


std::vector<Matrix> WT_B::readInterH(int VeUEIdx, int subCarrierIdx) {
	vector<Matrix> res;
	for (int interVeUEIdx : m_VeUEAry[VeUEIdx].m_interUEArray) {
		Matrix m(m_Nr, m_Nt);
		for (int row = 0; row < m_Nr; row++) {
			for (int col = 0; col < m_Nt; col++) {
				m[row][col] = Complex(m_VeUEAry[VeUEIdx].m_interH[interVeUEIdx*row*subCarrierIdx], m_VeUEAry[VeUEIdx].m_interH[interVeUEIdx*row*subCarrierIdx + 1]);
			}
		}
		res.push_back(m);
	}
	return res;
}


/*****查找MCS等级曲线*****/
int WT_B::searchMCSLevelTable(double SINR) {
	//double SINR_dB = 10 * log10(SINR);
	if (SINR > -10 && SINR < 30) {
		return m_MCSLevelTable[static_cast<int>(ceil((SINR +10)*1000))];
	}
	throw Exp("SINR越界");
}


int WT_B::closest(std::vector<double> v, double target) {
	int leftIndex = 0;
	int rightIndex = static_cast<int>(v.size() - 1);
	double leftDiff = v[leftIndex] - target;
	double rightDiff = v[rightIndex] - target;
	
	while (leftIndex <= rightIndex) {
		if (rightDiff <= 0) return rightIndex;//???
		if (leftDiff >= 0) return leftIndex;//???

		int midIndex = leftIndex + ((rightIndex - leftIndex) >> 1);
		double midDiff = v[midIndex] - target;
		if (midDiff == 0) return midIndex;
		else if (midDiff < 0) {
			leftIndex = midIndex + 1;
			leftDiff = v[leftIndex] - target;
			if (abs(midDiff) < abs(leftDiff)) return midIndex;
		}
		else {
			rightIndex = midIndex - 1;
			rightDiff = v[rightIndex] - target;
			if (abs(midDiff) < abs(rightDiff)) return midIndex;
		}
	}
	return abs(v[leftDiff] - target) < abs(v[leftDiff - 1] - target) ? leftIndex : leftIndex - 1;//???
}


int WT_B::closest2(std::vector<double> v, double target) {
	int res = -1;
	double minimum = (numeric_limits<double>::max)();
	for (int i = 0; i < static_cast<int>(v.size()); i++) {
		if (abs(v[i] - target) < minimum) {
			minimum = abs(v[i] - target);
			res = i;
		}
	}
	return res;
}



double WT_B::getMutualInformation(std::vector<double> v, int dex) {
	if (dex < 0) return v[0];
	if (dex >= (int)v.size()) return v[v.size() - 1];
	return v[dex];
}


