#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"
#include"RSU.h"

using namespace std;

void cSystem::process() {

	//参数配置
	configure();

	//仿真初始化
	initialization();

	//打印事件链表
	writeEventListInfo(g_OutEventListInfo);

	//开始仿真
	for (int count = 0;count < m_NTTI;count++) {
		cout << "Current TTI = " << m_TTI << endl;
		if (count % m_Config.locationUpdateNTTI == 0)
			channelGeneration();
		DRASchedule();
		m_TTI++;
	}

	//打印事件日志信息
	writeEventLogInfo(g_OutEventLogInfo);

	//打印车辆地理位置更新日志信息
	writeVeUELocationUpdateLogInfo(g_OutVeUELocationUpdateLogInfo);

	//时延统计
	delayStatistics();
}

void cSystem::configure() {//系统仿真参数配置

	/*--------------------------------------------------------------
	*                 地理拓扑单元参数配置
	* -------------------------------------------------------------*/
	m_Config.eNBNum = c_eNBNumber;
	m_Config.RoadNum = c_roadNumber;
	m_Config.RSUNum = c_RSUNumber;//目前只表示UE RSU数
	m_Config.pupr = new int[m_Config.RoadNum];
	m_Config.VeUENum = 0;
	int Lambda = static_cast<int>((c_length + c_wide) * 2 * 3.6 / (2.5 * 15));
	srand((unsigned)time(NULL));
	for (unsigned short temp = 0; temp != m_Config.RoadNum; ++temp)
	{
		int k = 0;
		long double p = 1.0;
		long double l = exp(-Lambda);  /* 为了精度，才定义为long double的，exp(-Lambda)是接近0的小数*/
		while (p >= l)
		{
			double u = (float)(rand() % 10000)*0.0001f;
			p *= u;
			k++;
		}
		m_Config.pupr[temp] = k - 1;
		m_Config.VeUENum = m_Config.VeUENum + k - 1;
		//printf("%d\n",k-1);
	}
	m_Config.wxNum = 36;
	m_Config.wyNum = 62;
	m_Config.ueTopoNum = (m_Config.wxNum + m_Config.wyNum) * 2 - 4;
	m_Config.pueTopo = new float[m_Config.ueTopoNum * 2];//重合了4个
	float temp_x = -(float)c_wide / 2 + c_lane_wide;
	float temp_y = -(float)c_length / 2 + c_lane_wide;
	for (int temp = 0;temp != m_Config.ueTopoNum; ++temp)
	{
		if (temp>0 && temp <= 61) {
			if (temp == 60) temp_y += 6; else temp_y += 7;
		}
		else if (61<temp&&temp <= 96) {
			if (temp == 95) temp_x += 5; else temp_x += 7;
		}
		else if (96<temp&&temp <= 157) {
			if (temp == 156) temp_y -= 6; else temp_y -= 7;
		}
		else if (157<temp&&temp <= 192) {
			if (temp == 191) temp_x -= 5; else temp_x -= 7;
		}

		m_Config.pueTopo[temp * 2 + 0] = temp_x;
		m_Config.pueTopo[temp * 2 + 1] = temp_y;
		//printf("%f,%f\n",temp_x,temp_y);//之后输出到表格直接读入可以省时间
	}
	m_Config.fv = 15;//车速设定,km/h


	/*--------------------------------------------------------------
	*                 无线资源管理单元参数配置
	* -------------------------------------------------------------*/

	m_NTTI = 2000;//仿真TTI时间
	m_Config.periodicEventNTTI = 50;
	m_Config.emergencyLamda = 2;
	m_Config.locationUpdateNTTI = 100;

	//选择DRA模式
	m_DRAMode = P123;

	//事件链表容器
	m_EventTTIList = vector<list<int>>(m_NTTI);
}


void cSystem::initialization() {
	srand((unsigned)time(NULL));//iomanip
	m_TTI = 0;

	/*--------------------------------------------------------------
	*                      地理拓扑单元初始化
	* -------------------------------------------------------------*/
	m_eNBAry = new ceNB[m_Config.eNBNum];
	m_RoadAry = new cRoad[m_Config.RoadNum];
	m_VeUEAry = new cVeUE[m_Config.VeUENum];
	m_RSUAry = new cRSU[m_Config.RSUNum];

	sRoadConfigure roadConfigure;
	for (unsigned short temp = 0;temp != m_Config.RoadNum; ++temp)
	{
		roadConfigure.wRoadID = temp;
		if (temp % 2 == 0)
		{
			roadConfigure.weNBNum = 1;
			roadConfigure.peNB = m_eNBAry;
			roadConfigure.weNBOffset = temp / 2;
		}
		else
		{
			roadConfigure.weNBNum = 0;
		}

		m_RoadAry[temp].initialize(roadConfigure);
	}

	//       FILE *fp1;//建立一个文件操作指针
	//        fp1=fopen("eNB.txt","w+");//以追加的方式建立或打开1.txt，默认位置在你程序的目录下面
	//        for(int eNBIdx=0;eNBIdx!=c_eNBNumber;++eNBIdx)
	//	      fprintf(fp1,"%f\n%f
	sRSUConfigure RSUConfigure;
	for (unsigned short RSUIdx = 0;RSUIdx != m_Config.RSUNum;RSUIdx++)
	{

		RSUConfigure.wRSUID = RSUIdx;
		m_RSUAry[RSUIdx].initialize(RSUConfigure);
	}
	//FILE *fp;//建立一个文件操作指针
	//      fp=fopen("RSU.txt","w+");//以追加的方式建立或打开1.txt，默认位置在你程序的目录下面
	//      for(unsigned short RSUIdx=0;RSUIdx!=conf.RSUNum;RSUIdx++)
	//fprintf(fp,"%f\n%f\n",RSU[RSUIdx].m_fAbsX,RSU[RSUIdx].m_fAbsY);
	//fclose(fp);//关闭流

	sUEConfigure ueConfigure;
	int ueidx = 0;

	//      FILE *fp;//建立一个文件操作指针
	//      fp=fopen("ue.txt","w+");//以追加的方式建立或打开1.txt，默认位置在你程序的目录下面

	for (unsigned short RoadIdx = 0;RoadIdx != m_Config.RoadNum;RoadIdx++)
	{

		for (int uprIdx = 0;uprIdx != m_Config.pupr[RoadIdx];uprIdx++)
		{
			ueConfigure.wRoadID = RoadIdx;
			ueConfigure.locationID = rand() % m_Config.ueTopoNum;
			ueConfigure.fX = m_Config.pueTopo[ueConfigure.locationID * 2 + 0];
			ueConfigure.fY = m_Config.pueTopo[ueConfigure.locationID * 2 + 1];
			ueConfigure.fAbsX = m_RoadAry[RoadIdx].m_fAbsX + ueConfigure.fX;
			ueConfigure.fAbsY = m_RoadAry[RoadIdx].m_fAbsY + ueConfigure.fY;
			ueConfigure.fv = m_Config.fv;
			//fprintf(fp,"%f\n%f\n",ueConfigure.fAbsX,ueConfigure.fAbsY);
			m_VeUEAry[ueidx++].initialize(ueConfigure);

		}
	}
	//fclose(fp);//关闭流


	//UNDONE
	////由于RSU和基站位置固定，随机将RSU撒给基站进行初始化即可
	//for (int RSUId = 0;RSUId < conf.RSUNum;RSUId++) {
	//	int eNBId = rand() % conf.eNBNum;
	//	eNB[eNBId].m_RSUIdList.push_back(RSUId);
	//}
	//UNDONE

	//创建事件链表
	buildEventList();
}



