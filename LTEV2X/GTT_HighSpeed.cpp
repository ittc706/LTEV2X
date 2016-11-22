#include<tuple>
#include<iomanip>
#include<iostream>
#include<string.h>
#include"GTT_HighSpeed.h"
#include"Exception.h"
#include"Function.h"

using namespace std;
using namespace ns_GTT_HighSpeed;


void GTT_HighSpeed_VeUE::initialize(VeUEConfig &t_VeUEConfig) {
	m_RoadId = t_VeUEConfig.laneId;
	m_X = t_VeUEConfig.X;
	m_Y = t_VeUEConfig.Y;
	m_AbsX = t_VeUEConfig.AbsX;
	m_AbsY = t_VeUEConfig.AbsY;
	m_V = t_VeUEConfig.V / 3.6f;

	if (m_RoadId <= 2)
		m_VAngle = 0;
	else
		m_VAngle = 180;

	randomUniform(&m_FantennaAngle, 1, 180.0f, -180.0f, false);

	m_Nt = 1;
	m_Nr = 2;
	m_H = new double[2 * 1024 * 2];
	m_InterferencePloss = vector<double>(t_VeUEConfig.VeUENum, 0);
	m_InterferenceH = vector<double*>(t_VeUEConfig.VeUENum, nullptr);
}


default_random_engine GTT_HighSpeed::s_Engine((unsigned)time(NULL));

GTT_HighSpeed::GTT_HighSpeed(int &t_TTI, SystemConfig& t_Config, eNB* &t_NBAry, Road* &t_RoadAry, RSU* &t_RSUAry) :
	GTT_Basic(t_TTI, t_Config, t_NBAry, t_RoadAry, t_RSUAry) {}


void GTT_HighSpeed::configure() {
	m_Config.eNBNum = gc_eNBNumber;
	m_HighSpeedRodeNum = gc_LaneNumber;
	m_Config.RSUNum = gc_RSUNumber;//目前只表示UE RSU数
	m_pupr = new int[m_HighSpeedRodeNum];
	m_Config.VeUENum = 0;
	double Lambda = gc_Length*3.6 / (2.5 * 140);
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
		m_Config.VeUENum = m_Config.VeUENum + k - 1;
	}
	m_Speed = 140;//车速设定,km/h
}


void GTT_HighSpeed::initialize() {
	//初始化m_eNBAry
	m_eNBAry = new eNB[m_Config.eNBNum];
	eNBConfig _eNBConfig;
	for (int temp = 0; temp != m_Config.eNBNum; ++temp)
	{
		_eNBConfig.eNBId = temp;
		m_eNBAry[temp].initializeHighSpeed(_eNBConfig);
	}

	//初始化m_RoadAry
	m_RoadAry = new Road[m_HighSpeedRodeNum];
	HighSpeedRodeConfig highSpeedRodeConfig;
	for (int temp = 0; temp != m_HighSpeedRodeNum; ++temp) {
		highSpeedRodeConfig.roadId = temp;
		m_RoadAry[temp].initializeHighSpeed(highSpeedRodeConfig);
	}

	//初始化m_RSUAry
	m_RSUAry = new RSU[m_Config.RSUNum];
	RSUConfig _RSUConfig;
	for (int RSUIdx = 0; RSUIdx != m_Config.RSUNum; RSUIdx++) {

		_RSUConfig.RSUId = RSUIdx;
		m_RSUAry[RSUIdx].initializeGTT_HighSpeed(_RSUConfig);
	}

	//初始化m_VeUEAry
	m_VeUEAry = new GTT_VeUE*[m_Config.VeUENum];
	VeUEConfig _VeUEConfig;
	int VeUEId = 0;

	for (int LaneIdx = 0; LaneIdx != m_HighSpeedRodeNum; LaneIdx++) {
		for (int uprIdx = 0; uprIdx != m_pupr[LaneIdx]; uprIdx++) {
			_VeUEConfig.laneId = LaneIdx;
			_VeUEConfig.X = -1732 + rand() % 3465;
			_VeUEConfig.Y = 0.0f;
			_VeUEConfig.AbsX = m_RoadAry[LaneIdx].m_GTT->m_AbsX + _VeUEConfig.X;
			_VeUEConfig.AbsY = m_RoadAry[LaneIdx].m_GTT->m_AbsY + _VeUEConfig.Y;
			_VeUEConfig.V = m_Speed;
			_VeUEConfig.VeUENum = m_Config.VeUENum;
			m_VeUEAry[VeUEId] = new GTT_HighSpeed_VeUE();
			m_VeUEAry[VeUEId++]->initialize(_VeUEConfig);
		}
	}

	for (int VeIdx = 0; VeIdx != m_Config.VeUENum; VeIdx++) {
		m_VeUEAry[VeIdx]->m_Distance = new double[gc_RSUNumber];
		for (int RSUIdx = 0; RSUIdx != gc_RSUNumber; RSUIdx++) {
			m_VeUEAry[VeIdx]->m_Distance[RSUIdx] = sqrt(pow((m_VeUEAry[VeIdx]->m_AbsX - m_RSUAry[RSUIdx].m_GTT->m_AbsX), 2.0f) + pow((m_VeUEAry[VeIdx]->m_AbsY - m_RSUAry[RSUIdx].m_GTT->m_AbsY), 2.0f));
		}
	}
}


void GTT_HighSpeed::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (auto &c : m_VeUEAry[VeUEId]->m_InterferenceH) {
			if (c != nullptr)
				Delete::safeDelete(c, true);
		}
	}
}


void GTT_HighSpeed::channelGeneration() {
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
			int clusterIdx = m_VeUEAry[VeUEId]->m_ClusterIdx;
			_RSU.m_GTT->m_ClusterVeUEIdList[clusterIdx].push_back(VeUEId);
		}
	}

	//记录并更新每辆车的位置日志
	for (int VeUEId = 0; VeUEId<m_Config.VeUENum; VeUEId++)
		m_VeUEAry[VeUEId]->m_LocationUpdateLogInfoList.push_back(tuple<int, int>(m_VeUEAry[VeUEId]->m_RSUId, m_VeUEAry[VeUEId]->m_ClusterIdx));

	//记录RSU内车辆的数目
	vector<int> curVeUENum;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		curVeUENum.push_back(static_cast<int>(_RSU.m_GTT->m_VeUEIdList.size()));
	}
	m_VeUENumPerRSU.push_back(curVeUENum);


	//<UNDONE>:基站类的RSUIDList在哪里维护的?
	//更新基站的VeUE容器
	for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
		eNB &_eNB = m_eNBAry[eNBId];
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int VeUEId : m_RSUAry[RSUId].m_GTT->m_VeUEIdList) {
				_eNB.m_VeUEIdList.push_back(VeUEId);
			}
		}
	}
}


void GTT_HighSpeed::freshLoc() {
	for (int UserIdx = 0; UserIdx != m_Config.VeUENum; UserIdx++)
	{

		if (m_VeUEAry[UserIdx]->m_VAngle == 0)
		{
			m_VeUEAry[UserIdx]->m_ClusterIdx = 0;//由西向东车辆簇编号为0

			if ((m_VeUEAry[UserIdx]->m_AbsX + gc_FreshTime*m_VeUEAry[UserIdx]->m_V)>(gc_Length / 2))
			{
				m_VeUEAry[UserIdx]->m_AbsX = (m_VeUEAry[UserIdx]->m_AbsX + gc_FreshTime*m_VeUEAry[UserIdx]->m_V) - gc_Length;
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_AbsX;
			}
			else
			{
				m_VeUEAry[UserIdx]->m_AbsX = m_VeUEAry[UserIdx]->m_AbsX + gc_FreshTime*m_VeUEAry[UserIdx]->m_V;
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_AbsX;
			}
		}
		else
		{
			m_VeUEAry[UserIdx]->m_ClusterIdx = 1;//由东向西车辆簇编号为1

			if ((m_VeUEAry[UserIdx]->m_AbsX - gc_FreshTime*m_VeUEAry[UserIdx]->m_V)<(-gc_Length / 2))
			{
				m_VeUEAry[UserIdx]->m_AbsX = (m_VeUEAry[UserIdx]->m_AbsX - gc_FreshTime*m_VeUEAry[UserIdx]->m_V) + gc_Length;
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_AbsX;
			}
			else
			{
				m_VeUEAry[UserIdx]->m_AbsX = m_VeUEAry[UserIdx]->m_AbsX - gc_FreshTime*m_VeUEAry[UserIdx]->m_V;
				m_VeUEAry[UserIdx]->m_X = m_VeUEAry[UserIdx]->m_AbsX;
			}
		}

		for (int RSUIdx = 0; RSUIdx != gc_RSUNumber; RSUIdx++) {
			m_VeUEAry[UserIdx]->m_Distance[RSUIdx] = sqrt(pow((m_VeUEAry[UserIdx]->m_AbsX - m_RSUAry[RSUIdx].m_GTT->m_AbsX), 2.0f) + pow((m_VeUEAry[UserIdx]->m_AbsY - m_RSUAry[RSUIdx].m_GTT->m_AbsY), 2.0f));
		}
	}


	Location location;
	Antenna antenna;
	location.eNBAntH = 5;
	location.VeUEAntH = 1.5;
	location.RSUAntH = 5;

	int RSUIdx = 0;
	for (int UserIdx1 = 0; UserIdx1 != m_Config.VeUENum; UserIdx1++)
	{
		//计算车辆与所有RSU之间的路径损耗
		double wPL[gc_RSUNumber] = { 0 };
		for (int RSUIdx = 0; RSUIdx != gc_RSUNumber; RSUIdx++) {
			double wSFSTD = 0;
			double wDistanceBP = 4 * (location.VeUEAntH - 1)*(location.RSUAntH - 1)*gc_FC / gc_C;
			if (m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] > 3 && m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] < wDistanceBP)
			{
				wPL[RSUIdx] = 22.7f * log10(m_VeUEAry[UserIdx1]->m_Distance[RSUIdx]) + 27.0f + 20.0f * (log10(gc_FC) - 9.0f);//转换为GHz
			}
			else
			{
				if (wDistanceBP < m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] && m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] < 5000)
				{
					wPL[RSUIdx] = 40.0f * log10(m_VeUEAry[UserIdx1]->m_Distance[RSUIdx]) + 7.56f - 17.3f * log10(location.VeUEAntH - 1) - 17.3f * log10(location.RSUAntH - 1) + 2.7f *(log10(gc_FC) - 9.0f);
				}
				else if (m_VeUEAry[UserIdx1]->m_Distance[RSUIdx] < 3)
				{
					wPL[RSUIdx] = 22.7f * log10(3) + 27.0f + 20.0f * (log10(gc_FC) - 9.0f);
				}
			}
		}

		//计算车辆与所有RSU之间的阴影衰落
		double wShadow[gc_RSUNumber] = { 0 };
		randomGaussian(wShadow, gc_RSUNumber, 0.0f, 3.0f);

		//计算车辆与所有RSU之间的大中尺度衰落和
		double wPLSF[gc_RSUNumber];
		for (int RSUIdx = 0; RSUIdx != gc_RSUNumber; RSUIdx++) {
			wPLSF[RSUIdx] = -(wPL[RSUIdx] + wShadow[RSUIdx]);
		}

		//计算出最小的大中尺度衰落
		int FirstRSUIdx, SecondRSUIdx;
		selectMax(wPLSF, gc_RSUNumber, &FirstRSUIdx, &SecondRSUIdx);
		//车辆选择最小衰落的RSU与之通信
		RSUIdx = FirstRSUIdx;

		m_VeUEAry[UserIdx1]->m_IMTA = new IMTA[m_Config.RSUNum];

		m_VeUEAry[UserIdx1]->m_RSUId = RSUIdx;
		m_RSUAry[RSUIdx].m_GTT->m_VeUEIdList.push_back(UserIdx1);
		location.locationType = None;
		location.distance = 0;
		location.distance1 = 0;
		location.distance2 = 0;

		double angle = 0;
		location.manhattan = false;

		location.locationType = Los;
		location.distance = sqrt(pow((m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx].m_GTT->m_AbsX), 2.0f) + pow((m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx].m_GTT->m_AbsY), 2.0f));
		angle = atan2(m_VeUEAry[UserIdx1]->m_AbsY - m_RSUAry[RSUIdx].m_GTT->m_AbsY, m_VeUEAry[UserIdx1]->m_AbsX - m_RSUAry[RSUIdx].m_GTT->m_AbsX) / gc_Degree2PI;

		randomGaussian(location.posCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

		antenna.TxAngle = angle - m_VeUEAry[UserIdx1]->m_FantennaAngle;
		antenna.RxAngle = angle - m_RSUAry[RSUIdx].m_GTT->m_FantennaAngle;
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

		m_VeUEAry[UserIdx1]->m_IMTA[RSUIdx].build(&t_Pl, gc_FC, location, antenna, m_VeUEAry[UserIdx1]->m_V, m_VeUEAry[UserIdx1]->m_VAngle);//计算了结果代入信道模型计算UE之间信道系数
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
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
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
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId]->m_IMTA = new IMTA[m_Config.RSUNum];
	}

	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
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
				location.distance = sqrt(pow((m_VeUEAry[interferenceVeUEId]->m_AbsX - m_RSUAry[RSUIdx].m_GTT->m_AbsX), 2.0f) + pow((m_VeUEAry[interferenceVeUEId]->m_AbsY - m_RSUAry[RSUIdx].m_GTT->m_AbsY), 2.0f));
				angle = atan2(m_VeUEAry[interferenceVeUEId]->m_AbsY - m_RSUAry[RSUIdx].m_GTT->m_AbsY, m_VeUEAry[interferenceVeUEId]->m_AbsX - m_RSUAry[RSUIdx].m_GTT->m_AbsX) / gc_Degree2PI;

				location.eNBAntH = 5;
				location.VeUEAntH = 1.5;
				randomGaussian(location.posCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

				antenna.TxAngle = angle - m_VeUEAry[interferenceVeUEId]->m_FantennaAngle;
				antenna.RxAngle = angle - m_RSUAry[RSUIdx].m_GTT->m_FantennaAngle;
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
				m_VeUEAry[interferenceVeUEId]->m_IMTA[RSUIdx].build(&t_Pl, gc_FC, location, antenna, m_VeUEAry[interferenceVeUEId]->m_V, m_VeUEAry[interferenceVeUEId]->m_VAngle);//计算了结果代入信道模型计算UE之间信道系数
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

	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		Delete::safeDelete(m_VeUEAry[VeUEId]->m_IMTA, true);
	}
}