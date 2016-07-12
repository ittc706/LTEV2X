#pragma once
#include<string>
#include<vector>
#include<fstream>

class Log {
public:
	static int * ATTI;
	static int * STTI;

	static std::ofstream out;
    static void log(std::string TAG,std::string msg);
};


class Function {
public:
	static std::vector<std::vector<int>> getVectorDim2(int size, int group);


	static std::vector<int> getVector(int size);

	static void print1DimVector(std::vector<int>&v);

	static void printVectorTuple(const std::vector<std::tuple<int, int, int>>&v);


	static std::vector<int> makeEqualInterverSequence(int begin, int interval, int num);

};