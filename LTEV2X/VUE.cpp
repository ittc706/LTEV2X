#include<vector>
#include<iomanip>
#include<math.h>
#include"VUE.h"

using namespace std;


bool cVeUE::isScheduled(eLinkType link) {
	if (link == DOWNLINK)
		return this->m_IsScheduledDL;
	else
		return this->m_IsScheduledUL;
}



int cVeUE::RBSelectBasedOnP2(const vector<int> &v) {
	int size = v.size();
	return v[rand() % size];
}
