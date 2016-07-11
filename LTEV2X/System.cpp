#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"

using namespace std;

void cSystem::process() {

	/*参数配置*/
	configure();

	/*仿真初始化*/
	initialization();

	for (int count = 0;count < m_NTTI;count++) {
		g_OutDRAScheduleInfo << "\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=Absolute TTI: " << m_TTI << "  ,  Relative TTI: " << count << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
		DRASchedule();
		m_TTI++;
	}
}