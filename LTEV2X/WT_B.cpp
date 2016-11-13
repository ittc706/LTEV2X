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
#include<iostream>
#include<math.h>
#include"WT_B.h"



using namespace std;

default_random_engine WT_B::s_Engine(0);

WT_B::WT_B(Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry) :WT_Basic(t_Config, t_RSUAry, t_VeUEAry) {}

WT_B::WT_B(const WT_B& t_WT_B) : WT_Basic(t_WT_B.m_Config, t_WT_B.m_RSUAry, t_WT_B.m_VeUEAry) {}


void WT_B::initialize() {
	//初始化VeUE的该模块参数部分
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId].initializeWT();
	}

	//初始化RSU的该模块参数部分
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId].initializeWT();
	}
}


WT_Basic* WT_B::getCopy() {
	return new WT_B(*this);
}


double WT_B::SINRCalculateMRC(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) {
	//配置本次函数调用的参数
	configuration(t_VeUEId, t_PatternIdx);

	//子载波数量
	int m_SubCarrierNum = t_SubCarrierIdxEnd - t_SubCarrierIdxStart + 1;

	///*****求每个子载波上的信噪比****/
	//RowVector Sinr(m_SubCarrierNum);//每个子载波上的信噪比，维度为Nt的向量
	//for (int subCarrierIdx = subCarrierIdxStart; subCarrierIdx <= subCarrierIdxEnd; subCarrierIdx++) {
	//	int relativeSubCarrierIdx = subCarrierIdx - subCarrierIdxStart;//相对的子载波下标

	//	m_H = readH(VeUEId, subCarrierIdx);//读入当前子载波的信道响应矩阵
	//	m_HInterference = readInterferenceH(VeUEId, subCarrierIdx, patternIdx);//读入当前子载波干扰相应矩阵数组

	//}
	return 10;
}


double WT_B::SINRCalculateMMSE(int t_VeUEId,int t_SubCarrierIdxStart,int t_SubCarrierIdxEnd, int t_PatternIdx) {
	//配置本次函数调用的参数
	configuration(t_VeUEId, t_PatternIdx);

	//子载波数量
	int m_SubCarrierNum = t_SubCarrierIdxEnd - t_SubCarrierIdxStart + 1;

	/*****求每个子载波上的信噪比****/
	RowVector SINRPerSubCarrier(m_SubCarrierNum);//每个子载波上的信噪比，维度为Nt的向量
	for (int subCarrierIdx = t_SubCarrierIdxStart; subCarrierIdx <= t_SubCarrierIdxEnd; subCarrierIdx++) {
		
		int relativeSubCarrierIdx = subCarrierIdx - t_SubCarrierIdxStart;//相对的子载波下标

		m_H=readH(t_VeUEId, subCarrierIdx);//读入当前子载波的信道响应矩阵
		m_HInterference = readInterferenceH(t_VeUEId, subCarrierIdx, t_PatternIdx);//读入当前子载波干扰相应矩阵数组

		/*if (m_VeUEAry[t_VeUEId].m_RRM->m_InterferenceVeUENum[t_PatternIdx] != 0) {
			m_H.print();
			cout << "干扰矩阵： ";
			for (auto &c : m_HInterference) {
				c.print();
			}
			cout << endl;
		}*/


		/* 下面开始计算W */

		Matrix HHermit = m_H.hermitian();//求信道矩阵的hermitian

		Matrix inverseExpLeft = m_Pt*m_Ploss*m_H * HHermit;//求逆表达式左边那项

		//计算干扰项
		Matrix Interference1(m_Nr, m_Nr);

		for (int i = 0; i < (int)m_HInterference.size(); i++) {
			Interference1 = Interference1 + m_Pt*m_PlossInterference[i] * m_HInterference[i] * m_HInterference[i].hermitian();
		}

		//求逆表达式右边那项
		Matrix inverseExpRight = m_Sigma*Matrix::eye(m_Nr) + Interference1;//sigma上带曲线
		
		Matrix W = (inverseExpLeft + inverseExpRight).inverse(true)*sqrt(m_Pt*m_Ploss)*m_H;//权重矩阵


		/* 下面开始计算D */
		//先计算分子
		Matrix WHer = W.hermitian();
		Matrix D = sqrt(m_Ploss*m_Pt)*WHer*m_H;
		Matrix DHer = D.hermitian();
		Matrix molecular = D*DHer;//SINR运算的分子，1*1的矩阵
		

		//然后计算分母
		Matrix denominatorLeft = WHer*W*m_Sigma;//SINR运算的分母中的第一项
		Matrix Iself = WHer*m_H*sqrt(m_Pt*m_Ploss) - D;
		Matrix IselfHer = Iself.hermitian();
		Matrix denominatorMiddle = Iself*IselfHer; //SINR运算的分母中的第二项

		/*以下计算公式中的干扰项,即公式中的第三项*/
		Matrix denominatorRight(m_Nt, m_Nt);
		for (int i = 0; i < (int)m_HInterference.size(); i++) {
			denominatorRight = denominatorRight + m_Pt*m_PlossInterference[i]*WHer*m_HInterference[i] * m_HInterference[i].hermitian()*W;
		}

		Matrix denominator = denominatorLeft + denominatorMiddle + denominatorRight;//SINR运算的分母


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




