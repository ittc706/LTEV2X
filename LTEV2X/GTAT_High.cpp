#include<tuple>
#include<iomanip>
#include<iostream>
#include"GTAT_HighSpeed.h"
#include"Exception.h"

using namespace std;
using namespace ns_GTAT_HighSpeed;

default_random_engine GTAT_HighSpeed::s_Engine((unsigned)time(NULL));

GTAT_HighSpeed::GTAT_HighSpeed(int &systemTTI, Configure& systemConfig, eNB* &systemeNBAry, Road* &systemRoadAry, RSU* &systemRSUAry, VeUE* &systemVeUEAry) :
	GTAT_Basic(systemTTI, systemConfig, systemeNBAry, systemRoadAry, systemRSUAry, systemVeUEAry) {}


void GTAT_HighSpeed::configure() {
	m_Config.eNBNum = gc_eNBNumber;
	m_Config.HighSpeedRodeNum = gc_LaneNumber;
	m_Config.RSUNum = gc_RSUNumber;//目前只表示UE RSU数
	m_Config.pupr = new int[m_Config.HighSpeedRodeNum];
	m_Config.VeUENum = 0;
	double Lambda = gc_Length*3.6 / (2.5 * 140);
	srand((unsigned)time(NULL));
	for (int temp = 0; temp != m_Config.HighSpeedRodeNum; ++temp)
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
		//printf("%d\n",k-1);
	}
	m_Config.fv = 140;//车速设定,km/h
}


void GTAT_HighSpeed::initialize() {
	m_eNBAry = new eNB[m_Config.eNBNum];
	m_RoadAry = new Road[m_Config.HighSpeedRodeNum];
	m_VeUEAry = new VeUE[m_Config.VeUENum];
	m_RSUAry = new RSU[m_Config.RSUNum];

	eNBConfigure eNBConfigure;
	for (int temp = 0; temp != m_Config.eNBNum; ++temp)
	{
		eNBConfigure.eNBId = temp;
		m_eNBAry[temp].initializeHighSpeed(eNBConfigure);
	}


	HighSpeedRodeConfigure laneConfigure;
	for (int temp = 0; temp != m_Config.HighSpeedRodeNum; ++temp) {
		laneConfigure.roadId = temp;
		m_RoadAry[temp].initializeHighSpeed(laneConfigure);
	}

	RSUConfigure RSUConfigure;
	for (int RSUIdx = 0; RSUIdx != m_Config.RSUNum; RSUIdx++) {

		RSUConfigure.RSUId = RSUIdx;
		m_RSUAry[RSUIdx].initializeGTAT_HighSpeed(RSUConfigure);
	}

	VeUEConfigure ueConfigure;
	int ueidx = 0;

	for (int LaneIdx = 0; LaneIdx != m_Config.HighSpeedRodeNum; LaneIdx++) {
		for (int uprIdx = 0; uprIdx != m_Config.pupr[LaneIdx]; uprIdx++) {
			ueConfigure.laneId = LaneIdx;
			//ueConfigure.locationID=rand()%conf.ueTopoNum;
			ueConfigure.X = -1732 + rand() % 3465;
			ueConfigure.Y = 0.0f;
			ueConfigure.AbsX = m_RoadAry[LaneIdx].m_GTAT_HighSpeed->m_AbsX + ueConfigure.X;
			ueConfigure.AbsY = m_RoadAry[LaneIdx].m_GTAT_HighSpeed->m_AbsY + ueConfigure.Y;
			ueConfigure.V = m_Config.fv;
			m_VeUEAry[ueidx++].initializeGTAT_HighSpeed(ueConfigure);
		}
	}
}


void GTAT_HighSpeed::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		for (auto &c : m_VeUEAry[VeUEId].m_GTAT->m_InterferenceH) {
			if (c != nullptr){
				delete[] c;
				c = nullptr;
			}
		}
	}
}


void GTAT_HighSpeed::channelGeneration() {
	//RSU.m_VeUEIdList是在freshLoc函数内生成的，因此需要在更新位置前清空这个列表
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_GTAT->m_VeUEIdList.clear();
		for (int clusterIdx = 0; clusterIdx < _RSU.m_GTAT->m_ClusterNum; clusterIdx++) {
			_RSU.m_GTAT->m_ClusterVeUEIdList[clusterIdx].clear();
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
		for (int VeUEId : _RSU.m_GTAT->m_VeUEIdList) {
			int clusterIdx = m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx;
			_RSU.m_GTAT->m_ClusterVeUEIdList[clusterIdx].push_back(VeUEId);
		}
	}

	//记录并更新每辆车的位置日志
	for (int VeUEId = 0; VeUEId<m_Config.VeUENum; VeUEId++)
		m_VeUEAry[VeUEId].m_TMC->m_LocationUpdateLogInfoList.push_back(tuple<int, int>(m_VeUEAry[VeUEId].m_GTAT->m_RSUId, m_VeUEAry[VeUEId].m_GTAT->m_ClusterIdx));

	//记录RSU内车辆的数目
	vector<int> curVeUENum;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		RSU &_RSU = m_RSUAry[RSUId];
		curVeUENum.push_back(static_cast<int>(_RSU.m_GTAT->m_VeUEIdList.size()));
	}
	m_VeUENumPerRSU.push_back(curVeUENum);


	//UNDONE
	//更新基站的VeUE容器
	for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
		eNB &_eNB = m_eNBAry[eNBId];
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int VeUEId : m_RSUAry[RSUId].m_GTAT->m_VeUEIdList) {
				_eNB.m_VeUEIdList.push_back(VeUEId);
			}
		}
	}
	//UNDONE
}


void GTAT_HighSpeed::freshLoc() {
	for (int UserIdx = 0; UserIdx != m_Config.VeUENum; UserIdx++)
	{

		if (m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_VAngle == 0)
		{
			m_VeUEAry[UserIdx].m_GTAT->m_ClusterIdx = 0;//由西向东车辆簇编号为0

			if ((m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX + gc_FreshTime*m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_V)>(gc_Length / 2))
			{
				m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX = (m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX + gc_FreshTime*m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_V) - gc_Length;
				m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_X = m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX;
			}
			else
			{
				m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX = m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX + gc_FreshTime*m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_V;
				m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_X = m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX;
			}
		}
		else
		{
			m_VeUEAry[UserIdx].m_GTAT->m_ClusterIdx = 1;//由东向西车辆簇编号为1

			if ((m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX - gc_FreshTime*m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_V)<(-gc_Length / 2))
			{
				m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX = (m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX - gc_FreshTime*m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_V) + gc_Length;
				m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_X = m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX;
			}
			else
			{
				m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX = m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX - gc_FreshTime*m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_V;
				m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_X = m_VeUEAry[UserIdx].m_GTAT_HighSpeed->m_AbsX;
			}
		}



	}


	Location location;
	Antenna antenna;


	int RSUIdx = 0;
	int ClusterID = 0;
	for (int UserIdx1 = 0; UserIdx1 != m_Config.VeUENum; UserIdx1++)
	{
		m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_IMTA = new IMTA[m_Config.RSUNum];
		RSUIdx = 17 - int(m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_AbsX / 100 + 0.5);
		m_VeUEAry[UserIdx1].m_GTAT->m_RSUId = RSUIdx;
		m_RSUAry[RSUIdx].m_GTAT->m_VeUEIdList.push_back(UserIdx1);
		location.eType = None;
		location.distance = 0;
		location.distance1 = 0;
		location.distance2 = 0;

		double angle = 0;
		location.bManhattan = false;

		location.eType = Los;
		location.distance = sqrt(pow((m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_AbsX - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_AbsX), 2.0f) + pow((m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_AbsY - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_AbsY), 2.0f));
		angle = atan2(m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_AbsY - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_AbsY, m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_AbsX - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_AbsX) / gc_Degree2PI;

		location.eNBAntH = 5;
		location.VeUEAntH = 1.5;
		randomGaussian(location.afPosCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

		antenna.fTxAngle = angle - m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_FantennaAngle;
		antenna.fRxAngle = angle - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_FantennaAngle;
		antenna.fAntGain = 3;
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

		m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_IMTA[RSUIdx].build(&t_Pl, gc_FC, location, antenna, m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_V, m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_VAngle);//计算了结果代入信道模型计算UE之间信道系数
		bool *flag = new bool();

		m_VeUEAry[UserIdx1].m_GTAT->m_Ploss = t_Pl;

		*flag = true;
		m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_IMTA[RSUIdx].enable(flag);

		double *H = new double[1 * 2 * 19 * 2];
		double *FFT = new double[1 * 2 * 1024 * 2];
		double *ch_buffer = new double[1 * 2 * 19 * 20];
		double *ch_sin = new double[1 * 2 * 19 * 20];
		double *ch_cos = new double[1 * 2 * 19 * 20];

		double *t_HAfterFFT = new double[2 * 1024 * 2];

		m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_IMTA[RSUIdx].calculate(t_HAfterFFT, 0.01f, ch_buffer, ch_sin, ch_cos, H, FFT);
		memcpy(m_VeUEAry[UserIdx1].m_GTAT->m_H, t_HAfterFFT, 2 * 1024 * 2 * sizeof(double(0)));
	

		delete flag;
		delete[] H;
		delete[] ch_buffer;
		delete[] ch_sin;
		delete[] ch_cos;
		delete[] antenna.pfTxSlantAngle;
		delete[] antenna.pfTxAntSpacing;
		delete[] antenna.pfRxSlantAngle;
		delete[] antenna.pfRxAntSpacing;
		delete[] m_VeUEAry[UserIdx1].m_GTAT_HighSpeed->m_IMTA;
		delete[] FFT;
		delete[] t_HAfterFFT;
	}
}


void GTAT_HighSpeed::writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) {
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


void GTAT_HighSpeed::calculateInterference(const std::vector<std::list<int>>& RRMInterferenceVec) {
	for (int patternIdx = 0; patternIdx < RRMInterferenceVec.size(); patternIdx++) {
		const list<int> &lst = RRMInterferenceVec[patternIdx];

		for (int VeUEId : lst) {
			m_VeUEAry[VeUEId].m_GTAT_HighSpeed->m_IMTA = new IMTA[m_Config.RSUNum];
		}

		for (int VeUEId : lst) {
			for(int interferenceVeUEId: lst){

				if (interferenceVeUEId == VeUEId) continue;

				if (m_VeUEAry[VeUEId].m_GTAT->m_InterferenceH[interferenceVeUEId] != nullptr) continue;

				m_VeUEAry[VeUEId].m_GTAT->m_InterferenceH[interferenceVeUEId] = new double[2 * 1024 * 2];

				Location location;
				Antenna antenna;


				int RSUIdx = m_VeUEAry[VeUEId].m_GTAT->m_RSUId;
				location.eType = None;
				location.distance = 0;
				location.distance1 = 0;
				location.distance2 = 0;

				double angle = 0;
				location.bManhattan = false;

				location.eType = Los;
				location.distance = sqrt(pow((m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_AbsX - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_AbsX), 2.0f) + pow((m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_AbsY - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_AbsY), 2.0f));
				angle = atan2(m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_AbsY - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_AbsY, m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_AbsX - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_AbsX) / gc_Degree2PI;

				location.eNBAntH = 5;
				location.VeUEAntH = 1.5;
				randomGaussian(location.afPosCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

				antenna.fTxAngle = angle - m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_FantennaAngle;
				antenna.fRxAngle = angle - m_RSUAry[RSUIdx].m_GTAT_HighSpeed->m_FantennaAngle;
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
				m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_IMTA[RSUIdx].build(&t_Pl, gc_FC, location, antenna, m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_V, m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_VAngle);//计算了结果代入信道模型计算UE之间信道系数
				bool *flag = new bool();


				m_VeUEAry[VeUEId].m_GTAT->m_InterferencePloss[interferenceVeUEId] = t_Pl;


				*flag = true;
				m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_IMTA[RSUIdx].enable(flag);
				double *H = new double[1 * 2 * 19 * 2];
				double *FFT = new double[1 * 2 * 1024 * 2];
				double *ch_buffer = new double[1 * 2 * 19 * 20];
				double *ch_sin = new double[1 * 2 * 19 * 20];
				double *ch_cos = new double[1 * 2 * 19 * 20];

				double *t_HAfterFFT = new double[2 * 1024 * 2];

				m_VeUEAry[interferenceVeUEId].m_GTAT_HighSpeed->m_IMTA[RSUIdx].calculate(t_HAfterFFT, 0.01f, ch_buffer, ch_sin, ch_cos, H, FFT);


				memcpy(m_VeUEAry[VeUEId].m_GTAT->m_InterferenceH[interferenceVeUEId], t_HAfterFFT, 2 * 1024 * 2 * sizeof(double(0)));

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
			delete[] m_VeUEAry[VeUEId].m_GTAT_HighSpeed->m_IMTA;
		}
	}
}