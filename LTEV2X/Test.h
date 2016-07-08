#pragma once
#include<vector>
#include<tuple>

class Function {
public :
	static std::vector<std::vector<int>> getVectorDim2(int size, int group);


	static std::vector<int> getVector(int size); 

	static void print1DimVector(std::vector<int>&v); 

	static void printVectorTuple(const std::vector<std::tuple<int, int, int>>&v);

};