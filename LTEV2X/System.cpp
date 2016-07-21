#include<iomanip>
#include<fstream>
#include"System.h"
#include"Global.h"
#include"RSU.h"

using namespace std;

void cSystem::process() {

	/*参数配置*/
	configure();

	/*仿真初始化*/
	initialization();

	/*打印事件链表*/
	writeEventListInfo(g_OutEventListInfo);

	for (int count = 0;count < m_NTTI;count++) {
		cout << "Current TTI = " << m_TTI << endl;
		if(count%100==0)ChannelGeneration();
		DRASchedule();
		m_TTI++;
	}

	/*打印事件日志信息*/
	writeEventLogInfo(g_OutEventLogInfo);


	/*打印车辆地理位置更新日志信息*/
	writeVeUELocationUpdateLogInfo(g_OutVeUELocationUpdateLogInfo);
}

void cSystem::configure() {//系统仿真参数配置
	/*LK*/
	//仿真参数配置

	//data.AllocMem(conf.userNum);
	conf.eNBNum = c_eNBNumber;
	conf.RoadNum = c_roadNumber;
	conf.RSUNum = c_RSUNumber;//目前只表示UE RSU数
	conf.pupr = new int[conf.RoadNum];
	conf.VeUENum = 0;
	int Lambda = (c_length + c_wide) * 2 * 3.6 / (2.5 * 15);
	srand((unsigned)time(NULL));
	for (unsigned short temp = 0; temp != conf.RoadNum; ++temp)
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
		conf.pupr[temp] = k - 1;
		conf.VeUENum = conf.VeUENum + k - 1;
		//printf("%d\n",k-1);
	}
	conf.wxNum = 36;
	conf.wyNum = 62;
	conf.ueTopoNum = (conf.wxNum + conf.wyNum) * 2 - 4;
	conf.pueTopo = new float[conf.ueTopoNum * 2];//重合了4个
	float temp_x = -(float)c_wide / 2 + c_lane_wide;
	float temp_y = -(float)c_length / 2 + c_lane_wide;
	for (int temp = 0;temp != conf.ueTopoNum; ++temp)
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

		conf.pueTopo[temp * 2 + 0] = temp_x;
		conf.pueTopo[temp * 2 + 1] = temp_y;
		//printf("%f,%f\n",temp_x,temp_y);//之后输出到表格直接读入可以省时间
	}
	conf.fv = 15;//车速设定,km/h
	/*LK*/





	m_NTTI = 200;//仿真TTI时间
	conf.periodicEventNTTI = 100;
	conf.emergencyLamda = 1;
	conf.locationUpdateNTTI = 150;

	//conf.VeUENum = 2000;
	//conf.RSUNum = 36;
	//conf.eNBNum = 4;
}


void cSystem::initialization() {
	srand((unsigned)time(NULL));//iomanip
	m_TTI = 0;

	/*LK*/
	eNB = new ceNB[conf.eNBNum];
	Road = new cRoad[conf.RoadNum];
	veUE = new cVeUE[conf.VeUENum];
	RSU = new cRSU[conf.RSUNum];

	sRoadConfigure roadConfigure;
	for (unsigned short temp = 0;temp != conf.RoadNum; ++temp)
	{
		roadConfigure.wRoadID = temp;
		if (temp % 2 == 0)
		{
			roadConfigure.weNBNum = 1;
			roadConfigure.peNB = eNB;
			roadConfigure.weNBOffset = temp / 2;
		}
		else
		{
			roadConfigure.weNBNum = 0;
		}

		Road[temp].Initialize(roadConfigure);
	}

	//       FILE *fp1;//建立一个文件操作指针
	//        fp1=fopen("eNB.txt","w+");//以追加的方式建立或打开1.txt，默认位置在你程序的目录下面
	//        for(int eNBIdx=0;eNBIdx!=c_eNBNumber;++eNBIdx)
	//	      fprintf(fp1,"%f\n%f
	sRSUConfigure RSUConfigure;
	for (unsigned short RSUIdx = 0;RSUIdx != conf.RSUNum;RSUIdx++)
	{

		RSUConfigure.wRSUID = RSUIdx;
		RSU[RSUIdx].Initialize(RSUConfigure);
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

	for (unsigned short RoadIdx = 0;RoadIdx != conf.RoadNum;RoadIdx++)
	{

		for (int uprIdx = 0;uprIdx != conf.pupr[RoadIdx];uprIdx++)
		{
			ueConfigure.wRoadID = RoadIdx;
			ueConfigure.locationID = rand() % conf.ueTopoNum;
			ueConfigure.fX = conf.pueTopo[ueConfigure.locationID * 2 + 0];
			ueConfigure.fY = conf.pueTopo[ueConfigure.locationID * 2 + 1];
			ueConfigure.fAbsX = Road[RoadIdx].m_fAbsX + ueConfigure.fX;
			ueConfigure.fAbsY = Road[RoadIdx].m_fAbsY + ueConfigure.fY;
			ueConfigure.fv = conf.fv;
			//fprintf(fp,"%f\n%f\n",ueConfigure.fAbsX,ueConfigure.fAbsY);
			veUE[ueidx++].Initialize(ueConfigure);

		}
	}
	//fclose(fp);//关闭流

	/*LK*/



	/*eNB = new ceNB[conf.eNBNum];
	RSU = new cRSU[conf.RSUNum];
	veUE = new cVeUE[conf.VeUENum];*/
	
	
	
	
	m_EventTTIList = vector<list<int>>(m_NTTI);



	//由于RSU和基站位置固定，随机将RSU撒给基站进行初始化即可
	for (int RSUId = 0;RSUId < conf.RSUNum;RSUId++) {
		int eNBId = rand() % conf.eNBNum;
		eNB[eNBId].m_RSUIdList.push_back(RSUId);
	}

	//选择DRA模式
	m_DRAMode = P123;

	//创建事件链表
	buildEventList();

}

void cSystem::buildEventList() {
	/*按时间顺序（事件的Id与时间相关，Id越小，事件发生的时间越小生成事件链表*/

	srand((unsigned)time(NULL));//iomanip
	//首先生成各个车辆的周期性事件的起始时刻
	vector<list<int>> startTTIVec(conf.periodicEventNTTI, list<int>());
	for (int VeUEId = 0; VeUEId < conf.VeUENum; VeUEId++) {
		int startTTI = rand() % conf.periodicEventNTTI;
		startTTIVec[startTTI].push_back(VeUEId);
	}

	srand((unsigned)time(NULL));//iomanip
    //根据startTTIVec依次填充PERIOD事件并在其中插入服从泊松分布的紧急事件
	int countEmergency = 0;
	int CTTI = 0;
	while (CTTI < m_NTTI) {
		for (int TTIOffset = 0; TTIOffset < conf.periodicEventNTTI; TTIOffset++) {
			list<int>lst = startTTIVec[TTIOffset];
			for (int VeUEId : lst) {
				//----------------WRONG--------------------
				//下面算的p好像不是触发概率，而是单位事件内触发的次数
				//产生服从泊松分布的紧急事件
				double p = conf.emergencyLamda / m_NTTI;//触发的概率
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
		CTTI += conf.periodicEventNTTI;
	}

	cout << "countEmergency: " << countEmergency << endl;
}

