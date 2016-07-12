#pragma once
#include<string>
#include<fstream>

class Log {
public:
	static int * ATTI;
	static int * STTI;

	static std::ofstream out;
    static void log(std::string TAG,std::string msg);
};