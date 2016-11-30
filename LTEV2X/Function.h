#pragma once

#include<iostream>
#include<iomanip>
#include<vector>

class Delete {
public:
	/*
	* É¾³ýº¯Êý£¬É¾³ýºó»áÖÃ¿ÕÖ¸Õë
	*/
	template<typename T>
	static void safeDelete(T*&t_Point, bool t_IsArray = false) {
		if (t_Point != nullptr) {
			if (t_IsArray)
				delete[] t_Point;
			else
				delete t_Point;
			t_Point = nullptr;
		}
	}
};

class Print {
public:
	template<typename T>
	static void printVectorDim1(std::vector<T> t_Vector) {
		for (T& t : t_Vector)
			std::cout << std::left << std::setw(5) << t;
		std::cout << endl;
	}

	template<typename T>
	static void printVectorDim2(std::vector<std::vector<T>> t_Vector) {
		for (std::vector<T>& innerVector : t_Vector) {
			for (T& t : innerVector) {
				std::cout << std::left << std::setw(5) << t;
			}
			std::cout << endl;
		}
	}
};