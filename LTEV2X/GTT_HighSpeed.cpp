/*
* =====================================================================================
*
*       Filename:  GTT_HighSpeed.cpp
*
*    Description:  TMC模块
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
#include<string.h>
#include"GTT_HighSpeed.h"
#include"Exception.h"
#include"Function.h"
#include"System.h"
#include"Log.h"
#include"IMTA.h"

using namespace std;


GTT_HighSpeed_VeUE::GTT_HighSpeed_VeUE(VeUEConfig &t_VeUEConfig) {
	m_RoadId = t_VeUEConfig.roadId;
	m_X = t_VeUEConfig.X;
	m_Y = t_VeUEConfig.Y;
	m_AbsX = t_VeUEConfig.AbsX;
	m_AbsY = t_VeUEConfig.AbsY;
	m_V = t_VeUEConfig.V / 3.6f;//由km/h换算为m/s，用于车辆的位置更新

	if (m_RoadId <= 2)
		m_VAngle = 0;
	else
		m_VAngle = 180;

	IMTA::randomUniform(&m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_Nt = 1;
	m_Nr = 2;
	m_H = new double[2 * 1024 * 2];
	m_InterferencePloss = vector<double>(t_VeUEConfig.VeUENum, 0);
	m_InterferenceH = vector<double*>(t_VeUEConfig.VeUENum, nullptr);
}


GTT_HighSpeed_RSU::GTT_HighSpeed_RSU() {
	m_AbsX = GTT_HighSpeed::s_RSU_TOPO_RATIO[m_RSUId * 2 + 0] * 100;
	m_AbsY = GTT_HighSpeed::s_RSU_TOPO_RATIO[m_RSUId * 2 + 1];
	IMTA::randomUniform(&m_FantennaAngle, 1, 180.0f, -180.0f, false);
	g_FileLocationInfo << toString(0);
	m_ClusterNum = GTT_HighSpeed::s_RSU_CLUSTER_NUM;
	m_ClusterVeUEIdList = vector<list<int>>(m_ClusterNum);
}


void GTT_HighSpeed_eNB::initialize(eNBConfig &t_eNBConfig) {
	m_eNBId = t_eNBConfig.eNBId;
	m_AbsX = GTT_HighSpeed::s_eNB_TOPO[m_eNBId * 2 + 0];
	m_AbsY = GTT_HighSpeed::s_eNB_TOPO[m_eNBId * 2 + 1];
	g_FileLocationInfo << toString(0);
}


GTT_HighSpeed_Road::GTT_HighSpeed_Road(HighSpeedRodeConfig &t_RoadHighSpeedConfig) {
	m_RoadId = t_RoadHighSpeedConfig.roadId;
	m_AbsX = 0.0f;
	m_AbsY = GTT_HighSpeed::s_ROAD_TOPO_RATIO[m_RoadId * 2 + 1] * GTT_HighSpeed::s_ROAD_WIDTH;
}


default_random_engine GTT_HighSpeed::s_Engine((unsigned)time(NULL));

const double GTT_HighSpeed::s_ROAD_WIDTH = 4.0f;
const double GTT_HighSpeed::s_ISD = 1732.0f;

const double GTT_HighSpeed::s_ROAD_TOPO_RATIO[s_ROAD_NUM * 2] = {
	0.0f, -2.5f,
	0.0f, -1.5f,
	0.0f, -0.5f,
	0.0f, 0.5f,
	0.0f, 1.5f,
	0.0f, 2.5f,
};

const double GTT_HighSpeed::s_RSU_TOPO_RATIO[s_RSU_NUM * 2] = {
	17.0f, 0.0f,
	16.0f, 0.0f,
	15.0f, 0.0f,
	14.0f, 0.0f,
	13.0f, 0.0f,
	12.0f, 0.0f,
	11.0f, 0.0f,
	10.0f, 0.0f,
	9.0f, 0.0f,
	8.0f, 0.0f,
	7.0f, 0.0f,
	6.0f, 0.0f,
	5.0f, 0.0f,
	4.0f, 0.0f,
	3.0f, 0.0f,
	2.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	-1.0f, 0.0f,
	-2.0f, 0.0f,
	-3.0f, 0.0f,
	-4.0f, 0.0f,
	-5.0f, 0.0f,
	-6.0f, 0.0f,
	-7.0f, 0.0f,
	-8.0f, 0.0f,
	-9.0f, 0.0f,
	-10.0f, 0.0f,
	-11.0f, 0.0f,
	-12.0f, 0.0f,
	-13.0f, 0.0f,
	-14.0f, 0.0f,
	-15.0f, 0.0f,
	-16.0f, 0.0f,
	-17.0f, 0.0f,
};
const double GTT_HighSpeed::s_eNB_TOPO[s_eNB_NUM * 2] = {
	-0.5f*s_ISD,35,
	0.5f*s_ISD,35,
};


GTT_HighSpeed::GTT_HighSpeed(System* t_Context) :
	GTT(t_Context) {}


void GTT_HighSpeed::configure() {
	getContext()->m_Config.eNBNum = s_eNB_NUM;
	m_HighSpeedRodeNum = s_ROAD_NUM;
	getContext()->m_Config.RSUNum = s_RSU_NUM;//目前只表示UE RSU数
	m_pupr = new int[m_HighSpeedRodeNum];
	getContext()->m_Config.VeUENum = 0;
	double Lambda = s_ROAD_LENGTH*3.6 / (2.5 * m_Speed);
	srand((unsigned)time(NULL));
	for (int temp = 0; temp != m_HighSpeedRodeNum; ++temp)
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
		getContext()->m_Config.VeUENum = getContext()->m_Config.VeUENum + k - 1;
	}
	m_Speed = 140;//车速设定,km/h
}


void GTT_HighSpeed::initialize() {
	//初始化m_eNBAry
	m_eNBAry = new GTT_eNB*[getContext()->m_Config.eNBNum];
	eNBConfig _eNBConfig;
	for (int eNBId = 0; eNBId != getContext()->m_Config.eNBNum; ++eNBId) {
		_eNBConfig.eNBId = eNBId;
		m_eNBAry[eNBId] = new GTT_HighSpeed_eNB();
		m_eNBAry[eNBId]->initialize(_eNBConfig);
	}

	//初始化m_RoadAry
	m_RoadAry = new GTT_Road*[m_HighSpeedRodeNum];
	HighSpeedRodeConfig highSpeedRodeConfig;
	for (int roadId = 0; roadId != m_HighSpeedRodeNum; ++roadId) {
		highSpeedRodeConfig.roadId = roadId;
		m_RoadAry[roadId] = new GTT_HighSpeed_Road(highSpeedRodeConfig);
	}

	
	//初始化m_RSUAry
	m_RSUAry = new GTT_RSU*[getContext()->m_Config.RSUNum];
	for (int RSUId = 0; RSUId != getContext()->m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId] = new GTT_HighSpeed_RSU();
	}

	//初始化m_VeUEAry
	m_VeUEAry = new GTT_VeUE*[getContext()->m_Config.VeUENum];
	VeUEConfig _VeUEConfig;
	int VeUEId = 0;

	for (int roadId = 0; roadId != m_HighSpeedRodeNum; roadId++) {
		for (int uprIdx = 0; uprIdx != m_pupr[roadId]; uprIdx++) {
			_VeUEConfig.roadId = roadId;
			_VeUEConfig.X = -1732 + rand() % 3465;
			_VeUEConfig.Y = 0.0f;
			_VeUEConfig.AbsX = m_RoadAry[roadId]->m_AbsX + _VeUEConfig.X;
			_VeUEConfig.AbsY = m_RoadAry[roadId]->m_AbsY + _VeUEConfig.Y;
			_VeUEConfig.V = m_Speed;
			_VeUEConfig.VeUENum = getContext()->m_Config.VeUENum;
			m_VeUEAry[VeUEId++] = new GTT_HighSpeed_VeUE(_VeUEConfig);
		}
	}

	for (int VeIdx = 0; VeIdx != getContext()->m_Config.VeUENum; VeIdx++) {
		m_VeUEAry[VeIdx]->m_Distance = new double[s_RSU_NUM];
		for (int RSUIdx = 0; RSUIdx != s_RSU_NUM; RSUIdx++) {
			m_VeUEAry[VeIdx]->m_Distance[RSUIdx] = sqrt(pow((m_VeUEAry[VeIdx]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX), 2.0f) + pow((m_VeUEAry[VeIdx]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY), 2.0f));
		}
	}
}


void GTT_HighSpeed::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		for (auto &c : m_VeUEAry[VeUEId]->m_InterferenceH) {
			if (c != nullptr)
				Delete::safeDelete(c, true);
		}
	}
}


void GTT_HighSpeed::channelGeneration() {
	//RSU.m_VeUEIdList是在freshLoc函数内生成的，因此需要在更新位置前清空这个列表
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		GTT_RSU *_GTT_RSU = m_RSUAry[RSUId];
		_GTT_RSU->m_VeUEIdList.clear();
		for (int clusterIdx = 0; clusterIdx < _GTT_RSU->m_ClusterNum; clusterIdx++) {
			_GTT_RSU->m_ClusterVeUEIdList[clusterIdx].clear();
		}
	}
	//同时也清除eNB.m_VeUEIdList
	for (int eNBId = 0; eNBId < getContext()->m_Config.eNBNum; eNBId++)
		m_eNBAry[eNBId]->m_VeUEIdList.clear();

	//运动模型
	freshLoc();

	//将更新后的RSU.m_VeUEIdList压入对应的簇中
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		GTT_RSU *_GTT_RSU = m_RSUAry[RSUId];
		for (int VeUEId : _GTT_RSU->m_VeUEIdList) {
			int clusterIdx = m_VeUEAry[VeUEId]->m_ClusterIdx;
			_GTT_RSU->m_ClusterVeUEIdList[clusterIdx].push_back(VeUEId);
		}
	}

	//记录并更新每辆车的位置日志
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++)
		m_VeUEAry[VeUEId]->m_LocationUpdateLogInfoList.push_back(tuple<int, int>(m_VeUEAry[VeUEId]->m_RSUId, m_VeUEAry[VeUEId]->m_ClusterIdx));

	//记录RSU内车辆的数目
	vector<int> curVeUENum;
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		GTT_RSU *_GTT_RSU = m_RSUAry[RSUId];
		curVeUENum.push_back(static_cast<int>(_GTT_RSU->m_VeUEIdList.size()));
	}
	m_VeUENumPerRSU.push_back(curVeUENum);


	//<UNDONE>:基站类的RSUIDList在哪里维护的?
	//更新基站的VeUE容器
	for (int eNBId = 0; eNBId < getContext()->m_Config.eNBNum; eNBId++) {
		GTT_eNB *_eNB = m_eNBAry[eNBId];
		for (int RSUId : _eNB->m_RSUIdList) {
			for (int VeUEId : m_RSUAry[RSUId]->m_VeUEIdList) {
				_eNB->m_VeUEIdList.push_back(VeUEId);
			}
		}
	}
}


void GTT_HighSpeed::freshLoc() {
	double freshTime = ((double)getContext()->m_Config.locationUpdateNTTI) / 1000.0;
	for (int UserIdx = 0; UserIdx != getContext()->m_Config.VeUENum; UserIdx++)
	{

		if (m_VeUEAry[UserIdx]->m_VAngle == 0)
		{
			m_VeUEAry[UserIdx]->m_ClusterIdx = 0;//由西向东车辆簇编号为0

			if ((m_VeUEAry[UserIdx]->m_AbsX + freshTime*m_VeUEAry[UserIdx]->m_V)>(s_ROAD_LENGTH / 2))
			{
				m_VeUEAry[UserIdx]->m_AbsX = (m_VeUEAry[UserIdx]->m_AbsX + freshTime*m_VeUEAry[UserIdx]->m_V) - s_ROAD_LENGTH;
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_AbsX;
			}
			else
			{
				m_VeUEAry[UserIdx]->m_AbsX = m_VeUEAry[UserIdx]->m_AbsX + freshTime*m_VeUEAry[UserIdx]->m_V;
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_AbsX;
			}
		}
		else
		{
			m_VeUEAry[UserIdx]->m_ClusterIdx = 1;//由东向西车辆簇编号为1

			if ((m_VeUEAry[UserIdx]->m_AbsX - freshTime*m_VeUEAry[UserIdx]->m_V)<(-s_ROAD_LENGTH / 2))
			{
				m_VeUEAry[UserIdx]->m_AbsX = (m_VeUEAry[UserIdx]->m_AbsX - freshTime*m_VeUEAry[UserIdx]->m_V) + s_ROAD_LENGTH;
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_AbsX;
			}
			else
			{
				m_VeUEAry[UserIdx]->m_AbsX = m_VeUEAry[UserIdx]->m_AbsX - freshTime*m_VeUEAry[UserIdx]->m_V;
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_AbsX;
			}
		}

		for (int RSUIdx = 0; RSUIdx != s_RSU_NUM; RSUIdx++) {
			m_VeUEAry[UserIdx]->m_Distance[RSUIdx] = sqrt(pow((m_VeUEAry[UserIdx]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX), 2.0f) + pow((m_VeUEAry[UserIdx]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY), 2.0f));
		}
	}


	Location location;
	Antenna antenna;
	location.eNBAntH = 5;
	location.VeUEAntH = 1.5;
	location.RSUAntH = 5;

	int RSUIdx = 0;
	for (int UserIdx1 = 0; UserIdx1 != getContext()->m_Config.VeUENum; UserIdx1++)
	{
		//计算车辆与所有RSU之间的路径损耗
		double wPL[s_RSU_NUM] = { 0 };
		for (int RSUIdx = 0; RSUIdx != s_RSU_NUM; RSUIdx++) {
			double wSFSTD = 0;
			double wDistanceBP = 4 * (location.VeUEAntH - 1)*(location.RSUAntH - 1)*IMTA::s_FC / IMTA::s_C;
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

		m_VeUEAry[UserIdx1]->m_IMTA = new IMTA[getContext()->m_Config.RSUNum];

		m_VeUEAry[UserIdx1]->m_RSUId = RSUIdx;
		m_RSUAry[RSUIdx]->m_VeUEIdList.push_back(UserIdx1);

		//输出VeUE信息到文档
		g_FileVeUEMessage << UserIdx1 << " ";
		g_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_RSUId << " ";
		g_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_ClusterIdx << " ";
		g_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_AbsX << " ";
		g_FileVeUEMessage << m_VeUEAry[UserIdx1]->m_AbsY << endl;

		location.locationType = None;
		location.distance = 0;
		location.distance1 = 0;
		location.distance2 = 0;

		double angle = 0;
		location.manhattan = false;

		location.locationType = Los;
		location.distance = sqrt(pow((m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX), 2.0f) + pow((m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY), 2.0f));
		angle = atan2(m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY, m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX) / IMTA::s_DEGREE_TO_PI;

		IMTA::randomGaussian(location.posCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

		antenna.TxAngle = angle - m_VeUEAry[UserIdx1]->m_FantennaAngle;
		antenna.RxAngle = angle - m_RSUAry[RSUIdx]->m_FantennaAngle;
		antenna.antGain = 3;
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

		double *H = new double[1 * 2 * 19 * 2];
		double *FFT = new double[1 * 2 * 1024 * 2];
		double *ch_buffer = new double[1 * 2 * 19 * 20];
		double *ch_sin = new double[1 * 2 * 19 * 20];
		double *ch_cos = new double[1 * 2 * 19 * 20];

		double *t_HAfterFFT = new double[2 * 1024 * 2];

		m_VeUEAry[UserIdx1]->m_IMTA[RSUIdx].calculate(t_HAfterFFT, 0.01f, ch_buffer, ch_sin, ch_cos, H, FFT);
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


void GTT_HighSpeed::writeVeUELocationUpdateLogInfo(ofstream &out1, ofstream &out2) {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		out1 << "VeUE[ " << left << setw(3) << VeUEId << "]" << endl;
		out1 << "{" << endl;
		for (const tuple<int, int> &t : m_VeUEAry[VeUEId]->m_LocationUpdateLogInfoList)
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


void GTT_HighSpeed::calculateInterference(const vector<vector<list<int>>>& t_RRMInterferenceVec) {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId]->m_IMTA = new IMTA[getContext()->m_Config.RSUNum];
	}

	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		for (int patternIdx = 0; patternIdx < t_RRMInterferenceVec[VeUEId].size(); patternIdx++) {
			const list<int> &lst = t_RRMInterferenceVec[VeUEId][patternIdx];//当前车辆，当前Pattern下所有干扰车辆的Id

			for (int interferenceVeUEId : lst) {

				if (m_VeUEAry[VeUEId]->m_InterferenceH[interferenceVeUEId] != nullptr) continue;

				m_VeUEAry[VeUEId]->m_InterferenceH[interferenceVeUEId] = new double[2 * 1024 * 2];

				Location location;
				Antenna antenna;


				int RSUIdx = m_VeUEAry[VeUEId]->m_RSUId;
				location.locationType = None;
				location.distance = 0;
				location.distance1 = 0;
				location.distance2 = 0;

				double angle = 0;
				location.manhattan = false;

				location.locationType = Los;
				location.distance = sqrt(pow((m_VeUEAry[interferenceVeUEId]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX), 2.0f) + pow((m_VeUEAry[interferenceVeUEId]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY), 2.0f));
				angle = atan2(m_VeUEAry[interferenceVeUEId]->m_AbsY - m_RSUAry[RSUIdx]->m_AbsY, m_VeUEAry[interferenceVeUEId]->m_AbsX - m_RSUAry[RSUIdx]->m_AbsX) / IMTA::s_DEGREE_TO_PI;

				location.eNBAntH = 5;
				location.VeUEAntH = 1.5;
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
				double *H = new double[1 * 2 * 19 * 2];
				double *FFT = new double[1 * 2 * 1024 * 2];
				double *ch_buffer = new double[1 * 2 * 19 * 20];
				double *ch_sin = new double[1 * 2 * 19 * 20];
				double *ch_cos = new double[1 * 2 * 19 * 20];

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

	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		Delete::safeDelete(m_VeUEAry[VeUEId]->m_IMTA, true);
	}
}