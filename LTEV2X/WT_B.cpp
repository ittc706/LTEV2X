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


#include "WT_B.h"

WT_B::WT_B(int t_Nt, int t_Nr, double t_Pt, double t_SNR, double t_Ploss,std::ifstream& in) :
	WT_Basic(t_Nt, t_Nr, t_Pt, t_SNR, t_Ploss) {
	loadH(in);
}


void WT_B::loadH(std::ifstream& in) {
	//<UNDONE>
	//���ڶ�ȡ�ļ�����ʼ��m_H
}


void WT_B::SINRCalculate() {

}