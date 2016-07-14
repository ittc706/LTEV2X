#pragma once
#include<string>
#include<vector>
#include<list>
#include<fstream>
#include"RSU.h"

class Log {
public:
	static int * TTI;

	static void log(std::ofstream &out, std::string TAG, std::string msg);
};


class Function {
public:
	static std::vector<std::vector<int>> getVectorDim2(int size, int group);

	static std::vector<int> getVector(int size);

	static void print1DimVector(std::vector<int>&v);

	static void printVectorTuple(const std::vector<std::tuple<int, int, int>>&v);

	static std::vector<int> makeEqualInterverSequence(int begin, int interval, int num);

	static std::list<int> makeContinuousSequence(int begin, int end);

	static int sumArray(const int(&ary)[cRSU::s_DRAPatternTypeNum]);
};