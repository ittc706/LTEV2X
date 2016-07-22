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

	/*打印车辆地理位置更新日志信息*/
	writeVeUELocationUpdateLogInfo(g_OutVeUELocationUpdateLogInfo);
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

	m_NTTI = 200;//仿真TTI时间
	m_Config.periodicEventNTTI = 100;
	m_Config.emergencyLamda = 1;
	m_Config.locationUpdateNTTI = 150;

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

void cSystem::buildEventList() {
	/*按时间顺序（事件的Id与时间相关，Id越小，事件发生的时间越小生成事件链表*/

	srand((unsigned)time(NULL));//iomanip
	//首先生成各个车辆的周期性事件的起始时刻
	vector<list<int>> startTTIVec(m_Config.periodicEventNTTI, list<int>());
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		int startTTI = rand() % m_Config.periodicEventNTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	srand((unsigned)time(NULL));//iomanip
    //根据startTTIVec依次填充PERIOD事件并在其中插入服从泊松分布的紧急事件
	int countEmergency = 0;
	int CTTI = 0;
	while (CTTI < m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < m_Config.periodicEventNTTI; TTIOffset++) {
			list<int>lst = startTTIVec[TTIOffset];
			for (int VeUEId : lst) {
				//----------------WRONG--------------------
				//下面算的p好像不是触发概率，而是单位事件内触发的次数
				//产生服从泊松分布的紧急事件
				double p = m_Config.emergencyLamda / m_NTTI;//触发的概率
				double r = static_cast<double>(rand()) / RAND_MAX;//产生[0-1)分布的随机数
				if (r < p&&CTTI + TTIOffset < m_NTTI) {//若随机数小于概率p，即认为该事件触发
					sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, EMERGENCY);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
					countEmergency++;
				}
				//----------------WRONG--------------------

				//产生周期性事件
				if (CTTI + TTIOffset < m_NTTI) {
					/*-----------------------ATTENTION-----------------------
					* 这里先生成sEvent的对象，然后将其压入m_EventVec
					* 由于Vector<T>.push_back是压入传入对象的复制品，因此会调用sEvent的拷贝构造函数
					* sEvent默认的拷贝构造函数会赋值id成员（因此是安全的）
					*sEvent如果自定义拷贝构造函数，必须在构造函数的初始化部分拷贝id成员
					*-----------------------ATTENTION-----------------------*/
					sEvent evt = sEvent(VeUEId, CTTI + TTIOffset, PERIOD);
					m_EventVec.push_back(evt);
					m_EventTTIList[CTTI + TTIOffset].push_back(evt.eventId);
				}	
			}
		}
		CTTI += m_Config.periodicEventNTTI;
	}

	cout << "countEmergency: " << countEmergency << endl;
}

