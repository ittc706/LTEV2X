#include<vector>
#include<iomanip>
#include<iostream>
#include<sstream>
#include<math.h>
#include"VUE.h"
#include"RSU.h"
#include"Function.h"

using namespace std;





void VeUE::initializeTMC() {
	m_TMC = new TMC();
}


VeUE::~VeUE() {
	Delete::safeDelete(m_TMC);
}



