#include<iomanip>
#include<fstream>
#include"System.h"

using namespace std;

extern ofstream g_OutDRAScheduleInfo;

void cSystem::process() {
	int TTINum = 100;//·ÂÕæTTIÊ±¼ä
	configure();
	for (int count = 0;count < TTINum;count++) {
		g_OutDRAScheduleInfo << "\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=Absolute TTI: " << m_TTI << "  ,  Relative TTI: " << count << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
		DRASchedule();
		m_TTI++;
	}
}