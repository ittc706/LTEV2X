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
#include<iostream>
#include<math.h>
#include"WT_B.h"



using namespace std;

default_random_engine WT_B::s_Engine(0);

WT_B::WT_B(Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry) :WT_Basic(t_Config, t_RSUAry, t_VeUEAry) {}

WT_B::WT_B(const WT_B& t_WT_B) : WT_Basic(t_WT_B.m_Config, t_WT_B.m_RSUAry, t_WT_B.m_VeUEAry) {}


void WT_B::initialize() {
	//��ʼ��VeUE�ĸ�ģ���������
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeWT();
	}

	//��ʼ��RSU�ĸ�ģ���������
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeWT();
	}
}


WT_Basic* WT_B::getCopy() {
	return new WT_B(*this);
}


double WT_B::SINRCalculateMRC(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) {
	//���ñ��κ������õĲ���
	configuration(t_VeUEId, t_PatternIdx);

	//���ز�����
	int m_SubCarrierNum = t_SubCarrierIdxEnd - t_SubCarrierIdxStart + 1;

	///*****��ÿ�����ز��ϵ������****/
	//RowVector Sinr(m_SubCarrierNum);//ÿ�����ز��ϵ�����ȣ�ά��ΪNt������
	//for (int subCarrierIdx = subCarrierIdxStart; subCarrierIdx <= subCarrierIdxEnd; subCarrierIdx++) {
	//	int relativeSubCarrierIdx = subCarrierIdx - subCarrierIdxStart;//��Ե����ز��±�

	//	m_H = readH(VeUEId, subCarrierIdx);//���뵱ǰ���ز����ŵ���Ӧ����
	//	m_HInterference = readInterferenceH(VeUEId, subCarrierIdx, patternIdx);//���뵱ǰ���ز�������Ӧ��������

	//}
	return 10;
}


double WT_B::SINRCalculateMMSE(int t_VeUEId,int t_SubCarrierIdxStart,int t_SubCarrierIdxEnd, int t_PatternIdx) {
	//���ñ��κ������õĲ���
	configuration(t_VeUEId, t_PatternIdx);

	//���ز�����
	int m_SubCarrierNum = t_SubCarrierIdxEnd - t_SubCarrierIdxStart + 1;

	/*****��ÿ�����ز��ϵ������****/
	RowVector SINRPerSubCarrier(m_SubCarrierNum);//ÿ�����ز��ϵ�����ȣ�ά��ΪNt������
	for (int subCarrierIdx = t_SubCarrierIdxStart; subCarrierIdx <= t_SubCarrierIdxEnd; subCarrierIdx++) {
		
		int relativeSubCarrierIdx = subCarrierIdx - t_SubCarrierIdxStart;//��Ե����ز��±�

		m_H=readH(t_VeUEId, subCarrierIdx);//���뵱ǰ���ز����ŵ���Ӧ����
		m_HInterference = readInterferenceH(t_VeUEId, subCarrierIdx, t_PatternIdx);//���뵱ǰ���ز�������Ӧ��������

		/*if (m_VeUEAry[t_VeUEId].m_RRM->m_InterferenceVeUENum[t_PatternIdx] != 0) {
			m_H.print();
			cout << "���ž��� ";
			for (auto &c : m_HInterference) {
				c.print();
			}
			cout << endl;
		}*/


		/* ���濪ʼ����W */

		Matrix HHermit = m_H.hermitian();//���ŵ������hermitian

		Matrix inverseExpLeft = m_Pt*m_Ploss*m_H * HHermit;//������ʽ�������

		//���������
		Matrix Interference1(m_Nr, m_Nr);

		for (int i = 0; i < (int)m_HInterference.size(); i++) {
			Interference1 = Interference1 + m_Pt*m_PlossInterference[i] * m_HInterference[i] * m_HInterference[i].hermitian();
		}

		//������ʽ�ұ�����
		Matrix inverseExpRight = m_Sigma*Matrix::eye(m_Nr) + Interference1;//sigma�ϴ�����
		
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

		/*���¼��㹫ʽ�еĸ�����,����ʽ�еĵ�����*/
		Matrix denominatorRight(m_Nt, m_Nt);
		for (int i = 0; i < (int)m_HInterference.size(); i++) {
			denominatorRight = denominatorRight + m_Pt*m_PlossInterference[i]*WHer*m_HInterference[i] * m_HInterference[i].hermitian()*W;
		}

		Matrix denominator = denominatorLeft + denominatorMiddle + denominatorRight;//SINR����ķ�ĸ


		SINRPerSubCarrier[relativeSubCarrierIdx] = molecular[0][0] / denominator[0][0];
	}

	for (int i = 0; i < SINRPerSubCarrier.col; i++) {
		SINRPerSubCarrier[i] = 10 * log10(Complex::abs(SINRPerSubCarrier[i]));
	}

	double averageSINR = 0;

	for (int i = 0; i < SINRPerSubCarrier.col; i++) {
		averageSINR += SINRPerSubCarrier[i].real;
	}
	
	return averageSINR;
}




void WT_B::configuration(int t_VeUEId, int t_PatternIdx){
	m_Nr = m_VeUEAry[t_VeUEId].m_GTT->m_Nr;
	m_Nt = m_VeUEAry[t_VeUEId].m_GTT->m_Nt;
	m_Mol = m_VeUEAry[t_VeUEId].m_RRM->m_ModulationType;
	m_Ploss = m_VeUEAry[t_VeUEId].m_GTT->m_Ploss;
	m_Pt = pow(10,-4.7);//-17dbm-70dbm
	m_Sigma = pow(10,-17.4);

	m_PlossInterference.clear();
	for (int interferenceVeUEId : m_VeUEAry[t_VeUEId].m_RRM->m_InterferenceVeUEIdVec[t_PatternIdx]) {
		m_PlossInterference .push_back(m_VeUEAry[t_VeUEId].m_GTT->m_InterferencePloss[interferenceVeUEId]);
	}
}




Matrix WT_B::readH(int t_VeUEIdx,int t_SubCarrierIdx) {
	Matrix res(m_Nr, m_Nt);
	for (int row = 0; row < m_Nr; row++) {
		for (int col = 0; col < m_Nt; col++) {
			res[row][col] = Complex(m_VeUEAry[t_VeUEIdx].m_GTT->m_H[row * 2048 + t_SubCarrierIdx * 2], m_VeUEAry[t_VeUEIdx].m_GTT->m_H[row * 2048 + t_SubCarrierIdx * 2 + 1]);
		}
	}
	return res;
}


vector<Matrix> WT_B::readInterferenceH(int t_VeUEIdx, int t_SubCarrierIdx, int t_PatternIdx) {
	vector<Matrix> res;
	for (int interferenceVeUEId : m_VeUEAry[t_VeUEIdx].m_RRM->m_InterferenceVeUEIdVec[t_PatternIdx]) {
		Matrix m(m_Nr, m_Nt);
		for (int row = 0; row < m_Nr; row++) {
			for (int col = 0; col < m_Nt; col++) {
				m[row][col] = Complex(m_VeUEAry[t_VeUEIdx].m_GTT->m_InterferenceH[interferenceVeUEId][row * 2048 + t_SubCarrierIdx * 2],
					m_VeUEAry[t_VeUEIdx].m_GTT->m_InterferenceH[interferenceVeUEId][row * 2048 + t_SubCarrierIdx * 2 + 1]);
			}
		}
		res.push_back(m);
	}
	return res;
}




int WT_B::closest(vector<double> t_Vec, double t_Target) {
	int leftIndex = 0;
	int rightIndex = static_cast<int>(t_Vec.size() - 1);
	double leftDiff = t_Vec[leftIndex] - t_Target;
	double rightDiff = t_Vec[rightIndex] - t_Target;
	
	while (leftIndex <= rightIndex) {
		if (rightDiff <= 0) return rightIndex;//???
		if (leftDiff >= 0) return leftIndex;//???

		int midIndex = leftIndex + ((rightIndex - leftIndex) >> 1);
		double midDiff = t_Vec[midIndex] - t_Target;
		if (midDiff == 0) return midIndex;
		else if (midDiff < 0) {
			leftIndex = midIndex + 1;
			leftDiff = t_Vec[leftIndex] - t_Target;
			if (abs(midDiff) < abs(leftDiff)) return midIndex;
		}
		else {
			rightIndex = midIndex - 1;
			rightDiff = t_Vec[rightIndex] - t_Target;
			if (abs(midDiff) < abs(rightDiff)) return midIndex;
		}
	}
	return abs(t_Vec[leftIndex] - t_Target) < abs(t_Vec[leftIndex - 1] - t_Target) ? leftIndex : leftIndex - 1;//???
}




