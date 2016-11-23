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
#include"System.h"



using namespace std;

default_random_engine WT_B::s_Engine(0);

WT_B::WT_B(System* t_Context) :WT(t_Context) {}


WT_B::WT_B(const WT_B& t_WT_B) : WT(t_WT_B.m_Context) {
	m_QPSK_MI = t_WT_B.m_QPSK_MI;
	m_VeUEAry = t_WT_B.m_VeUEAry;
	m_RSUAry = t_WT_B.m_RSUAry;
	m_SINRMode = t_WT_B.m_SINRMode;
}


void WT_B::initialize() {
	//读入信噪比和互信息的对应表(QPSK),维度是1*95
	ifstream in;
	if (getContext()->m_Config.platform == Windows) {
		in.open("WT\\QPSK_MI.md");
	}
	else{
		in.open("WT/QPSK_MI.md");
	}
	m_QPSK_MI = make_shared<vector<double>>();
	istream_iterator<double> inIter2(in), eof2;
	m_QPSK_MI->assign(inIter2, eof2);
	in.close();

	//初始化VeUE的该模块参数部分
	m_VeUEAry = new WT_VeUE*[getContext()->m_Config.VeUENum];
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId] = new WT_VeUE();
	}

	//初始化RSU的该模块参数部分
	m_RSUAry = new WT_RSU*[getContext()->m_Config.RSUNum];
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId] = new WT_RSU();
	}
}


WT* WT_B::getCopy() {
	return new WT_B(*this);
}

void WT_B::freeCopy() {
	m_VeUEAry = nullptr;
	m_RSUAry = nullptr;
}

double WT_B::SINRCalculate(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) {
	switch (m_SINRMode) {
	case SINR_MRC:
		return SINRCalculateMRC(t_VeUEId, t_SubCarrierIdxStart, t_SubCarrierIdxEnd, t_PatternIdx);
	case SINR_MMSE:
		return SINRCalculateMMSE(t_VeUEId, t_SubCarrierIdxStart, t_SubCarrierIdxEnd, t_PatternIdx);
	default:
		throw Exp("wrong SINRMode");
	}
}



double WT_B::SINRCalculateMRC(int t_VeUEId, int t_SubCarrierIdxStart, int t_SubCarrierIdxEnd, int t_PatternIdx) {
	//子载波数量
	int subCarrierNum = t_SubCarrierIdxEnd - t_SubCarrierIdxStart + 1;

	//配置本次函数调用的参数
	configuration(t_VeUEId, t_PatternIdx, subCarrierNum);

	/*****求每个子载波上的信噪比****/
	vector<double> SINR(subCarrierNum);//每个子载波上的信噪比，维度为Nt的向量
	for (int subCarrierIdx = t_SubCarrierIdxStart; subCarrierIdx <= t_SubCarrierIdxEnd; subCarrierIdx++) {
		int relativeSubCarrierIdx = subCarrierIdx - t_SubCarrierIdxStart;//相对的子载波下标

		m_H = readH(t_VeUEId, subCarrierIdx);//读入当前子载波的信道响应矩阵
		m_HInterference = readInterferenceH(t_VeUEId, subCarrierIdx, t_PatternIdx);//读入当前子载波干扰相应矩阵数组

		double HSum1 = 0;
		for (int r = 0; r < m_Nr; r++) {
			HSum1 += Complex::abs(m_H[r][0])*Complex::abs(m_H[r][0]);
		}

		double molecule = m_Pt*m_Ploss*HSum1*HSum1;

		double HSum2 = 0;
		for (int j = 0; j < m_HInterference.size(); j++) {
			double weight = m_Pt*m_PlossInterference[j];
			Complex tmp(0,0);
			for (int r = 0; r < m_Nr; r++) {
				tmp += m_H[r][0] * m_HInterference[j][r][0];
			}
			HSum2 += weight * Complex::abs(tmp)*Complex::abs(tmp);
		}

		double denominator = HSum1*m_Sigma + HSum2;

		SINR[relativeSubCarrierIdx] = 10 * log10(molecule / denominator);
	}

	//互信息方法求有效信噪比Sinreff

	double sum_MI = 0, ave_MI = 0;
	double Sinreff = 0;

	for (int k = 0; k < subCarrierNum; k++) {
		sum_MI = sum_MI + getMutualInformation(*m_QPSK_MI, (int)ceil((SINR[k] + 20) * 2));
	}
	ave_MI = sum_MI / subCarrierNum;

	int SNRIndex = closest(*m_QPSK_MI, ave_MI);
	Sinreff = 0.5*(SNRIndex - 40);

	/*if (m_HInterference.size() > 0) {
		cout << "InterNum: " << m_HInterference.size() << endl;
		cout << "Ploss: " << m_Ploss << endl;
		cout << "InterPloss: [";
		for (auto tmpPloss : m_PlossInterference)
			cout << tmpPloss << " , ";
		cout << "]" << endl;
		cout << "SINR: " << Sinreff << endl;
		cout << endl;
	}*/

	return Sinreff;
}


double WT_B::SINRCalculateMMSE(int t_VeUEId,int t_SubCarrierIdxStart,int t_SubCarrierIdxEnd, int t_PatternIdx) {
	//子载波数量
	int subCarrierNum = t_SubCarrierIdxEnd - t_SubCarrierIdxStart + 1;

	//配置本次函数调用的参数
	configuration(t_VeUEId, t_PatternIdx, subCarrierNum);

	/*****求每个子载波上的信噪比****/
	vector<double> SINR(subCarrierNum);//每个子载波上的信噪比，维度为Nt的向量
	for (int subCarrierIdx = t_SubCarrierIdxStart; subCarrierIdx <= t_SubCarrierIdxEnd; subCarrierIdx++) {
		
		int relativeSubCarrierIdx = subCarrierIdx - t_SubCarrierIdxStart;//相对的子载波下标

		m_H=readH(t_VeUEId, subCarrierIdx);//读入当前子载波的信道响应矩阵
		m_HInterference = readInterferenceH(t_VeUEId, subCarrierIdx, t_PatternIdx);//读入当前子载波干扰相应矩阵数组

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


		SINR[relativeSubCarrierIdx] = 10*log10((molecular[0][0] / denominator[0][0]).real);
	}

	//互信息方法求有效信噪比Sinreff
	double sum_MI = 0, ave_MI = 0;
	double Sinreff = 0;

	for (int k = 0; k < subCarrierNum; k++) {
		sum_MI = sum_MI + getMutualInformation(*m_QPSK_MI, (int)ceil((SINR[k] + 20) * 2));
	}
	ave_MI = sum_MI / subCarrierNum;

	int SNRIndex = closest(*m_QPSK_MI, ave_MI);
	Sinreff = 0.5*(SNRIndex - 40);
	
	return Sinreff;
}




void WT_B::configuration(int t_VeUEId, int t_PatternIdx, int t_SubCarrierNum){
	m_Nr = m_VeUEAry[t_VeUEId]->getSystemPoint()->getGTTPoint()->m_Nr;
	m_Nt = m_VeUEAry[t_VeUEId]->getSystemPoint()->getGTTPoint()->m_Nt;
	m_Ploss = m_VeUEAry[t_VeUEId]->getSystemPoint()->getGTTPoint()->m_Ploss;
	m_Pt = pow(10, (23 - 10 * log10(t_SubCarrierNum * 15 * 1000))/10);
	m_Sigma = pow(10,-17.4);

	m_PlossInterference.clear();
	for (int interferenceVeUEId : m_VeUEAry[t_VeUEId]->getSystemPoint()->getRRMPoint()->m_InterferenceVeUEIdVec[t_PatternIdx]) {
		m_PlossInterference .push_back(m_VeUEAry[t_VeUEId]->getSystemPoint()->getGTTPoint()->m_InterferencePloss[interferenceVeUEId]);
	}
}




Matrix WT_B::readH(int t_VeUEId,int t_SubCarrierIdx) {
	Matrix res(m_Nr, m_Nt);
	for (int row = 0; row < m_Nr; row++) {
		for (int col = 0; col < m_Nt; col++) {
			res[row][col] = Complex(m_VeUEAry[t_VeUEId]->getSystemPoint()->getGTTPoint()->m_H[row * 2048 + t_SubCarrierIdx * 2], m_VeUEAry[t_VeUEId]->getSystemPoint()->getGTTPoint()->m_H[row * 2048 + t_SubCarrierIdx * 2 + 1]);
		}
	}
	return res;
}


vector<Matrix> WT_B::readInterferenceH(int t_VeUEId, int t_SubCarrierIdx, int t_PatternIdx) {
	vector<Matrix> res;
	for (int interferenceVeUEId : m_VeUEAry[t_VeUEId]->getSystemPoint()->getRRMPoint()->m_InterferenceVeUEIdVec[t_PatternIdx]) {
		Matrix m(m_Nr, m_Nt);
		for (int row = 0; row < m_Nr; row++) {
			for (int col = 0; col < m_Nt; col++) {
				m[row][col] = Complex(m_VeUEAry[t_VeUEId]->getSystemPoint()->getGTTPoint()->m_InterferenceH[interferenceVeUEId][row * 2048 + t_SubCarrierIdx * 2],
					m_VeUEAry[t_VeUEId]->getSystemPoint()->getGTTPoint()->m_InterferenceH[interferenceVeUEId][row * 2048 + t_SubCarrierIdx * 2 + 1]);
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


double WT_B::getMutualInformation(vector<double> t_Vec, int t_Index) {
	if (t_Index < 0) return t_Vec[0];
	if (t_Index >= (int)t_Vec.size()) return t_Vec[t_Vec.size() - 1];
	return t_Vec[t_Index];
}