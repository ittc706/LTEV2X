#pragma once
#include<fstream>
#include "WT.h"

class WT_B :public WT_Basic {
public:
	WT_B(int t_Nt, int t_Nr, double t_Pt, double t_SNR, double t_Ploss, std::ifstream& in);
	void loadH(std::ifstream& in);

	void SINRCalculate() override;


private:

};