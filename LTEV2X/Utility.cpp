#include<iomanip>
#include"Utility.h"

using namespace std;

int *Log::ATTI = nullptr;
int *Log::STTI = nullptr;
ofstream Log::out= ofstream("output\\LogInfo.txt");

void Log::log(std::string TAG, std::string msg) {
	out << "[ ATTI = " << left << setw(4) << *ATTI << " , RTTI = " << left << setw(4) << *ATTI - *STTI << " ] ";
	out << "    " << left << setw(8) << TAG;
	out << "    " << msg << endl;
}