#include<vector>
#include<iomanip>
#include<math.h>
#include"VUE.h"
#include"RSU.h"

using namespace std;

int cVeUE::m_VeUECount = 0;



int cVeUE::RBSelectBasedOnP2(const std::vector<int>(&curAvaliablePatternIdx)[cRSU::s_DRAPatternTypeNum], eMessageType messageType) {
	int size = curAvaliablePatternIdx[messageType].size();
	return curAvaliablePatternIdx[messageType][rand() % size];
}
