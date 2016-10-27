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


WT_B::WT_B(VeUE* systemVeUEAry):WT_Basic(systemVeUEAry){
	initialize();
}


std::tuple<ModulationType, int, double> WT_B::SINRCalculate(int VeUEId,int subCarrierIdxStart,int subCarrierIdxEnd, MessageType messageType) {
	//配置本次函数调用的参数
	configuration(VeUEId, messageType);

	//子载波数量
	int m_SubCarrierNum = subCarrierIdxEnd - subCarrierIdxStart + 1;

	/*****求每个子载波上的信噪比****/
	RowVector Sinr(m_SubCarrierNum);//每个子载波上的信噪比，维度为Nt的向量
	for (int subCarrierIdx = subCarrierIdxStart; subCarrierIdx <= subCarrierIdxEnd; subCarrierIdx++) {
		
		int relativeSubCarrierIdx = subCarrierIdx - subCarrierIdxStart;//相对的子载波下标

		m_H=readH(VeUEId, subCarrierIdx);//读入当前子载波的信道响应矩阵
		m_HInterference = readInterferenceH(VeUEId, subCarrierIdx, messageType);//读入当前子载波干扰相应矩阵数组


		/* 下面开始计算W */

		Matrix HHermit(m_Nt, m_Nr);
		HHermit = m_H.hermitian();//求信道矩阵的hermitian

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
			denominatorRight = denominatorRight + m_Pt*WHer*m_HInterference[i] * m_HInterference[i].hermitian()*W;
		}

		Matrix denominator = denominatorLeft + denominatorMiddle + denominatorRight;//SINR运算的分母


		Sinr[relativeSubCarrierIdx] = molecular[0][0] / denominator[0][0];
	}

	/******互信息方法求有效信噪比Sinreff*****/
	double sum_MI = 0, ave_MI = 0;
	double Sinreff = 0;

	for (int i = 0; i < Sinr.col; i++) {
		Sinr[i] = 10 * log10(Complex::abs(Sinr[i]));
	}
	//Sinr.print();
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

	int MCS = searchMCSLevelTable(Sinreff);
	g_FileTemp << "MCS: " << MCS << endl;
	return MCS2ModulationAndRate(MCS);
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
	std::ifstream in("WT\\MCSLevelTable.md");
	istream_iterator<int> inIter(in), eof;
	m_MCSLevelTable.assign(inIter, eof);
	in.close();


	//读入信噪比和互信息的对应表，m_mol=2是QPSK，m_mol=4是16QAM，m_mol=6=64QAM
	//维度是1*95

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


void WT_B::configuration(int VeUEId, MessageType messageType){
	m_Nr = m_VeUEAry[VeUEId].m_GTAT->m_Nr;
	m_Nt = m_VeUEAry[VeUEId].m_GTAT->m_Nt;
	m_Mol = m_VeUEAry[VeUEId].m_RRM->m_PreModulation[messageType];
	m_Ploss = m_VeUEAry[VeUEId].m_GTAT->m_Ploss;
	m_Pt = pow(10,-4.7);//-17dbm-70dbm
	m_Sigma = pow(10,-17.4);

	m_PlossInterference = m_VeUEAry[VeUEId].m_GTAT->m_InterferencePloss[messageType];
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


std::vector<Matrix> WT_B::readInterferenceH(int VeUEIdx, int subCarrierIdx, MessageType messageType) {
	vector<Matrix> res;
	for (int iter = 0; iter < m_VeUEAry[VeUEIdx].m_RRM->m_InterferenceVeUENum[messageType];iter++) {
		int interVeUEIdx = m_VeUEAry[VeUEIdx].m_RRM->m_InterferenceVeUEVec[messageType][iter];
		Matrix m(m_Nr, m_Nt);
		for (int row = 0; row < m_Nr; row++) {
			for (int col = 0; col < m_Nt; col++) {
				m[row][col] = Complex(m_VeUEAry[VeUEIdx].m_GTAT->m_InterferenceH[messageType][iter*row*subCarrierIdx], m_VeUEAry[VeUEIdx].m_GTAT->m_InterferenceH[messageType][iter*row*subCarrierIdx + 1]);
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


tuple<ModulationType, int, double> WT_B::MCS2ModulationAndRate(int MCSLevel) {
	//目前对应的是LTE上行的0-27对应于这里的1-28
	switch (MCSLevel) {
	case 1:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.0764);
	case 2:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.1250);
	case 3:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.1528);
	case 4:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.1806);
	case 5:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.2222);
	case 6:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.2847);
	case 7:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.3403);
	case 8:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.4097);
	case 9:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.4653);
	case 10:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.5347);
	case 11:
		return tuple<ModulationType, int, double>(QPSK, 1, 0.5903);
	case 12:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.2951);
	case 13:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.3368);
	case 14:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.3924);
	case 15:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.4340);
	case 16:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.4896);
	case 17:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.5313);
	case 18:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.5590);
	case 19:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.6146);
	case 20:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.6701);
	case 21:
		return tuple<ModulationType, int, double>(_16QAM, 2, 0.7535);
	case 22:
		return tuple<ModulationType, int, double>(_64QAM, 3, 0.5023);
	case 23:
		return tuple<ModulationType, int, double>(_64QAM, 3, 0.5394);
	case 24:
		return tuple<ModulationType, int, double>(_64QAM, 3, 0.5764);
	case 25:
		return tuple<ModulationType, int, double>(_64QAM, 3, 0.6227);
	case 26:
		return tuple<ModulationType, int, double>(_64QAM, 3, 0.6597);
	case 27:
		return tuple<ModulationType, int, double>(_64QAM, 3, 0.6968);
	case 28:
		return tuple<ModulationType, int, double>(_64QAM, 3, 0.7338);
	default:
		throw Exp("MCSLevel is Wrong!");
	}
}