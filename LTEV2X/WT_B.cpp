/*
* =====================================================================================
*
*       Filename:  WT_B.cpp
*
*    Description:  WTģ��
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
	//���ñ��κ������õĲ���
	configuration(VeUEId);

	//���ز�����
	int m_SubCarrierNum = subCarrierIdxEnd - subCarrierIdxStart + 1;

	/*****��ÿ�����ز��ϵ������****/
	RowVector Sinr(m_SubCarrierNum);//ÿ�����ز��ϵ�����ȣ�ά��ΪNt������
	for (int subCarrierIdx = subCarrierIdxStart; subCarrierIdx <= subCarrierIdxEnd; subCarrierIdx++) {
		
		int relativeSubCarrierIdx = subCarrierIdx - subCarrierIdxStart;//��Ե����ز��±�

		m_H=readH(VeUEId, subCarrierIdx);//���뵱ǰ���ز����ŵ���Ӧ����
		//m_HInter = readInterH(VeUEId, subCarrierIdx);//���뵱ǰ���ز�������Ӧ��������


		/* ���濪ʼ����W */

		Matrix HHermit(m_Nt, m_Nr);
		HHermit = m_H.hermitian();//���ŵ������hermitian

		Matrix inverseExpLeft = m_Pt*m_Ploss*m_H * HHermit;//������ʽ�������

		//���������
		Matrix Inter1(m_Nr, m_Nr);

		/*for (int i = 0; i < (int)m_HInter.size(); i++) {
			Inter1 = Inter1 + m_Pt*m_PlossInter[i] * m_HInter[i] * m_HInter[i].hermitian();
		}*/

		//������ʽ�ұ�����
		Matrix inverseExpRight = m_Sigma*Matrix::eye(m_Nr) + Inter1;//sigma�ϴ�����
		
		Matrix W = (inverseExpLeft + inverseExpRight).inverse(true)*sqrt(m_Pt*m_Ploss)*m_H;//Ȩ�ؾ���


		/* ���濪ʼ����D */
		//�ȼ������
		Matrix WHer = W.hermitian();
		Matrix D = sqrt(m_Ploss*m_Pt)*WHer*m_H;
		Matrix DHer = D.hermitian();
		Matrix molecular = D*DHer;//SINR����ķ��ӣ�1*1�ľ���
		

		//Ȼ������ĸ
		Matrix denominatorLeft = WHer*W*m_Sigma;//SINR����ķ�ĸ�еĵ�һ��
		Matrix Iself = WHer*m_H*sqrt(m_Pt*m_Ploss) - D;
		Matrix IselfHer = Iself.hermitian();
		Matrix denominatorMiddle = Iself*IselfHer; //SINR����ķ�ĸ�еĵڶ���

		///*���¼��㹫ʽ�еĸ�����,����ʽ�еĵ�����*/
		//Matrix denominatorRight(m_Nr, m_Nr);
		//for (int i = 0; i < (int)m_HInter.size(); i++) {
		//	denominatorRight = denominatorRight + m_Pt*WHer*m_HInter[i] * m_HInter[i].hermitian()*W;
		//}

		//Matrix denominator = denominatorLeft + denominatorMiddle + denominatorRight;//SINR����ķ�ĸ
		Matrix denominator = denominatorLeft + denominatorMiddle;//SINR����ķ�ĸ


		Sinr[relativeSubCarrierIdx] = molecular[0][0] / denominator[0][0];
	}

	/******����Ϣ��������Ч�����Sinreff*****/
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
	//��ȡm_MCSLevelTable
	std::ifstream in("WT\\MCSLevelTable.md");
	istream_iterator<int> inIter(in), eof;
	m_MCSLevelTable.assign(inIter, eof);
	in.close();


	//��������Ⱥͻ���Ϣ�Ķ�Ӧ��m_mol=2��QPSK��m_mol=4��16QAM��m_mol=6=64QAM
	//ά����1*95

	in.open("WT\\QPSK_MI.md");
	istream_iterator<double> inIter2(in), eof2;
	m_QPSK_MI.assign(inIter2, eof2);
	in.close();

	in.open("WT\\QAM_MI16.md");
	inIter2 = istream_iterator<double>(in);
	m_QAM_MI16.assign(inIter2, eof2);
	in.close();

	in.open("WT\\QAM_MI64.md");
	inIter2 = istream_iterator<double>(in);
	m_QAM_MI64.assign(inIter2, eof2);
	in.close();
}


void WT_B::configuration(int VeUEId){
	m_Nr = m_VeUEAry[VeUEId].m_GTAT->m_Nr;
	m_Nt = m_VeUEAry[VeUEId].m_GTAT->m_Nt;
	m_Mol = m_VeUEAry[VeUEId].m_RRM->m_PreModulation;
	m_Ploss = m_VeUEAry[VeUEId].m_GTAT->m_Ploss;
	m_Pt = pow(10,-4.7);//23dbm-70dbm
	m_Sigma = pow(10,-17.4);

	m_PlossInter = m_VeUEAry[VeUEId].m_GTAT->m_InterferencePloss;
}




Matrix WT_B::readH(int VeUEIdx,int subCarrierIdx) {
	Matrix res(m_Nr, m_Nt);
	for (int row = 0; row < m_Nr; row++) {
		for (int col = 0; col < m_Nt; col++) {
			res[row][col] = Complex(m_VeUEAry[VeUEIdx].m_GTAT->m_H[row * subCarrierIdx], m_VeUEAry[VeUEIdx].m_GTAT->m_H[row * subCarrierIdx + 1]);
		}
	}
	return res;
}


std::vector<Matrix> WT_B::readInterH(int VeUEIdx, int subCarrierIdx) {
	vector<Matrix> res;
	for (int interVeUEIdx : m_VeUEAry[VeUEIdx].m_RRM->m_InterVeUEVec) {
		Matrix m(m_Nr, m_Nt);
		for (int row = 0; row < m_Nr; row++) {
			for (int col = 0; col < m_Nt; col++) {
				m[row][col] = Complex(m_VeUEAry[VeUEIdx].m_GTAT->m_InterferenceH[interVeUEIdx*row*subCarrierIdx], m_VeUEAry[VeUEIdx].m_GTAT->m_InterferenceH[interVeUEIdx*row*subCarrierIdx + 1]);
			}
		}
		res.push_back(m);
	}
	return res;
}


/*****����MCS�ȼ�����*****/
int WT_B::searchMCSLevelTable(double SINR) {
	//double SINR_dB = 10 * log10(SINR);
	if (SINR > -10 && SINR < 30) {
		return m_MCSLevelTable[static_cast<int>(ceil((SINR +10)*1000))];
	}
	throw Exp("SINRԽ��");
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
	return abs(v[leftIndex] - target) < abs(v[leftIndex - 1] - target) ? leftIndex : leftIndex - 1;//???
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


