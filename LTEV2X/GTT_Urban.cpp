/*
* =====================================================================================
*
*       Filename:  GTT_Urban.cpp
*
*    Description:  GTT模块下的城镇场景
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  LK,WYB
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<tuple>
#include<iomanip>
#include<iostream>
#include<stdexcept>
#include<string.h>
#include<sstream>
#include"System.h"
#include"GTT_Urban.h"

#include"GTT_RSU.h"
#include"GTT_VeUE.h"

#include"IMTA.h"
#include"Function.h"
#include"ConfigLoader.h"

#define INVALID -1

using namespace std;








default_random_engine GTT_Urban::s_Engine((unsigned)time(NULL));

int GTT_Urban::s_ROAD_LENGTH_SN = INVALID;

int GTT_Urban::s_ROAD_LENGTH_EW = INVALID;

double GTT_Urban::s_ROAD_WIDTH = INVALID;

double GTT_Urban::s_SPEED = INVALID;

const double GTT_Urban::s_ROAD_TOPO_RATIO[s_ROAD_NUM * 2] = {
	-1.5f, 1.0f,
	-0.5f, 1.0f,
	0.5f, 1.0f,
	1.5f, 1.0f,
	-2.5f, 0.0f,
	-1.5f, 0.0f,
	-0.5f, 0.0f,
	0.5f, 0.0f,
	1.5f, 0.0f,
	2.5f, 0.0f,
	-1.5f,-1.0f,
	-0.5f,-1.0f,
	0.5f,-1.0f,
	1.5f,-1.0f
};

const int GTT_Urban::s_WRAP_AROUND_ROAD[s_ROAD_NUM][9] = {
	{ 0,1,6,5,4,13,8,9,10 },
	{ 1,2,7,6,5,0,9,10,11 },
	{ 2,3,8,7,6,1,10,11,12 },
	{ 3,4,9,8,7,2,11,12,13 },
	{ 4,5,10,9,8,3,12,13,0 },
	{ 5,6,11,10,9,4,13,0,1 },
	{ 6,7,12,11,10,5,0,1,2 },
	{ 7,8,13,12,11,6,1,2,3 },
	{ 8,9,0,13,12,7,2,3,4 },
	{ 9,10,1,0,13,8,3,4,5 },
	{ 10,11,2,1,0,9,4,5,6 },
	{ 11,12,3,2,1,10,5,6,7 },
	{ 12,13,4,3,2,11,6,7,8 },
	{ 13,0,5,4,3,12,7,8,9 }
};

const int GTT_Urban::s_RSU_CLUSTER_NUM[s_RSU_NUM] = {
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
};

const double GTT_Urban::s_RSU_TOPO_RATIO[s_RSU_NUM * 2] = {
	-2.0f, 1.5f,
	-1.0f, 1.5f,
	0.0f, 1.5f,
	1.0f, 1.5f,
	2.0f, 1.5f,
	-3.0f, 0.5f,
	-2.0f, 0.5f,
	-1.0f, 0.5f,
	0.0f, 0.5f,
	1.0f, 0.5f,
	2.0f, 0.5f,
	3.0f, 0.5f,
	-3.0f,-0.5f,
	-2.0f,-0.5f,
	-1.0f,-0.5f,
	0.0f,-0.5f,
	1.0f,-0.5f,
	2.0f,-0.5f,
	3.0f,-0.5f,
	-2.0f,-1.5f,
	-1.0f,-1.5f,
	0.0f,-1.5f,
	1.0f,-1.5f,
	2.0f,-1.5f,
};

const int GTT_Urban::s_RSU_IN_ROAD[s_ROAD_NUM][4] = {
	{ 0,1,7,6 },
	{ 1,2,8,7 },
	{ 2,3,9,8 },
	{ 3,4,10,9 },
	{ 5,6,13,12 },
	{ 6,7,14,13 },
	{ 7,8,15,14 },
	{ 8,9,16,15 },
	{ 9,10,17,16 },
	{ 10,11,18,17 },
	{ 13,14,20,19 },
	{ 14,15,21,20 },
	{ 15,16,22,21 },
	{ 16,17,23,22 }
};

void GTT_Urban::loadConfig(Platform t_Platform) {
	ConfigLoader configLoader;
	if (t_Platform == Windows) {
		configLoader.resolvConfigPath("Config\\UrbanConfig.xml");
	}
	else if (t_Platform == Linux) {
		configLoader.resolvConfigPath("Config/UrbanConfig.xml");
	}
	else {
		throw logic_error("Platform Config Error!");
	}

	stringstream ss;

	const string nullString("");
	string temp;

	if ((temp = configLoader.getParam("RoadLengthSN")) != nullString) {
		ss << temp;
		ss >> s_ROAD_LENGTH_SN;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("RoadLengthEW")) != nullString) {
		ss << temp;
		ss >> s_ROAD_LENGTH_EW;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("RoadWidth")) != nullString) {
		ss << temp;
		ss >> s_ROAD_WIDTH;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("Speed")) != nullString) {
		ss << temp;
		ss >> s_SPEED;
		ss.clear();//清除标志位
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	/*cout << "RoadLengthSN: " << s_ROAD_LENGTH_SN << endl;
	cout << "RoadLengthEW: " << s_ROAD_LENGTH_EW << endl;
	cout << "RoadWidth: " << s_ROAD_WIDTH << endl;
	cout << "Speed: " << s_SPEED << endl;
	system("pause");*/
}

GTT_Urban::GTT_Urban(System* t_Context) :
	GTT(t_Context) {}


void GTT_Urban::configure() {
	GTT::s_eNB_NUM = s_eNB_NUM;
	GTT::s_ROAD_NUM = s_ROAD_NUM;
	GTT::s_RSU_NUM = s_RSU_NUM;//目前只表示UE RSU数
	m_pupr = new int[GTT::s_ROAD_NUM];
	int tempVeUENum = 0;
	int Lambda = static_cast<int>((s_ROAD_LENGTH_EW + s_ROAD_LENGTH_SN) * 2 * 3.6 / (2.5 * 15));
	for (int temp = 0; temp != GTT::s_ROAD_NUM; ++temp)
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
		m_pupr[temp] = k - 1;
		tempVeUENum = tempVeUENum + k - 1;
	}
	GTT::s_VeUE_NUM = tempVeUENum;
	//m_xNum = 36;
	//m_yNum = 62;
	//m_ueTopoNum = (m_xNum + m_yNum) * 2 - 4;
	//m_pueTopo = new double[m_ueTopoNum * 2];//重合了4个
	//double temp_x = -(double)gc_Width / 2 + gc_LaneWidth;
	//double temp_y = -(double)gc_Length / 2 + gc_LaneWidth;
	//for (int temp = 0; temp != m_ueTopoNum; ++temp)
	//{
	//	if (temp>0 && temp <= 61) {
	//		if (temp == 60) temp_y += 6; else temp_y += 7;
	//	}
	//	else if (61<temp&&temp <= 96) {
	//		if (temp == 95) temp_x += 5; else temp_x += 7;
	//	}
	//	else if (96<temp&&temp <= 157) {
	//		if (temp == 156) temp_y -= 6; else temp_y -= 7;
	//	}
	//	else if (157<temp&&temp <= 192) {
	//		if (temp == 191) temp_x -= 5; else temp_x -= 7;
	//	}

	//	m_pueTopo[temp * 2 + 0] = temp_x;
	//	m_pueTopo[temp * 2 + 1] = temp_y;
	//}
	m_Speed = s_SPEED;//车速设定,km/h
}


void GTT_Urban::initialize() {
	//初始化m_eNBAry
	m_eNBAry = new GTT_eNB*[GTT::s_eNB_NUM];
	for (int eNBId = 0; eNBId < GTT::s_eNB_NUM; eNBId++) {
		m_eNBAry[eNBId] = new GTT_Urban_eNB();
		//在Road初始化的时候，对eNB进行初始化
	}
	
	//初始化m_RoadAry
	m_RoadAry = new GTT_Road*[GTT::s_ROAD_NUM];
	UrbanRoadConfig urbanRoadConfig;
	for (int roadId = 0; roadId != GTT::s_ROAD_NUM; ++roadId) {
		urbanRoadConfig.roadId = roadId;
		if (roadId % 2 == 0) {
			urbanRoadConfig.eNBNum = 1;
			urbanRoadConfig.eNB = m_eNBAry;
			urbanRoadConfig.eNBOffset = roadId / 2;
		}
		else {
			urbanRoadConfig.eNBNum = 0;
		}
		m_RoadAry[roadId] = new GTT_Urban_Road(urbanRoadConfig);
	}

	//就为了打印一下eNB的信息，由于eNB初始化比较奇怪，由road来初始化
	for (int eNBId = 0; eNBId < GTT::s_eNB_NUM; eNBId++) {
		m_FileLocationInfo << m_eNBAry[eNBId]->toString(0);
	}

	//初始化m_RSUAry
	m_RSUAry = new GTT_RSU*[GTT::s_RSU_NUM];
	for (int RSUId = 0; RSUId != GTT::s_RSU_NUM; RSUId++) {
		m_RSUAry[RSUId] = new GTT_Urban_RSU();
		m_FileLocationInfo << m_RSUAry[RSUId]->toString(0);
	}

	//初始化m_VeUEAry
	m_VeUEAry = new GTT_VeUE*[GTT::s_VeUE_NUM];
	VeUEConfig _VeUEConfig;
	int VeUEId = 0;
	int DistanceFromBottomLeft = 0;


	for (int RoadIdx = 0; RoadIdx != GTT::s_ROAD_NUM; RoadIdx++) {
		for (int uprIdx = 0; uprIdx != m_pupr[RoadIdx]; uprIdx++) {
			_VeUEConfig.roadId = RoadIdx;
			DistanceFromBottomLeft = rand() % (2 * (s_ROAD_LENGTH_EW + s_ROAD_LENGTH_SN));
			if (DistanceFromBottomLeft <= s_ROAD_LENGTH_EW) {
				_VeUEConfig.X = -(s_ROAD_LENGTH_SN + s_ROAD_WIDTH) / 2;
				_VeUEConfig.Y = DistanceFromBottomLeft - s_ROAD_LENGTH_EW / 2;
				_VeUEConfig.angle = 90;
			}
			else if (DistanceFromBottomLeft > s_ROAD_LENGTH_EW && DistanceFromBottomLeft <= (s_ROAD_LENGTH_EW + s_ROAD_LENGTH_SN)) {
				_VeUEConfig.X = DistanceFromBottomLeft - s_ROAD_LENGTH_EW - s_ROAD_LENGTH_SN / 2;
				_VeUEConfig.Y = (s_ROAD_LENGTH_EW + s_ROAD_WIDTH) / 2;
				_VeUEConfig.angle = 0;
			}
			else if (DistanceFromBottomLeft > (s_ROAD_LENGTH_EW + s_ROAD_LENGTH_SN) && DistanceFromBottomLeft < (s_ROAD_LENGTH_EW * 2 + s_ROAD_LENGTH_SN)) {
				_VeUEConfig.X = (s_ROAD_LENGTH_SN + s_ROAD_WIDTH) / 2;
				_VeUEConfig.Y = s_ROAD_LENGTH_EW / 2 - (DistanceFromBottomLeft - (s_ROAD_LENGTH_EW + s_ROAD_LENGTH_SN));
				_VeUEConfig.angle = -90;
			}
			else {
				_VeUEConfig.X = s_ROAD_LENGTH_SN / 2 - (DistanceFromBottomLeft - (s_ROAD_LENGTH_EW * 2 + s_ROAD_LENGTH_SN));
				_VeUEConfig.Y = -(s_ROAD_LENGTH_EW + s_ROAD_WIDTH) / 2;
				_VeUEConfig.angle = -180;
			}
			_VeUEConfig.AbsX = m_RoadAry[RoadIdx]->m_AbsX + _VeUEConfig.X;
			_VeUEConfig.AbsY = m_RoadAry[RoadIdx]->m_AbsY + _VeUEConfig.Y;
			_VeUEConfig.V = m_Speed;
			_VeUEConfig.VeUENum = GTT::s_VeUE_NUM;
			m_VeUEAry[VeUEId++] = new GTT_Urban_VeUE(_VeUEConfig);

		}
	}

	//初始化车辆与RSU的距离
	for (int VeIdx = 0; VeIdx != GTT::s_VeUE_NUM; VeIdx++) {
		m_VeUEAry[VeIdx]->m_Distance = new double[s_RSU_NUM];
			for (int RSUIdx = 0; RSUIdx != s_RSU_NUM; RSUIdx++) {
				m_VeUEAry[VeIdx]->m_Distance[RSUIdx] = sqrt(pow((m_VeUEAry[VeIdx]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX), 2.0f) + pow((m_VeUEAry[VeIdx]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY), 2.0f));
			}
	}
}


void GTT_Urban::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId <GTT::s_VeUE_NUM; VeUEId++) {
		for (auto &c : m_VeUEAry[VeUEId]->m_InterferenceH) {
			if (c != nullptr)
				Delete::safeDelete(c, true);
		}
	}
}


void GTT_Urban::channelGeneration() {
	//RSU.m_VeUEIdList是在freshLoc函数内生成的，因此需要在更新位置前清空这个列表
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		GTT_RSU *_GTT_RSU = m_RSUAry[RSUId];
		_GTT_RSU->m_VeUEIdList.clear();
		for (int clusterIdx = 0; clusterIdx < _GTT_RSU->m_ClusterNum; clusterIdx++) {
			_GTT_RSU->m_ClusterVeUEIdList[clusterIdx].clear();
		}
	}
	//同时也清除eNB.m_VeUEIdList
	for (int eNBId = 0; eNBId < GTT::s_eNB_NUM; eNBId++)
		m_eNBAry[eNBId]->m_VeUEIdList.clear();

	//运动模型
	freshLoc();

	//将更新后的RSU.m_VeUEIdList压入对应的簇中
	for (int RSUId = 0; RSUId <GTT::s_RSU_NUM; RSUId++) {
		GTT_RSU *_GTT_RSU = m_RSUAry[RSUId];
		for (int VeUEId : _GTT_RSU->m_VeUEIdList) {
			int clusterIdx = m_VeUEAry[VeUEId]->m_ClusterIdx;
			_GTT_RSU->m_ClusterVeUEIdList[clusterIdx].push_back(VeUEId);
		}
	}

	//记录并更新每辆车的位置日志
	for (int VeUEId = 0; VeUEId<GTT::s_VeUE_NUM; VeUEId++)
		m_VeUEAry[VeUEId]->m_LocationUpdateLogInfoList.push_back(tuple<int, int>(m_VeUEAry[VeUEId]->m_RSUId, m_VeUEAry[VeUEId]->m_ClusterIdx));
	
	//记录RSU内车辆的数目
	vector<int> curVeUENum;
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		GTT_RSU *_GTT_RSU = m_RSUAry[RSUId];
		curVeUENum.push_back(static_cast<int>(_GTT_RSU->m_VeUEIdList.size()));
	}
	m_VeUENumPerRSU.push_back(curVeUENum);


	//<UNDONE>:基站类的RSUIDList在哪里维护的?
	//更新基站的VeUE容器
	for (int eNBId = 0; eNBId <GTT::s_eNB_NUM; eNBId++) {
		GTT_eNB *_eNB = m_eNBAry[eNBId];
		for (int RSUId : _eNB->m_RSUIdList) {
			for (int VeUEId : m_RSUAry[RSUId]->m_VeUEIdList) {
				_eNB->m_VeUEIdList.push_back(VeUEId);
			}
		}
	}
}


void GTT_Urban::freshLoc() {
	//g_FileVeUEMessage.close();
	//g_FileVeUEMessage.open("Log\\GTTLog\\VeUEMessage.txt");
	double freshTime = ((double)getContext()->m_Config.locationUpdateNTTI) / 1000.0;
	for (int UserIdx = 0; UserIdx != GTT::s_VeUE_NUM; UserIdx++)
	{
		bool RoadChangeFlag = false;
		int temp;
		if (m_VeUEAry[UserIdx]->m_VAngle == 90) {//left
			if ((m_VeUEAry[UserIdx]->m_Y + freshTime*m_VeUEAry[UserIdx]->m_V) > (s_ROAD_LENGTH_EW / 2)) {//top left
				temp = rand() % 4;
				if (temp == 0) {//turn left
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx]->m_X = s_ROAD_LENGTH_SN / 2 - (m_VeUEAry[UserIdx]->m_Y + freshTime*m_VeUEAry[UserIdx]->m_V - s_ROAD_LENGTH_EW / 2);
					m_VeUEAry[UserIdx]->m_Y = -(s_ROAD_LENGTH_EW + s_ROAD_WIDTH) / 2;
					m_VeUEAry[UserIdx]->m_RoadId = s_WRAP_AROUND_ROAD[m_VeUEAry[UserIdx]->m_RoadId][6];
					m_VeUEAry[UserIdx]->m_VAngle = -180;
				}
				else if (temp == 2) {//turn right
					m_VeUEAry[UserIdx]->m_X = (m_VeUEAry[UserIdx]->m_Y + freshTime*m_VeUEAry[UserIdx]->m_V - s_ROAD_LENGTH_EW / 2) - s_ROAD_LENGTH_SN / 2;
					m_VeUEAry[UserIdx]->m_Y = (s_ROAD_LENGTH_EW + s_ROAD_WIDTH) / 2;
					m_VeUEAry[UserIdx]->m_VAngle = 0;
				}
				else {//go straight
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx]->m_Y = (m_VeUEAry[UserIdx]->m_Y + freshTime*m_VeUEAry[UserIdx]->m_V - s_ROAD_LENGTH_EW / 2) - s_ROAD_LENGTH_EW / 2;
					m_VeUEAry[UserIdx]->m_RoadId = s_WRAP_AROUND_ROAD[m_VeUEAry[UserIdx]->m_RoadId][7];
				}
			}
			else {
				m_VeUEAry[UserIdx]->m_Y = m_VeUEAry[UserIdx]->m_Y + freshTime*m_VeUEAry[UserIdx]->m_V;
			}
		}

		else if(m_VeUEAry[UserIdx]->m_VAngle == 0) {//top
			if ((m_VeUEAry[UserIdx]->m_X + freshTime*m_VeUEAry[UserIdx]->m_V) > (s_ROAD_LENGTH_SN / 2)) {//top right
				temp = rand() % 4;
				if (temp == 0) {//turn left
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx]->m_Y = (m_VeUEAry[UserIdx]->m_X + freshTime*m_VeUEAry[UserIdx]->m_V - s_ROAD_LENGTH_SN / 2) - s_ROAD_LENGTH_EW / 2;
					m_VeUEAry[UserIdx]->m_X = -(s_ROAD_LENGTH_SN + s_ROAD_WIDTH) / 2;
					m_VeUEAry[UserIdx]->m_RoadId = s_WRAP_AROUND_ROAD[m_VeUEAry[UserIdx]->m_RoadId][8];
					m_VeUEAry[UserIdx]->m_VAngle = 90;
				}
				else if (temp == 2) {//turn right
					m_VeUEAry[UserIdx]->m_Y = s_ROAD_LENGTH_EW / 2 - (m_VeUEAry[UserIdx]->m_X + freshTime*m_VeUEAry[UserIdx]->m_V - s_ROAD_LENGTH_SN / 2);
					m_VeUEAry[UserIdx]->m_X = (s_ROAD_LENGTH_SN + s_ROAD_WIDTH) / 2;
					m_VeUEAry[UserIdx]->m_VAngle = -90;
				}
				else {//go straight
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx]->m_X = (m_VeUEAry[UserIdx]->m_X + freshTime*m_VeUEAry[UserIdx]->m_V - s_ROAD_LENGTH_SN / 2) - s_ROAD_LENGTH_SN / 2;
					m_VeUEAry[UserIdx]->m_RoadId = s_WRAP_AROUND_ROAD[m_VeUEAry[UserIdx]->m_RoadId][1];
				}
			}
			else {
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_X + freshTime*m_VeUEAry[UserIdx]->m_V;
			}
		}

		else if (m_VeUEAry[UserIdx]->m_VAngle == -90) {//right
			if ((m_VeUEAry[UserIdx]->m_Y - freshTime*m_VeUEAry[UserIdx]->m_V) < -(s_ROAD_LENGTH_EW / 2)) {//bottom right
				temp = rand() % 4;
				if (temp == 0) {//turn left
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx]->m_X = (-s_ROAD_LENGTH_EW / 2 - (m_VeUEAry[UserIdx]->m_Y - freshTime*m_VeUEAry[UserIdx]->m_V)) - s_ROAD_LENGTH_SN / 2;
					m_VeUEAry[UserIdx]->m_Y = (s_ROAD_LENGTH_EW + s_ROAD_WIDTH) / 2;
					m_VeUEAry[UserIdx]->m_RoadId = s_WRAP_AROUND_ROAD[m_VeUEAry[UserIdx]->m_RoadId][2];
					m_VeUEAry[UserIdx]->m_VAngle = 0;
				}
				else if (temp == 2) {//turn right
					m_VeUEAry[UserIdx]->m_X = s_ROAD_LENGTH_SN / 2 - (-s_ROAD_LENGTH_EW / 2 - (m_VeUEAry[UserIdx]->m_Y - freshTime*m_VeUEAry[UserIdx]->m_V));
					m_VeUEAry[UserIdx]->m_Y = -(s_ROAD_LENGTH_EW + s_ROAD_WIDTH) / 2;
					m_VeUEAry[UserIdx]->m_VAngle = -180;
				}
				else {//go straight
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx]->m_Y = s_ROAD_LENGTH_EW / 2 - (-s_ROAD_LENGTH_EW / 2 - (m_VeUEAry[UserIdx]->m_Y - freshTime*m_VeUEAry[UserIdx]->m_V));
					m_VeUEAry[UserIdx]->m_RoadId = s_WRAP_AROUND_ROAD[m_VeUEAry[UserIdx]->m_RoadId][3];
				}
			}
			else {
				m_VeUEAry[UserIdx]->m_Y = m_VeUEAry[UserIdx]->m_Y - freshTime*m_VeUEAry[UserIdx]->m_V;
			}
		}

		else{//bottom
			if ((m_VeUEAry[UserIdx]->m_X - freshTime*m_VeUEAry[UserIdx]->m_V) < -(s_ROAD_LENGTH_SN / 2)) {//bottom left
				temp = rand() % 4;
				if (temp == 0) {//turn left
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx]->m_Y = s_ROAD_LENGTH_EW / 2 - (-s_ROAD_LENGTH_SN / 2 - (m_VeUEAry[UserIdx]->m_X - freshTime*m_VeUEAry[UserIdx]->m_V));
					m_VeUEAry[UserIdx]->m_X = (s_ROAD_LENGTH_SN + s_ROAD_WIDTH) / 2;
					m_VeUEAry[UserIdx]->m_RoadId = s_WRAP_AROUND_ROAD[m_VeUEAry[UserIdx]->m_RoadId][4];
					m_VeUEAry[UserIdx]->m_VAngle = -90;
				}
				else if (temp == 2) {//turn right
					m_VeUEAry[UserIdx]->m_Y = (-s_ROAD_LENGTH_SN / 2 - (m_VeUEAry[UserIdx]->m_X - freshTime*m_VeUEAry[UserIdx]->m_V)) - s_ROAD_LENGTH_EW / 2;
					m_VeUEAry[UserIdx]->m_X = -(s_ROAD_LENGTH_SN + s_ROAD_WIDTH) / 2;
					m_VeUEAry[UserIdx]->m_VAngle = 90;
				}
				else {//go straight
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx]->m_X = s_ROAD_LENGTH_SN / 2 - (-s_ROAD_LENGTH_SN / 2 - (m_VeUEAry[UserIdx]->m_X - freshTime*m_VeUEAry[UserIdx]->m_V));
					m_VeUEAry[UserIdx]->m_RoadId = s_WRAP_AROUND_ROAD[m_VeUEAry[UserIdx]->m_RoadId][5];
				}
			}
			else {
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_X - freshTime*m_VeUEAry[UserIdx]->m_V;
			}
		}
		m_VeUEAry[UserIdx]->m_AbsX = m_RoadAry[m_VeUEAry[UserIdx]->m_RoadId]->m_AbsX + m_VeUEAry[UserIdx]->m_X;
		m_VeUEAry[UserIdx]->m_AbsY = m_RoadAry[m_VeUEAry[UserIdx]->m_RoadId]->m_AbsY + m_VeUEAry[UserIdx]->m_Y;

		//更新车辆与所有RSU之间的距离
		for (int RSUIdx = 0; RSUIdx != s_RSU_NUM; RSUIdx++) {
			m_VeUEAry[UserIdx]->m_Distance[RSUIdx] = sqrt(pow((m_VeUEAry[UserIdx]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX), 2.0f) + pow((m_VeUEAry[UserIdx]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY), 2.0f));
		}
	}


	Location location;
	Antenna antenna;

	location.manhattan = true;

	location.eNBAntH = 5;
	location.VeUEAntH = 1.5;
	location.RSUAntH = 5;

	IMTA::randomGaussian(location.posCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

	int RSUIdx = 0;
	int ClusterID = 0;

	for (int UserIdx1 = 0; UserIdx1 != GTT::s_VeUE_NUM; UserIdx1++)
	{
		//计算车辆与所有RSU之间的路径损耗
		double wPL[s_RSU_NUM] = { 0 };
		for (int RSUIdx = 0; RSUIdx != s_RSU_NUM; RSUIdx++) {

			double absX = abs(m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX);
			double absY = abs(m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY);

			double wDistanceBP = 4 * (location.VeUEAntH - 1)*(location.RSUAntH - 1)*IMTA::s_FC / IMTA::s_C;
			//LOS
			if (absX <= 3.5 || absY <= 3.5) {

				if (m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] > 3 && m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] < wDistanceBP)
				{
					wPL[RSUIdx] = 22.7f * log10(m_VeUEAry[UserIdx1]->m_Distance[RSUIdx]) + 27.0f + 20.0f * (log10(IMTA::s_FC) - 9.0f);//转换为GHz
				}
				else
				{
					if (wDistanceBP < m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] && m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] < 5000)
					{
						wPL[RSUIdx] = 40.0f * log10(m_VeUEAry[UserIdx1]->m_Distance[RSUIdx]) + 7.56f - 17.3f * log10(location.VeUEAntH - 1) - 17.3f * log10(location.RSUAntH - 1) + 2.7f *(log10(IMTA::s_FC) - 9.0f);
					}
					else if (m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] < 3)
					{
						wPL[RSUIdx] = 22.7f * log10(3) + 27.0f + 20.0f * (log10(IMTA::s_FC) - 9.0f);
					}
				}
			}
			//Nlos
			else {
				double fTemp;
				double fPL1, fPL2;
				fTemp = (2.8f - 0.0024f * absX) > 1.84f ? (2.8f - 0.0024f * absX) : 1.84f;
				if (3 < absX&&absX < wDistanceBP)
				{
					fPL1 = 22.7f * log10(absX) + 27.0f + 20.0f *(log10(IMTA::s_FC) - 9.0f);
				}
				else
				{
					if (wDistanceBP < absX&&absX < 5000)
					{
						fPL1 = 40.0f * log10(absX) + 7.56f - 17.3f * log10(location.VeUEAntH - 1) - 17.3f * log10(location.RSUAntH - 1) + 2.7f * (log10(IMTA::s_FC) - 9.0f);
					}
					else if (absX<3)
					{
						fPL1 = 22.7f * log10(3) + 27.0f + 20.0f * (log10(IMTA::s_FC) - 9.0f);
					}
				}
				fPL1 = fPL1 + 17.3f - 12.5f*fTemp + 10 * fTemp * log10(absY) + 3 * (log10(IMTA::s_FC) - 9.0f);
				fTemp = (2.8f - 0.0024f * absY) > 1.84f ? (2.8f - 0.0024f * absY) : 1.84f;
				if (3 < absY&&absY < wDistanceBP)
				{
					fPL2 = 22.7f * log10(absY) + 27.0f + 20.0f * (log10(IMTA::s_FC) - 9.0f);
				}
				else
				{
					if (wDistanceBP < absY&&absY < 5000)
					{
						fPL2 = 40.0f * log10(absY) + 7.56f - 17.3f * log10(location.VeUEAntH - 1) - 17.3f * log10(location.RSUAntH - 1) + 2.7f * (log10(IMTA::s_FC) - 9.0f);
					}
					else if (absY < 3)
					{
						fPL2 = 22.7f * log10(3) + 27.0f + 20.0f *(log10(IMTA::s_FC) - 9.0f);
					}
				}
				fPL2 = fPL2 + 17.3f - 12.5f*fTemp + 10 * fTemp * log10(absX) + 3 * (log10(IMTA::s_FC) - 9.0f);
				wPL[RSUIdx] = fPL1 < fPL2 ? fPL1 : fPL2;
			}
		}

		//计算车辆与所有RSU之间的阴影衰落
		double wShadow[s_RSU_NUM] = { 0 };
		IMTA::randomGaussian(wShadow, s_RSU_NUM, 0.0f, 3.0f);

		//计算车辆与所有RSU之间的大中尺度衰落和
		double wPLSF[s_RSU_NUM];
		for (int RSUIdx = 0; RSUIdx != s_RSU_NUM; RSUIdx++) {
			wPLSF[RSUIdx] = -(wPL[RSUIdx] + wShadow[RSUIdx]);
		}

		//计算出最小的大中尺度衰落
		int FirstRSUIdx, SecondRSUIdx;
		IMTA::selectMax(wPLSF, s_RSU_NUM, &FirstRSUIdx, &SecondRSUIdx);
		//车辆选择最小衰落的RSU与之通信
		RSUIdx = FirstRSUIdx;
			
		m_VeUEAry[UserIdx1]->m_IMTA = new IMTA[GTT::s_RSU_NUM];
		//计算起点为RSU终点为车辆的向量
		double vectorI2V[2];
		vectorI2V[0] = m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX;//向量横坐标
		vectorI2V[1] = m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY;//向量纵坐标
		double tan = vectorI2V[1] / vectorI2V[0];//计算方向角
		//根据方向角判断所在簇的Id
		if (m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] > ((GTT_Urban::s_ROAD_LENGTH_SN + 2 * GTT_Urban::s_ROAD_WIDTH)) / 6) {
			if (vectorI2V[0] >= 0 && vectorI2V[1] >= 0) {
				if (tan < 1)
					ClusterID = 2;
				else
					ClusterID = 1;
			}
			else if (vectorI2V[0] < 0 && vectorI2V[1] >= 0) {
				if (tan < -1)
					ClusterID = 1;
				else
					ClusterID = 4;
			}
			else if (vectorI2V[0] < 0 && vectorI2V[1] < 0) {
				if (tan < 1)
					ClusterID = 4;
				else
					ClusterID = 3;
			}
			else {
				if (tan < -1)
					ClusterID = 3;
				else
					ClusterID = 2;
			}
		}
		else
			ClusterID = 0;

		//判断拥塞等级
		if (ClusterID == 0)
			m_VeUEAry[UserIdx1]->m_CongestionLevel = 1;
		else
			m_VeUEAry[UserIdx1]->m_CongestionLevel = 0;

		m_VeUEAry[UserIdx1]->m_RSUId = RSUIdx;
		m_VeUEAry[UserIdx1]->m_ClusterIdx = ClusterID;
		m_RSUAry[RSUIdx]->m_VeUEIdList.push_back(UserIdx1);

		//输出VeUE信息到文档
		m_FileVeUEMessage << UserIdx1 << " ";
		m_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_RSUId << " ";
		m_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_ClusterIdx << " ";
		m_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_AbsX << " ";
		m_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_AbsY << " ";
		m_FileVeUEMessage << endl;
		//g_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_RoadId << " ";
		//g_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_VAngle << endl;

		location.locationType = None;
		location.distance = 0;
		location.distance1 = 0;
		location.distance2 = 0;

		double angle = 0;

		//判断LOS还是NLOS
		double absX1 = abs(m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX);
		double absY1 = abs(m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY);

		if (absX1 <= 3.5 || absY1 <= 3.5) {
			location.locationType = Los;// 在同一条道路上即为LOS
			location.distance = sqrt(pow((m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX), 2.0f) + pow((m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY), 2.0f));
			angle = atan2(m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY, m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX) / IMTA::s_DEGREE_TO_PI;
		}
		else
		{
			location.locationType = Nlos;
			location.distance1 = absX1;
			location.distance2 = absY1;
			angle = atan2(m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY, m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX) / IMTA::s_DEGREE_TO_PI;
		}
	
		

		antenna.TxAngle = angle - m_VeUEAry[UserIdx1]->m_FantennaAngle;
		antenna.RxAngle = angle - m_RSUAry[RSUIdx]->m_FantennaAngle;
		antenna.antGain = 6;//收发天线各3dbi
		antenna.byTxAntNum = 1;
		antenna.byRxAntNum = 2;
		antenna.TxSlantAngle = new double[antenna.byTxAntNum];
		antenna.TxAntSpacing = new double[antenna.byTxAntNum];
		antenna.RxSlantAngle = new double[antenna.byRxAntNum];
		antenna.RxAntSpacing = new double[antenna.byRxAntNum];
		antenna.TxSlantAngle[0] = 90.0f;
		antenna.TxAntSpacing[0] = 0.0f;
		antenna.RxSlantAngle[0] = 90.0f;
		antenna.RxSlantAngle[1] = 90.0f;
		antenna.RxAntSpacing[0] = 0.0f;
		antenna.RxAntSpacing[1] = 0.5f;

		double t_Pl = 0;

		m_VeUEAry[UserIdx1]->m_IMTA[RSUIdx].build(&t_Pl, IMTA::s_FC, location, antenna, m_VeUEAry[UserIdx1]->m_V*3.6, m_VeUEAry[UserIdx1]->m_VAngle);//计算了结果代入信道模型计算UE之间信道系数
		bool *flag = new bool();

		m_VeUEAry[UserIdx1]->m_Ploss = t_Pl;

		*flag = true;
		m_VeUEAry[UserIdx1]->m_IMTA[RSUIdx].enable(flag);
		double *H = new double[1 * 2 * 12 * 2];
		double *FFT = new double[1 * 2 * 1024 * 2];
		double *ch_buffer = new double[1 * 2 * 12 * 20];
		double *ch_sin = new double[1 * 2 * 12 * 20];
		double *ch_cos = new double[1 * 2 * 12 * 20];

		double *t_HAfterFFT = new double[2 * 1024 * 2];

		m_VeUEAry[UserIdx1]->m_IMTA[RSUIdx].calculate(t_HAfterFFT,0.01f, ch_buffer, ch_sin, ch_cos, H,FFT);
		memcpy(m_VeUEAry[UserIdx1]->m_H, t_HAfterFFT, 2 * 1024 * 2 * sizeof(double(0)));

		Delete::safeDelete(flag);
		Delete::safeDelete(H, true);
		Delete::safeDelete(ch_buffer, true);
		Delete::safeDelete(ch_sin, true);
		Delete::safeDelete(ch_cos, true);
		Delete::safeDelete(antenna.TxSlantAngle, true);
		Delete::safeDelete(antenna.TxAntSpacing, true);
		Delete::safeDelete(antenna.RxSlantAngle, true);
		Delete::safeDelete(antenna.RxAntSpacing, true);
		Delete::safeDelete(m_VeUEAry[UserIdx1]->m_IMTA, true);
		Delete::safeDelete(FFT, true);
		Delete::safeDelete(t_HAfterFFT, true);
	}
}


void GTT_Urban::writeVeUELocationUpdateLogInfo() {
	for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++) {
		m_FileVeUELocationUpdateLogInfo << "VeUE[ " << left << setw(3) << VeUEId << "]" << endl;
		m_FileVeUELocationUpdateLogInfo << "{" << endl;
		for (const tuple<int, int> &t : m_VeUEAry[VeUEId]->m_LocationUpdateLogInfoList)
			m_FileVeUELocationUpdateLogInfo << "    " << "[ RSUId = " << left << setw(2) << get<0>(t) << " , ClusterIdx = " << get<1>(t) << " ]" << endl;
		m_FileVeUELocationUpdateLogInfo << "}" << endl;
	}
	for (const vector<int> &v : m_VeUENumPerRSU) {
		for (int i : v) {
			m_FileVeUENumPerRSULogInfo << i << " ";
		}
		m_FileVeUENumPerRSULogInfo << endl;
	}
}

void GTT_Urban::calculateInterference(const vector<vector<list<int>>>& t_RRMInterferenceVec) {
	for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++) {
		m_VeUEAry[VeUEId]->m_IMTA = new IMTA[GTT::s_RSU_NUM];
	}

	for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++) {
		for (int patternIdx = 0; patternIdx < t_RRMInterferenceVec[0].size(); patternIdx++) {
			const list<int> &interList = t_RRMInterferenceVec[VeUEId][patternIdx];//当前车辆，当前Pattern下所有干扰车辆的Id

			for (int interferenceVeUEId : interList) {

				if (m_VeUEAry[VeUEId]->m_InterferenceH[interferenceVeUEId] != nullptr) continue;

				m_VeUEAry[VeUEId]->m_InterferenceH[interferenceVeUEId] = new double[2 * 1024 * 2];

				Location location;
				Antenna antenna;
				location.manhattan = true;

				int RSUIdx = m_VeUEAry[VeUEId]->m_RSUId;
				location.locationType = None;
				location.distance = 0;
				location.distance1 = 0;
				location.distance2 = 0;

				double angle = 0;
				if (location.manhattan == true)  //计算location distance
				{
					if (abs(m_VeUEAry[interferenceVeUEId]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX) <= 10.5 || abs(m_VeUEAry[interferenceVeUEId]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY) <= 10.5)
					{
						location.locationType = Los;
						location.distance = sqrt(pow((m_VeUEAry[interferenceVeUEId]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX), 2.0f) + pow((m_VeUEAry[interferenceVeUEId]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY), 2.0f));
						angle = atan2(m_VeUEAry[interferenceVeUEId]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY, m_VeUEAry[interferenceVeUEId]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX) / IMTA::s_DEGREE_TO_PI;
					}
					else
					{
						location.locationType = Nlos;
						location.distance1 = abs(m_VeUEAry[interferenceVeUEId]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX);
						location.distance2 = abs(m_VeUEAry[interferenceVeUEId]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY);
						location.distance = sqrt(pow(location.distance1, 2.0f) + pow(location.distance2, 2.0f));

					}
				}
				location.eNBAntH = 5;
				location.VeUEAntH = 1.5;
				location.RSUAntH = 5;
				IMTA::randomGaussian(location.posCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

				antenna.TxAngle = angle - m_VeUEAry[interferenceVeUEId]->m_FantennaAngle;
				antenna.RxAngle = angle - m_RSUAry[RSUIdx]->m_FantennaAngle;
				antenna.antGain = 6;
				antenna.byTxAntNum = 1;
				antenna.byRxAntNum = 2;
				antenna.TxSlantAngle = new double[antenna.byTxAntNum];
				antenna.TxAntSpacing = new double[antenna.byTxAntNum];
				antenna.RxSlantAngle = new double[antenna.byRxAntNum];
				antenna.RxAntSpacing = new double[antenna.byRxAntNum];
				antenna.TxSlantAngle[0] = 90.0f;
				antenna.TxAntSpacing[0] = 0.0f;
				antenna.RxSlantAngle[0] = 90.0f;
				antenna.RxSlantAngle[1] = 90.0f;
				antenna.RxAntSpacing[0] = 0.0f;
				antenna.RxAntSpacing[1] = 0.5f;

				double t_Pl = 0;
				m_VeUEAry[interferenceVeUEId]->m_IMTA[RSUIdx].build(&t_Pl, IMTA::s_FC, location, antenna, m_VeUEAry[interferenceVeUEId]->m_V*3.6, m_VeUEAry[interferenceVeUEId]->m_VAngle);//计算了结果代入信道模型计算UE之间信道系数
				bool *flag = new bool();


				m_VeUEAry[VeUEId]->m_InterferencePloss[interferenceVeUEId] = t_Pl;


				*flag = true;
				m_VeUEAry[interferenceVeUEId]->m_IMTA[RSUIdx].enable(flag);
				double *H = new double[1 * 2 * 12 * 2];
				double *FFT = new double[1 * 2 * 1024 * 2];
				double *ch_buffer = new double[1 * 2 * 12 * 20];
				double *ch_sin = new double[1 * 2 * 12 * 20];
				double *ch_cos = new double[1 * 2 * 12 * 20];

				double *t_HAfterFFT = new double[2 * 1024 * 2];

				m_VeUEAry[interferenceVeUEId]->m_IMTA[RSUIdx].calculate(t_HAfterFFT, 0.01f, ch_buffer, ch_sin, ch_cos, H, FFT);


				memcpy(m_VeUEAry[VeUEId]->m_InterferenceH[interferenceVeUEId], t_HAfterFFT, 2 * 1024 * 2 * sizeof(double(0)));

				Delete::safeDelete(flag);
				Delete::safeDelete(H, true);
				Delete::safeDelete(ch_buffer, true);
				Delete::safeDelete(ch_sin, true);
				Delete::safeDelete(ch_cos, true);
				Delete::safeDelete(antenna.TxSlantAngle, true);
				Delete::safeDelete(antenna.TxAntSpacing, true);
				Delete::safeDelete(antenna.RxSlantAngle, true);
				Delete::safeDelete(antenna.RxAntSpacing, true);
				Delete::safeDelete(FFT, true);
				Delete::safeDelete(t_HAfterFFT, true);
			}	
		}
	}

	for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++) {
		Delete::safeDelete(m_VeUEAry[VeUEId]->m_IMTA, true);
	}
}
