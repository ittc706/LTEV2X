#include<iomanip>
#include<iostream>
#include<tuple>
#include"Utility.h"

using namespace std;

int *Log::TTI = nullptr;

void Log::log(std::ofstream &out, std::string TAG, std::string msg) {
	out << "[ TTI = " << left << setw(3) << *TTI << "]";
	out << "    " << left << setw(8) << TAG;
	out << "    " << msg << endl;
}


vector<int> Function::getVector(int size) {
	return vector<int>(size, 0);
}

vector<vector<int>> Function::getVectorDim2(int size, int group) {
	srand((unsigned)time(NULL));//iomanip
	vector<vector<int>> v(group);
	int remain = size;
	for (int i = 0;i < group - 1;i++) {
		int curSize = std::rand() % (remain - (group - i) + 1) + 1;
		v[i] = vector<int>(curSize, 0);
		remain -= curSize;
	}
	v[group - 1] = vector<int>(remain, 0);
	return v;
}


void Function::print1DimVector(std::vector<int>&v) {
	for (int t : v)
		cout << t << ", ";
	cout << endl;
}


void Function::printVectorTuple(const vector<tuple<int, int, int>>&v) {
	for (const tuple<int, int, int> &t : v) {
		cout << "Interval£º[ " << get<0>(t) << " , " << get<1>(t) << " ]  , length: " << get<2>(t) << endl;
	}
}


vector<int> Function::makeEqualInterverSequence(int begin, int interval, int num) {
	vector<int> lst;
	int cur = begin;
	for (int i = 0; i < num; i++) {
		lst.push_back(cur);
		cur += interval;
	}
	return lst;
}


list<int> Function::makeContinuousSequence(int begin, int end) {
	list<int> res;
	for (int i = begin; i <= end; i++)
		res.push_back(i);
	return res;
}

int Function::sumArray(const int(&ary)[cRSU::s_DRAPatternTypeNum]) {
	int res = 0;
	for (int i : ary)
		res += i;
	return res;
}