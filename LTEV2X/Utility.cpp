#include"Utility.h"


using namespace std;

vector<int> makeContinuousSequence(int begin, int end) {
	vector<int> res;
	for (int i = begin; i <= end; i++)
		res.push_back(i);
	return res;
}