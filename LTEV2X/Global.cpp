#include "Global.h"

using namespace std;

ofstream g_OutDRAScheduleInfo("DRAOutput\\DRAScheduleInfo.txt");
ofstream g_OutDRAProcessInfo("DRAOutput\\DRAProcessInfo.txt");
ofstream g_OutClasterPerformInfo("DRAOutput\\ClasterPerformInfo.txt");
ofstream g_OutEventListInfo("DRAOutput\\EventListInfo.txt");


int max(int t1, int t2) {
	return t1 < t2 ? t2 : t1;
}