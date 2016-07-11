#include "Global.h"

using namespace std;

ofstream g_OutDRAScheduleInfo("output\\DRAScheduleInfo.txt");
ofstream g_OutDRAProcessInfo("output\\DRAProcessInfo.txt");
ofstream g_OutClasterPerformInfo("output\\ClasterPerformInfo.txt");
ofstream g_OutEventListInfo("output\\EventListInfo.txt");

int max(int t1, int t2) {
	return t1 < t2 ? t2 : t1;
}