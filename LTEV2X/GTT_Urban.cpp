#include<tuple>
#include<iomanip>
#include<iostream>
#include"GTT_Urban.h"
#include"Exception.h"

using namespace std;
using namespace ns_GTT_Urban;

default_random_engine GTT_Urban::s_Engine((unsigned)time(NULL));

GTT_Urban::GTT_Urban(int &t_TTI, Configure& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry, VeUE* &t_VeUEAry) :
	GTT_Basic(t_TTI, t_Config, t_eNBAry, t_RoadAry, t_RSUAry, t_VeUEAry) {}


void GTT_Urban::configure() {
	m_Config.eNBNum = gc_eNBNumber;
	m_Config.UrbanRoadNum = gc_RoadNumber;
	m_Config.RSUNum = gc_RSUNumber;//目前只表示UE RSU数
	m_Config.pupr = new int[m_Config.UrbanRoadNum];
	m_Config.VeUENum = 0;
	int Lambda = static_cast<int>((gc_Length + gc_Width) * 2 * 3.6 / (2.5 * 15));
	for (int temp = 0; temp != m_Config.UrbanRoadNum; ++temp)
	{
		int k = 0;
		long double p = 1.0;
		long double l = exp(-Lambda);  /* 为了精度，才定义为long double的，exp(-Lambda)是接近0的小数*/
		while (p >= l)
		{
			double u = (double)(rand() % 10000)*0.0001f;
			p *= u;
			k++;
		}
		m_Config.pupr[temp] = k - 1;
		m_Config.VeUENum = m_Config.VeUENum + k - 1;
	}
	m_Config.wxNum = 36;
	m_Config.wyNum = 62;
	m_Config.ueTopoNum = (m_Config.wxNum + m_Config.wyNum) * 2 - 4;
	m_Config.pueTopo = new double[m_Config.ueTopoNum * 2];//重合了4个
	double temp_x = -(double)gc_Width / 2 + gc_LaneWidth;
	double temp_y = -(double)gc_Length / 2 + gc_LaneWidth;
	for (int temp = 0; temp != m_Config.ueTopoNum; ++temp)
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
	}
	m_Config.fv = 15;//车速设定,km/h
}


void GTT_Urban::initialize() {
	m_eNBAry = new eNB[m_Config.eNBNum];
	m_RoadAry = new Road[m_Config.UrbanRoadNum];
	m_VeUEAry = new VeUE[m_Config.VeUENum];
	m_RSUAry = new RSU[m_Config.RSUNum];

	UrbanRoadConfigure roadConfigure;
	for (int temp = 0; temp != m_Config.UrbanRoadNum; ++temp)
	{
		roadConfigure.roadId = temp;
		if (temp % 2 == 0)
		{
			roadConfigure.eNBNum = 1;
			roadConfigure.peNB = m_eNBAry;
			roadConfigure.eNBOffset = temp / 2;
		}
		else
		{
			roadConfigure.eNBNum = 0;
		}

		m_RoadAry[temp].initializeUrban(roadConfigure);
	}

	RSUConfigure RSUConfigure;
	for (int RSUIdx = 0; RSUIdx != m_Config.RSUNum; RSUIdx++)
	{

		RSUConfigure.RSUId = RSUIdx;
		m_RSUAry[RSUIdx].initializeGTT_Urban(RSUConfigure);
	}

	VeUEConfigure ueConfigure;
	int ueidx = 0;

	for (int RoadIdx = 0; RoadIdx != m_Config.UrbanRoadNum; RoadIdx++)
	{

		for (int uprIdx = 0; uprIdx != m_Config.pupr[RoadIdx]; uprIdx++)
		{
			ueConfigure.roadId = RoadIdx;
			ueConfigure.locationId = rand() % m_Config.ueTopoNum;
			ueConfigure.X = m_Config.pueTopo[ueConfigure.locationId * 2 + 0];
			ueConfigure.Y = m_Config.pueTopo[ueConfigure.locationId * 2 + 1];
			ueConfigure.AbsX = m_RoadAry[RoadIdx].m_GTT_Urban->m_AbsX + ueConfigure.X;
			ueConfigure.AbsY = m_RoadAry[RoadIdx].m_GTT_Urban->m_AbsY + ueConfigure.Y;
			ueConfigure.V = m_Config.fv;
			m_VeUEAry[ueidx++].initializeGTT_Urban(ueConfigure);

		}
	}
}


void GTT_Urban::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (auto &c : m_VeUEAry[VeUEId].m_GTT->m_InterferenceH) {
			if (c != nullptr) {
				delete[] c;
				c = nullptr;
			}
		}
	}
}


void GTT_Urban::channelGeneration() {
	//RSU.m_VeUEIdList是在freshLoc函数内生成的，因此需要在更新位置前清空这个列表
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_GTT->m_VeUEIdList.clear();
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTT->m_ClusterNum; clusterIdx++) {
			_RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx].clear();
		}
	}
	//同时也清除eNB.m_VeUEIdList
	for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++)
		m_eNBAry[eNBId].m_VeUEIdList.clear();

	//运动模型
	freshLoc();

	//将更新后的RSU.m_VeUEIdList压入对应的簇中
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		for (int VeUEId : _RSU.m_GTT->m_VeUEIdList) {
			int clusterIdx = m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx;
			_RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx].push_back(VeUEId);
		}
	}

	//记录并更新每辆车的位置日志
	for (int VeUEId = 0; VeUEId<m_Config.VeUENum; VeUEId++)
		m_VeUEAry[VeUEId].m_TMC->m_LocationUpdateLogInfoList.push_back(tuple<int, int>(m_VeUEAry[VeUEId].m_GTT->m_RSUId, m_VeUEAry[VeUEId].m_GTT->m_ClusterIdx));
	
	//记录RSU内车辆的数目
	vector<int> curVeUENum;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		curVeUENum.push_back(static_cast<int>(_RSU.m_GTT->m_VeUEIdList.size()));
	}
	m_VeUENumPerRSU.push_back(curVeUENum);


	//UNDONE
	//更新基站的VeUE容器
	for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
		eNB &_eNB = m_eNBAry[eNBId];
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int VeUEId : m_RSUAry[RSUId].m_GTT->m_VeUEIdList) {
				_eNB.m_VeUEIdList.push_back(VeUEId);
			}
		}
	}
	//UNDONE
}


void GTT_Urban::freshLoc() {
	for (int UserIdx = 0; UserIdx != m_Config.VeUENum; UserIdx++)
	{
		bool RoadChangeFlag = false;
		int temp;
		switch (m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId)
		{
		case 191:
			m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 0;
			break;
		case 0:
			temp = rand() % 4;
			if (temp == 0)//turn left
			{
				RoadChangeFlag = true;
				m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId = gc_WrapAroundRoad[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId][4];
				m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 96;
				m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = -90;
			}
			else
				if (temp == 2)//turn right
				{
					m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId + 1;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = 90;
				}
				else//go straight，不改变方向
				{
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId = gc_WrapAroundRoad[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId][5];
					m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 157;
				}
			break;
		case 61:
			temp = rand() % 4;
			if (temp == 0)//turn left
			{
				RoadChangeFlag = true;
				m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId = gc_WrapAroundRoad[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId][6];
				m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 157;
				m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = -180;
			}
			else
				if (temp == 2)
				{
					m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId + 1;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = 0;
				}
				else
				{
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId = gc_WrapAroundRoad[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId][7];
					m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 0;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = 270;
				}
			break;
		case 96:
			temp = rand() % 4;
			if (temp == 0)//turn left
			{
				RoadChangeFlag = true;
				m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId = gc_WrapAroundRoad[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId][8];
				m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 0;
				m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = 90;
			}
			else
				if (temp == 2)
				{
					m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId + 1;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = -90;
				}
				else
				{
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId = gc_WrapAroundRoad[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId][1];
					m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 61;
				}
			break;
		case 157:
			temp = rand() % 4;
			if (temp == 0)//turn left
			{
				RoadChangeFlag = true;
				m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId = gc_WrapAroundRoad[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId][2];
				m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 61;
				m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = 0;
			}
			else
				if (temp == 2)
				{
					m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId + 1;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_VAngle = -180;
				}
				else
				{
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId = gc_WrapAroundRoad[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId][3];
					m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = 96;
				}
			break;
		default:
			m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId = m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId + 1;
			break;
		}
		m_VeUEAry[UserIdx].m_GTT_Urban->m_X = m_Config.pueTopo[m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId * 2 + 0];
		m_VeUEAry[UserIdx].m_GTT_Urban->m_Y = m_Config.pueTopo[m_VeUEAry[UserIdx].m_GTT_Urban->m_LocationId * 2 + 1];
		m_VeUEAry[UserIdx].m_GTT_Urban->m_AbsX = m_RoadAry[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId].m_GTT_Urban->m_AbsX + m_VeUEAry[UserIdx].m_GTT_Urban->m_X;
		m_VeUEAry[UserIdx].m_GTT_Urban->m_AbsY = m_RoadAry[m_VeUEAry[UserIdx].m_GTT_Urban->m_RoadId].m_GTT_Urban->m_AbsY + m_VeUEAry[UserIdx].m_GTT_Urban->m_Y;

	}


	Location location;
	Antenna antenna;
	location.bManhattan = true;

	int RSUIdx = 0;
	int ClusterID = 0;
	for (int UserIdx1 = 0; UserIdx1 != m_Config.VeUENum; UserIdx1++)
	{
		m_VeUEAry[UserIdx1].m_GTT_Urban->m_IMTA = new IMTA[m_Config.RSUNum];
		if (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId <= 30) {
			RSUIdx = gc_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][3];
			ClusterID = 0;
		}
		else if(m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId >= 175) {
			RSUIdx = gc_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][3];
			ClusterID = 1;
		}
		else if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId >= 31) && (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId <= 61)) {
			RSUIdx = gc_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][0];
			ClusterID = 3;
		}
		else if	((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId <= 79) && (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId >=62)){
			RSUIdx = gc_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][0];
			ClusterID = 2;
		}
		else if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId >= 80) && (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId <= 96)) {
			RSUIdx = gc_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][1];
			ClusterID = 5;
		}
		else if((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId >= 97) && (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId <= 126)){
			RSUIdx = gc_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][1];
			ClusterID = 4;
		}
		else if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId >= 127) && (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId <= 157)) {
			RSUIdx = gc_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][2];
			ClusterID = 7;
		}
		else {
			RSUIdx = gc_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][2];
			ClusterID = 6;
	}
		/*switch (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId / 10)
		{
		case 1:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][8];
			ClusterID = 2;
			break;
		case 2:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][8];
			ClusterID = 1;
			break;
		case 3:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][9];
			ClusterID = 2;
			break;
		case 4:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][9];
			ClusterID = 1;
			break;
		case 5:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][0];
			ClusterID = 4;
			break;
		case 6:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][0];
			ClusterID = 3;
			break;
		case 7:
		case 8:
		case 9:
			if (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<87)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][1];
				if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<78))  ClusterID = 3;
				else ClusterID = 2;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][2];
				if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<96))  ClusterID = 6;
				else ClusterID = 5;
			}
			break;
		case 10:
		case 11:
		case 12:
			if (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<106)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][2];
				ClusterID = 5;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][3];
				if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<116))  ClusterID = 0;
				else ClusterID = 3;
			}
			break;
		case 13:
			if (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<126)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][3];
				ClusterID = 3;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][4];
				if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<136))  ClusterID = 0;
				else ClusterID = 3;
			}
			break;
		case 14:
			if (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<146)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][4];
				ClusterID = 3;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][5];
				ClusterID = 0;
			}
			break;
		case 15:
		case 16:
		case 17:
			if (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<165)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][5];
				if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<157))  ClusterID = 0;
				else ClusterID = 7;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][6];
				if ((m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<174))  ClusterID = 1;
				else ClusterID = 0;
			}
			break;
		case 18:
		case 19:
			if (m_VeUEAry[UserIdx1].m_GTT_Urban->m_LocationId<183)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][6];
				ClusterID = 0;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][7];
				ClusterID = 2;
			}
			break;
		case 0:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_GTT_Urban->m_RoadId][7];
			ClusterID = 1;
			break;
		default:
			break;
		}*/
		m_VeUEAry[UserIdx1].m_GTT->m_RSUId = RSUIdx;
		m_VeUEAry[UserIdx1].m_GTT->m_ClusterIdx = ClusterID;
		m_RSUAry[RSUIdx].m_GTT->m_VeUEIdList.push_back(UserIdx1);
		location.eType = None;
		location.distance = 0;
		location.distance1 = 0;
		location.distance2 = 0;

		double angle = 0;
		if (location.bManhattan == true)  //计算location distance
		{
		
			location.eType = Los;// 车辆与所对应的RSU之间均为LOS
			location.distance = sqrt(pow((m_VeUEAry[UserIdx1].m_GTT_Urban->m_AbsX - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsX), 2.0f) + pow((m_VeUEAry[UserIdx1].m_GTT_Urban->m_AbsY - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsY), 2.0f));
			angle = atan2(m_VeUEAry[UserIdx1].m_GTT_Urban->m_AbsY - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsY, m_VeUEAry[UserIdx1].m_GTT_Urban->m_AbsX - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsX) / gc_Degree2PI;

		}
	
		location.eNBAntH = 5;
		location.VeUEAntH = 1.5;
		randomGaussian(location.afPosCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

		antenna.fTxAngle = angle - m_VeUEAry[UserIdx1].m_GTT_Urban->m_FantennaAngle;
		antenna.fRxAngle = angle - m_RSUAry[RSUIdx].m_GTT_Urban->m_FantennaAngle;
		antenna.fAntGain = 6;//收发天线各3dbi
		antenna.byTxAntNum = 1;
		antenna.byRxAntNum = 2;
		antenna.pfTxSlantAngle = new double[antenna.byTxAntNum];
		antenna.pfTxAntSpacing = new double[antenna.byTxAntNum];
		antenna.pfRxSlantAngle = new double[antenna.byRxAntNum];
		antenna.pfRxAntSpacing = new double[antenna.byRxAntNum];
		antenna.pfTxSlantAngle[0] = 90.0f;
		antenna.pfTxAntSpacing[0] = 0.0f;
		antenna.pfRxSlantAngle[0] = 90.0f;
		antenna.pfRxSlantAngle[1] = 90.0f;
		antenna.pfRxAntSpacing[0] = 0.0f;
		antenna.pfRxAntSpacing[1] = 0.5f;

		double t_Pl = 0;

		m_VeUEAry[UserIdx1].m_GTT_Urban->m_IMTA[RSUIdx].build(&t_Pl, gc_FC, location, antenna, m_VeUEAry[UserIdx1].m_GTT_Urban->m_V, m_VeUEAry[UserIdx1].m_GTT_Urban->m_VAngle);//计算了结果代入信道模型计算UE之间信道系数
		bool *flag = new bool();

		m_VeUEAry[UserIdx1].m_GTT->m_Ploss = t_Pl;

		*flag = true;
		m_VeUEAry[UserIdx1].m_GTT_Urban->m_IMTA[RSUIdx].enable(flag);
		double *H = new double[1 * 2 * 12 * 2];
		double *FFT = new double[1 * 2 * 1024 * 2];
		double *ch_buffer = new double[1 * 2 * 12 * 20];
		double *ch_sin = new double[1 * 2 * 12 * 20];
		double *ch_cos = new double[1 * 2 * 12 * 20];

		double *t_HAfterFFT = new double[2 * 1024 * 2];

		m_VeUEAry[UserIdx1].m_GTT_Urban->m_IMTA[RSUIdx].calculate(t_HAfterFFT,0.01f, ch_buffer, ch_sin, ch_cos, H,FFT);
		memcpy(m_VeUEAry[UserIdx1].m_GTT->m_H, t_HAfterFFT, 2 * 1024 * 2 * sizeof(double(0)));

		delete flag;
		delete[] H;
		delete[] ch_buffer;
		delete[] ch_sin;
		delete[] ch_cos;
		delete[] antenna.pfTxSlantAngle;
		delete[] antenna.pfTxAntSpacing;
		delete[] antenna.pfRxSlantAngle;
		delete[] antenna.pfRxAntSpacing;
		delete[] m_VeUEAry[UserIdx1].m_GTT_Urban->m_IMTA;
		delete[] FFT;
		delete[] t_HAfterFFT;
	}
}


void GTT_Urban::writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		out1 << "VeUE[ " << left << setw(3) << VeUEId << "]" << endl;
		out1 << "{" << endl;
		for (const tuple<int, int> &t : m_VeUEAry[VeUEId].m_TMC->m_LocationUpdateLogInfoList)
			out1 << "    " << "[ RSUId = " << left << setw(2) << get<0>(t) << " , ClusterIdx = " << get<1>(t) << " ]" << endl;
		out1 << "}" << endl;
	}
	for (const vector<int> &v : m_VeUENumPerRSU) {
		for (int i : v) {
			out2 << i << " ";
		}
		out2 << endl;
	}
}

void GTT_Urban::calculateInterference(const std::vector<std::list<int>>& RRMInterferenceVec) {
	//由于不同的Pattern可能存在相同的干扰车辆，但是对于同一个干扰车辆，对当前车辆的干扰矩阵只算一次
	for (int patternIdx = 0; patternIdx < RRMInterferenceVec.size(); patternIdx++) {
		const list<int> &lst = RRMInterferenceVec[patternIdx];//当前Pattern下所有车辆的Id

		for (int VeUEId : lst) {
			m_VeUEAry[VeUEId].m_GTT_Urban->m_IMTA = new IMTA[m_Config.RSUNum];
		}
		
		for (int VeUEId : lst) {
			for (int interferenceVeUEId: lst){

				if (interferenceVeUEId == VeUEId) continue;

				if (m_VeUEAry[VeUEId].m_GTT->m_InterferenceH[interferenceVeUEId] != nullptr) continue;

				m_VeUEAry[VeUEId].m_GTT->m_InterferenceH[interferenceVeUEId] = new double[2 * 1024 * 2];

				Location location;
				Antenna antenna;
				location.bManhattan = true;

				int RSUIdx = m_VeUEAry[VeUEId].m_GTT->m_RSUId;
				location.eType = None;
				location.distance = 0;
				location.distance1 = 0;
				location.distance2 = 0;

				double angle = 0;
				if (location.bManhattan == true)  //计算location distance
				{
					if (abs(m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_AbsX - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsX) <= 10.5 || abs(m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_AbsY - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsY) <= 10.5)
					{
						location.eType = Los;
						location.distance = sqrt(pow((m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_AbsX - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsX), 2.0f) + pow((m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_AbsY - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsY), 2.0f));
						angle = atan2(m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_AbsY - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsY, m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_AbsX - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsX) / gc_Degree2PI;
					}
					else
					{
						location.eType = Nlos;
						location.distance1 = abs(m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_AbsX - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsX);
						location.distance2 = abs(m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_AbsY - m_RSUAry[RSUIdx].m_GTT_Urban->m_AbsY);
						location.distance = sqrt(pow(location.distance1, 2.0f) + pow(location.distance2, 2.0f));

					}
				}
				location.eNBAntH = 5;
				location.VeUEAntH = 1.5;
				randomGaussian(location.afPosCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

				antenna.fTxAngle = angle - m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_FantennaAngle;
				antenna.fRxAngle = angle - m_RSUAry[RSUIdx].m_GTT_Urban->m_FantennaAngle;
				antenna.fAntGain = 6;
				antenna.byTxAntNum = 1;
				antenna.byRxAntNum = 2;
				antenna.pfTxSlantAngle = new double[antenna.byTxAntNum];
				antenna.pfTxAntSpacing = new double[antenna.byTxAntNum];
				antenna.pfRxSlantAngle = new double[antenna.byRxAntNum];
				antenna.pfRxAntSpacing = new double[antenna.byRxAntNum];
				antenna.pfTxSlantAngle[0] = 90.0f;
				antenna.pfTxAntSpacing[0] = 0.0f;
				antenna.pfRxSlantAngle[0] = 90.0f;
				antenna.pfRxSlantAngle[1] = 90.0f;
				antenna.pfRxAntSpacing[0] = 0.0f;
				antenna.pfRxAntSpacing[1] = 0.5f;

				double t_Pl = 0;
				m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_IMTA[RSUIdx].build(&t_Pl, gc_FC, location, antenna, m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_V, m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_VAngle);//计算了结果代入信道模型计算UE之间信道系数
				bool *flag = new bool();


				m_VeUEAry[VeUEId].m_GTT->m_InterferencePloss[interferenceVeUEId] = t_Pl;


				*flag = true;
				m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_IMTA[RSUIdx].enable(flag);
				double *H = new double[1 * 2 * 12 * 2];
				double *FFT = new double[1 * 2 * 1024 * 2];
				double *ch_buffer = new double[1 * 2 * 12 * 20];
				double *ch_sin = new double[1 * 2 * 12 * 20];
				double *ch_cos = new double[1 * 2 * 12 * 20];

				double *t_HAfterFFT = new double[2 * 1024 * 2];

				m_VeUEAry[interferenceVeUEId].m_GTT_Urban->m_IMTA[RSUIdx].calculate(t_HAfterFFT, 0.01f, ch_buffer, ch_sin, ch_cos, H, FFT);


				memcpy(m_VeUEAry[VeUEId].m_GTT->m_InterferenceH[interferenceVeUEId], t_HAfterFFT, 2 * 1024 * 2 * sizeof(double(0)));

				delete flag;
				delete[] H;
				delete[] ch_buffer;
				delete[] ch_sin;
				delete[] ch_cos;
				delete[] antenna.pfTxSlantAngle;
				delete[] antenna.pfTxAntSpacing;
				delete[] antenna.pfRxSlantAngle;
				delete[] antenna.pfRxAntSpacing;
				delete[] FFT;
				delete[] t_HAfterFFT;
			}
		}

		for (int VeUEId : lst) {
			delete[] m_VeUEAry[VeUEId].m_GTT_Urban->m_IMTA;
		}

	}
}
