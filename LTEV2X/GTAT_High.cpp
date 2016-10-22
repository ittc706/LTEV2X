#include<tuple>
#include<iomanip>
#include<iostream>
#include"GTAT_High.h"
#include"Exception.h"

using namespace std;
using namespace ns_GTAT_High;

default_random_engine GTAT_High::s_Engine((unsigned)time(NULL));

GTAT_High::GTAT_High(int &systemTTI, sConfigure& systemConfig, ceNB* &systemeNBAry, cRoad* &systemRoadAry, cRSU* &systemRSUAry, cVeUE* &systemVeUEAry, cLane* &systemLaneAry) :
	GTAT_Basic(systemTTI, systemConfig, systemeNBAry, systemRoadAry, systemRSUAry, systemVeUEAry), m_LaneAry(systemLaneAry){}


void GTAT_High::configure() {
	m_Config.eNBNum = c_eNBNumber;
	m_Config.LaneNum = c_laneNumber;
	m_Config.RSUNum = c_RSUNumber;//目前只表示UE RSU数
	m_Config.pupr = new int[m_Config.LaneNum];
	m_Config.VeUENum = 0;
	double Lambda = c_length*3.6 / (2.5 * 140);
	srand((unsigned)time(NULL));
	for (unsigned short temp = 0; temp != m_Config.LaneNum; ++temp)
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


void GTAT_High::initialize() {
	m_eNBAry = new ceNB[m_Config.eNBNum];
	m_LaneAry = new cLane[m_Config.LaneNum];
	m_VeUEAry = new cVeUE[m_Config.VeUENum];
	m_RSUAry = new cRSU[m_Config.RSUNum];

	seNBConfigure eNBConfigure;
	for (unsigned short temp = 0; temp != m_Config.eNBNum; ++temp)
	{
		eNBConfigure.weNBID = temp;
		m_eNBAry[temp].initializeHigh(eNBConfigure);
	}


	sLaneConfigure laneConfigure;
	for (unsigned short temp = 0; temp != m_Config.LaneNum; ++temp) {
		laneConfigure.wLaneID = temp;
		m_LaneAry[temp].initializeHigh(laneConfigure);
	}

	sRSUConfigure RSUConfigure;
	for (unsigned short RSUIdx = 0; RSUIdx != m_Config.RSUNum; RSUIdx++) {

		RSUConfigure.wRSUID = RSUIdx;
		m_RSUAry[RSUIdx].initializeHigh(RSUConfigure);
	}

	sUEConfigure ueConfigure;
	int ueidx = 0;

	for (unsigned short LaneIdx = 0; LaneIdx != m_Config.LaneNum; LaneIdx++) {
		for (int uprIdx = 0; uprIdx != m_Config.pupr[LaneIdx]; uprIdx++) {
			ueConfigure.wLaneID = LaneIdx;
			//ueConfigure.locationID=rand()%conf.ueTopoNum;
			ueConfigure.fX = -1732 + rand() % 3465;
			ueConfigure.fY = 0.0f;
			ueConfigure.fAbsX = m_LaneAry[LaneIdx].m_fAbsX + ueConfigure.fX;
			ueConfigure.fAbsY = m_LaneAry[LaneIdx].m_fAbsY + ueConfigure.fY;
			ueConfigure.fv = m_Config.fv;
			m_VeUEAry[ueidx++].initializeHigh(ueConfigure);
		}
	}
}


void GTAT_High::channelGeneration() {
	//RSU.m_VeUEIdList是在freshLoc函数内生成的，因此需要在更新位置前清空这个列表
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_VeUEIdList.clear();
		for (int clusterIdx = 0; clusterIdx < _RSU.m_DRAClusterNum; clusterIdx++) {
			_RSU.m_DRAClusterVeUEIdList[clusterIdx].clear();
		}
	}
	//同时也清除eNB.m_VeUEIdList
	for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++)
		m_eNBAry[eNBId].m_VeUEIdList.clear();

	//运动模型
	freshLoc();

	//将更新后的RSU.m_VeUEIdList压入对应的簇中
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		for (int VeUEId : _RSU.m_VeUEIdList) {
			int clusterIdx = m_VeUEAry[VeUEId].m_ClusterIdx;
			_RSU.m_DRAClusterVeUEIdList[clusterIdx].push_back(VeUEId);
		}
	}

	//记录并更新每辆车的位置日志
	for (int VeUEId = 0; VeUEId<m_Config.VeUENum; VeUEId++)
		m_VeUEAry[VeUEId].m_LocationUpdateLogInfoList.push_back(tuple<int, int>(m_VeUEAry[VeUEId].m_RSUId, m_VeUEAry[VeUEId].m_ClusterIdx));

	//记录RSU内车辆的数目
	vector<int> curVeUENum;
	for (int RSUId = 0; RSUId < m_Config.RSUNum; RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		curVeUENum.push_back(static_cast<int>(_RSU.m_VeUEIdList.size()));
	}
	m_VeUENumPerRSU.push_back(curVeUENum);


	//UNDONE
	//更新基站的VeUE容器
	for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
		ceNB &_eNB = m_eNBAry[eNBId];
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int VeUEId : m_RSUAry[RSUId].m_VeUEIdList) {
				_eNB.m_VeUEIdList.push_back(VeUEId);
			}
		}
	}
	//UNDONE
}


void GTAT_High::freshLoc() {
	for (int UserIdx = 0; UserIdx != m_Config.VeUENum; UserIdx++)
	{

		if (m_VeUEAry[UserIdx].m_fvAngle == 0)
		{
			m_VeUEAry[UserIdx].m_ClusterIdx = 0;//由西向东车辆簇编号为0

			if ((m_VeUEAry[UserIdx].m_fAbsX + Fresh_time*m_VeUEAry[UserIdx].m_fv)>(c_length / 2))
			{
				m_VeUEAry[UserIdx].m_fAbsX = (m_VeUEAry[UserIdx].m_fAbsX + Fresh_time*m_VeUEAry[UserIdx].m_fv) - c_length;
				m_VeUEAry[UserIdx].m_fX = m_VeUEAry[UserIdx].m_fAbsX;
			}
			else
			{
				m_VeUEAry[UserIdx].m_fAbsX = m_VeUEAry[UserIdx].m_fAbsX + Fresh_time*m_VeUEAry[UserIdx].m_fv;
				m_VeUEAry[UserIdx].m_fX = m_VeUEAry[UserIdx].m_fAbsX;
			}
		}
		else
		{
			m_VeUEAry[UserIdx].m_ClusterIdx = 1;//由东向西车辆簇编号为1

			if ((m_VeUEAry[UserIdx].m_fAbsX - Fresh_time*m_VeUEAry[UserIdx].m_fv)<(-c_length / 2))
			{
				m_VeUEAry[UserIdx].m_fAbsX = (m_VeUEAry[UserIdx].m_fAbsX - Fresh_time*m_VeUEAry[UserIdx].m_fv) + c_length;
				m_VeUEAry[UserIdx].m_fX = m_VeUEAry[UserIdx].m_fAbsX;
			}
			else
			{
				m_VeUEAry[UserIdx].m_fAbsX = m_VeUEAry[UserIdx].m_fAbsX - Fresh_time*m_VeUEAry[UserIdx].m_fv;
				m_VeUEAry[UserIdx].m_fX = m_VeUEAry[UserIdx].m_fAbsX;
			}
		}



	}


	sLocation location;
	sAntenna antenna;


	unsigned short RSUIdx = 0;
	unsigned short ClusterID = 0;
	for (int UserIdx1 = 0; UserIdx1 != m_Config.VeUENum; UserIdx1++)
	{
		m_VeUEAry[UserIdx1].imta = new cIMTA[m_Config.RSUNum];
		RSUIdx = 17 - int(m_VeUEAry[UserIdx1].m_fAbsX / 100 + 0.5);
		m_VeUEAry[UserIdx1].m_RSUId = RSUIdx;
		m_RSUAry[RSUIdx].m_VeUEIdList.push_back(UserIdx1);
		location.eType = None;
		location.fDistance = 0;
		location.fDistance1 = 0;
		location.fDistance2 = 0;

		double angle = 0;
		location.bManhattan = false;

		location.eType = Los;
		location.fDistance = sqrt(pow((m_VeUEAry[UserIdx1].m_fAbsX - m_RSUAry[RSUIdx].m_fAbsX), 2.0f) + pow((m_VeUEAry[UserIdx1].m_fAbsY - m_RSUAry[RSUIdx].m_fAbsY), 2.0f));
		angle = atan2(m_VeUEAry[UserIdx1].m_fAbsY - m_RSUAry[RSUIdx].m_fAbsY, m_VeUEAry[UserIdx1].m_fAbsX - m_RSUAry[RSUIdx].m_fAbsX) / c_Degree2PI;

		location.feNBAntH = 5;
		location.fUEAntH = 1.5;
		RandomGaussian(location.afPosCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

		antenna.fTxAngle = angle - m_VeUEAry[UserIdx1].m_fantennaAngle;
		antenna.fRxAngle = angle - m_RSUAry[RSUIdx].m_fantennaAngle;
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

		m_VeUEAry[UserIdx1].imta[RSUIdx].Build(&t_Pl, c_FC, location, antenna, m_VeUEAry[UserIdx1].m_fv, m_VeUEAry[UserIdx1].m_fvAngle);//计算了结果代入信道模型计算UE之间信道系数
		bool *flag = new bool();

		m_VeUEAry[UserIdx1].m_Ploss = t_Pl;

		*flag = true;
		m_VeUEAry[UserIdx1].imta[RSUIdx].Enable(flag);

		double *H = new double[1 * 2 * 19 * 2];
		double *FFT = new double[1 * 2 * 1024 * 2];
		double *ch_buffer = new double[1 * 2 * 19 * 20];
		double *ch_sin = new double[1 * 2 * 19 * 20];
		double *ch_cos = new double[1 * 2 * 19 * 20];

		double *t_HAfterFFT = new double[2 * 1024 * 2];

		m_VeUEAry[UserIdx1].imta[RSUIdx].Calculate(t_HAfterFFT, 0.01f, ch_buffer, ch_sin, ch_cos, H, FFT);
		memcpy(m_VeUEAry[UserIdx1].m_H, t_HAfterFFT, 2 * 1024 * 2 * sizeof(0.0f));
	

		delete flag;
		delete[] H;
		delete[]ch_buffer;
		delete[]ch_sin;
		delete[]ch_cos;
		delete[]antenna.pfTxSlantAngle;
		delete[]antenna.pfTxAntSpacing;
		delete[]antenna.pfRxSlantAngle;
		delete[]antenna.pfRxAntSpacing;
		delete[]m_VeUEAry[UserIdx1].imta;
		delete[] FFT;
		delete[]t_HAfterFFT;
	}
}


void GTAT_High::writeVeUELocationUpdateLogInfo(std::ofstream &out1, std::ofstream &out2) {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		out1 << "VeUE[ " << left << setw(3) << VeUEId << "]" << endl;
		out1 << "{" << endl;
		for (const tuple<int, int> &t : m_VeUEAry[VeUEId].m_LocationUpdateLogInfoList)
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


void GTAT_High::calculateInter(std::vector<int> transimitingVeUEId) {
	for (int VeUEId : transimitingVeUEId) {
		m_VeUEAry[VeUEId].imta = new cIMTA[m_Config.RSUNum];
	}
	for (int VeUEId : transimitingVeUEId) {
		m_VeUEAry[VeUEId].m_InterferencePloss.assign(m_VeUEAry[VeUEId].m_InterVeUENum, 0);

		delete[]m_VeUEAry[VeUEId].m_InterferenceH;
		m_VeUEAry[VeUEId].m_InterferenceH = new double[m_VeUEAry[VeUEId].m_InterVeUENum * 2 * 1024 * 2];

		for (int count = 0; count != m_VeUEAry[VeUEId].m_InterVeUENum; count++)
		{
			int interUserIdx = m_VeUEAry[VeUEId].m_InterVeUEVec[count];
			sLocation location;
			sAntenna antenna;


			unsigned short RSUIdx = m_VeUEAry[VeUEId].m_RSUId;
			location.eType = None;
			location.fDistance = 0;
			location.fDistance1 = 0;
			location.fDistance2 = 0;

			double angle = 0;
			location.bManhattan = false;

			location.eType = Los;
			location.fDistance = sqrt(pow((m_VeUEAry[interUserIdx].m_fAbsX - m_RSUAry[RSUIdx].m_fAbsX), 2.0f) + pow((m_VeUEAry[interUserIdx].m_fAbsY - m_RSUAry[RSUIdx].m_fAbsY), 2.0f));
			angle = atan2(m_VeUEAry[interUserIdx].m_fAbsY - m_RSUAry[RSUIdx].m_fAbsY, m_VeUEAry[interUserIdx].m_fAbsX - m_RSUAry[RSUIdx].m_fAbsX) / c_Degree2PI;

			location.feNBAntH = 5;
			location.fUEAntH = 1.5;
			RandomGaussian(location.afPosCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

			antenna.fTxAngle = angle - m_VeUEAry[interUserIdx].m_fantennaAngle;
			antenna.fRxAngle = angle - m_RSUAry[RSUIdx].m_fantennaAngle;
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
			m_VeUEAry[interUserIdx].imta[RSUIdx].Build(&t_Pl, c_FC, location, antenna, m_VeUEAry[interUserIdx].m_fv, m_VeUEAry[interUserIdx].m_fvAngle);//计算了结果代入信道模型计算UE之间信道系数
			bool *flag = new bool();


			m_VeUEAry[VeUEId].m_InterferencePloss[count] = t_Pl;


			*flag = true;
			m_VeUEAry[interUserIdx].imta[RSUIdx].Enable(flag);
			double *H = new double[1 * 2 * 19 * 2];
			double *FFT = new double[1 * 2 * 1024 * 2];
			double *ch_buffer = new double[1 * 2 * 19 * 20];
			double *ch_sin = new double[1 * 2 * 19 * 20];
			double *ch_cos = new double[1 * 2 * 19 * 20];

			double *t_HAfterFFT = new double[2 * 1024 * 2];

			m_VeUEAry[interUserIdx].imta[RSUIdx].Calculate(t_HAfterFFT, 0.01f, ch_buffer, ch_sin, ch_cos, H, FFT);


			memcpy(&m_VeUEAry[VeUEId].m_InterferenceH[count * 2 * 1024 * 2], t_HAfterFFT, 2 * 1024 * 2 * sizeof(0.0f));

			delete flag;
			delete[] H;
			delete[]ch_buffer;
			delete[]ch_sin;
			delete[]ch_cos;
			delete[]antenna.pfTxSlantAngle;
			delete[]antenna.pfTxAntSpacing;
			delete[]antenna.pfRxSlantAngle;
			delete[]antenna.pfRxAntSpacing;
			delete[] FFT;
			delete[]t_HAfterFFT;
		}
	}
	for (int VeUEId : transimitingVeUEId) {
		delete[]m_VeUEAry[VeUEId].imta;
	}
}