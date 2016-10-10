#pragma once
//WT:Wireless transmission
#include<vector>
#include"Matrix.h"



class WT_Basic {
public:
	WT_Basic() {}

	virtual void SINRCalculate()=0;

};