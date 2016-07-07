#include<vector>
#include<iostream>
#include"Test.h"

using namespace std;

vector<int> Function::getVector(int size){
	return vector<int>(size, 0);
}

//template<typename T>
void Function::print1DimVector(std::vector<int>&v) {
	for (int t : v)
		cout << t << ", ";
	cout << endl;
}