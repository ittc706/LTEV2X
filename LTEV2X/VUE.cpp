#include<vector>
#include<iomanip>
#include<math.h>
#include"VUE.h"

using namespace std;





int cVeUE::RBSelectBasedOnP2(const vector<int> &v) {
	int size = v.size();
	return v[rand() % size];
}
