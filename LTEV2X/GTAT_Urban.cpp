#include<tuple>
#include<iomanip>
#include"GTAT_Urban.h"

using namespace std;

default_random_engine GTAT_Urban::s_Engine((unsigned)time(NULL));

GTAT_Urban::GTAT_Urban(int &systemTTI, sConfigure& systemConfig, ceNB* &systemeNBAry, cRoad* &systemRoadAry, cRSU* &systemRSUAry, cVeUE* &systemVeUEAry) :
	GTAT_Basic(systemTTI, systemConfig, systemeNBAry, systemRoadAry, systemRSUAry, systemVeUEAry) {}


void GTAT_Urban::configure() {
	m_Config.eNBNum = c_eNBNumber;
	m_Config.RoadNum = c_roadNumber;
	m_Config.RSUNum = c_RSUNumber;//目前只表示UE RSU数
	m_Config.pupr = new int[m_Config.RoadNum];
	m_Config.VeUENum = 0;
	int Lambda = static_cast<int>((c_length + c_wide) * 2 * 3.6 / (2.5 * 15));
	//srand((unsigned)time(NULL));
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
	}
	m_Config.wxNum = 36;
	m_Config.wyNum = 62;
	m_Config.ueTopoNum = (m_Config.wxNum + m_Config.wyNum) * 2 - 4;
	m_Config.pueTopo = new float[m_Config.ueTopoNum * 2];//重合了4个
	float temp_x = -(float)c_wide / 2 + c_lane_wide;
	float temp_y = -(float)c_length / 2 + c_lane_wide;
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


void GTAT_Urban::initialize() {
	m_eNBAry = new ceNB[m_Config.eNBNum];
	m_RoadAry = new cRoad[m_Config.RoadNum];
	m_VeUEAry = new cVeUE[m_Config.VeUENum];
	m_RSUAry = new cRSU[m_Config.RSUNum];

	sRoadConfigure roadConfigure;
	for (unsigned short temp = 0; temp != m_Config.RoadNum; ++temp)
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

	sRSUConfigure RSUConfigure;
	for (unsigned short RSUIdx = 0; RSUIdx != m_Config.RSUNum; RSUIdx++)
	{

		RSUConfigure.wRSUID = RSUIdx;
		m_RSUAry[RSUIdx].initialize(RSUConfigure);
	}

	sUEConfigure ueConfigure;
	int ueidx = 0;

	for (unsigned short RoadIdx = 0; RoadIdx != m_Config.RoadNum; RoadIdx++)
	{

		for (int uprIdx = 0; uprIdx != m_Config.pupr[RoadIdx]; uprIdx++)
		{
			ueConfigure.wRoadID = RoadIdx;
			ueConfigure.locationID = rand() % m_Config.ueTopoNum;
			ueConfigure.fX = m_Config.pueTopo[ueConfigure.locationID * 2 + 0];
			ueConfigure.fY = m_Config.pueTopo[ueConfigure.locationID * 2 + 1];
			ueConfigure.fAbsX = m_RoadAry[RoadIdx].m_fAbsX + ueConfigure.fX;
			ueConfigure.fAbsY = m_RoadAry[RoadIdx].m_fAbsY + ueConfigure.fY;
			ueConfigure.fv = m_Config.fv;
			m_VeUEAry[ueidx++].initialize(ueConfigure);

		}
	}
}


void GTAT_Urban::channelGeneration() {
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


void GTAT_Urban::freshLoc() {
	for (int UserIdx = 0; UserIdx != m_Config.VeUENum; UserIdx++)
	{
		bool RoadChangeFlag = false;
		int temp;
		switch (m_VeUEAry[UserIdx].m_locationID)
		{
		case 191:
			m_VeUEAry[UserIdx].m_locationID = 0;
			break;
		case 0:
			temp = rand() % 4;
			if (temp == 0)//turn left
			{
				RoadChangeFlag = true;
				m_VeUEAry[UserIdx].m_wRoadID = c_WrapAroundRoad[m_VeUEAry[UserIdx].m_wRoadID][4];
				m_VeUEAry[UserIdx].m_locationID = 96;
				m_VeUEAry[UserIdx].m_fvAngle = -90;
			}
			else
				if (temp == 2)//turn right
				{
					m_VeUEAry[UserIdx].m_locationID = m_VeUEAry[UserIdx].m_locationID + 1;
					m_VeUEAry[UserIdx].m_fvAngle = 90;
				}
				else//go straight，不改变方向
				{
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx].m_wRoadID = c_WrapAroundRoad[m_VeUEAry[UserIdx].m_wRoadID][5];
					m_VeUEAry[UserIdx].m_locationID = 157;
				}
			break;
		case 61:
			temp = rand() % 4;
			if (temp == 0)//turn left
			{
				RoadChangeFlag = true;
				m_VeUEAry[UserIdx].m_wRoadID = c_WrapAroundRoad[m_VeUEAry[UserIdx].m_wRoadID][6];
				m_VeUEAry[UserIdx].m_locationID = 157;
				m_VeUEAry[UserIdx].m_fvAngle = -180;
			}
			else
				if (temp == 2)
				{
					m_VeUEAry[UserIdx].m_locationID = m_VeUEAry[UserIdx].m_locationID + 1;
					m_VeUEAry[UserIdx].m_fvAngle = 0;
				}
				else
				{
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx].m_wRoadID = c_WrapAroundRoad[m_VeUEAry[UserIdx].m_wRoadID][7];
					m_VeUEAry[UserIdx].m_locationID = 0;
					m_VeUEAry[UserIdx].m_fvAngle = 270;
				}
			break;
		case 96:
			temp = rand() % 4;
			if (temp == 0)//turn left
			{
				RoadChangeFlag = true;
				m_VeUEAry[UserIdx].m_wRoadID = c_WrapAroundRoad[m_VeUEAry[UserIdx].m_wRoadID][8];
				m_VeUEAry[UserIdx].m_locationID = 0;
				m_VeUEAry[UserIdx].m_fvAngle = 90;
			}
			else
				if (temp == 2)
				{
					m_VeUEAry[UserIdx].m_locationID = m_VeUEAry[UserIdx].m_locationID + 1;
					m_VeUEAry[UserIdx].m_fvAngle = -90;
				}
				else
				{
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx].m_wRoadID = c_WrapAroundRoad[m_VeUEAry[UserIdx].m_wRoadID][1];
					m_VeUEAry[UserIdx].m_locationID = 61;
				}
			break;
		case 157:
			temp = rand() % 4;
			if (temp == 0)//turn left
			{
				RoadChangeFlag = true;
				m_VeUEAry[UserIdx].m_wRoadID = c_WrapAroundRoad[m_VeUEAry[UserIdx].m_wRoadID][2];
				m_VeUEAry[UserIdx].m_locationID = 61;
				m_VeUEAry[UserIdx].m_fvAngle = 0;
			}
			else
				if (temp == 2)
				{
					m_VeUEAry[UserIdx].m_locationID = m_VeUEAry[UserIdx].m_locationID + 1;
					m_VeUEAry[UserIdx].m_fvAngle = -180;
				}
				else
				{
					RoadChangeFlag = true;
					m_VeUEAry[UserIdx].m_wRoadID = c_WrapAroundRoad[m_VeUEAry[UserIdx].m_wRoadID][3];
					m_VeUEAry[UserIdx].m_locationID = 96;
				}
			break;
		default:
			m_VeUEAry[UserIdx].m_locationID = m_VeUEAry[UserIdx].m_locationID + 1;
			break;
		}
		m_VeUEAry[UserIdx].m_fX = m_Config.pueTopo[m_VeUEAry[UserIdx].m_locationID * 2 + 0];
		m_VeUEAry[UserIdx].m_fY = m_Config.pueTopo[m_VeUEAry[UserIdx].m_locationID * 2 + 1];
		m_VeUEAry[UserIdx].m_fAbsX = m_RoadAry[m_VeUEAry[UserIdx].m_wRoadID].m_fAbsX + m_VeUEAry[UserIdx].m_fX;
		m_VeUEAry[UserIdx].m_fAbsY = m_RoadAry[m_VeUEAry[UserIdx].m_wRoadID].m_fAbsY + m_VeUEAry[UserIdx].m_fY;

	}


	sLocation location;
	sAntenna antenna;
	location.bManhattan = true;

	unsigned short RSUIdx = 0;
	unsigned short ClusterID = 0;
	for (int UserIdx1 = 0; UserIdx1 != m_Config.VeUENum; UserIdx1++)
	{
		m_VeUEAry[UserIdx1].imta = new cIMTA[m_Config.RSUNum];
		switch (m_VeUEAry[UserIdx1].m_locationID / 10)
		{
		case 1:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][8];
			ClusterID = 2;
			break;
		case 2:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][8];
			ClusterID = 1;
			break;
		case 3:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][9];
			ClusterID = 2;
			break;
		case 4:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][9];
			ClusterID = 1;
			break;
		case 5:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][0];
			ClusterID = 4;
			break;
		case 6:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][0];
			ClusterID = 3;
			break;
		case 7:
		case 8:
		case 9:
			if (m_VeUEAry[UserIdx1].m_locationID<87)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][1];
				if ((m_VeUEAry[UserIdx1].m_locationID<78))  ClusterID = 3;
				else ClusterID = 2;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][2];
				if ((m_VeUEAry[UserIdx1].m_locationID<96))  ClusterID = 6;
				else ClusterID = 5;
			}
			break;
		case 10:
		case 11:
		case 12:
			if (m_VeUEAry[UserIdx1].m_locationID<106)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][2];
				ClusterID = 5;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][3];
				if ((m_VeUEAry[UserIdx1].m_locationID<116))  ClusterID = 0;
				else ClusterID = 3;
			}
			break;
		case 13:
			if (m_VeUEAry[UserIdx1].m_locationID<126)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][3];
				ClusterID = 3;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][4];
				if ((m_VeUEAry[UserIdx1].m_locationID<136))  ClusterID = 0;
				else ClusterID = 3;
			}
			break;
		case 14:
			if (m_VeUEAry[UserIdx1].m_locationID<146)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][4];
				ClusterID = 3;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][5];
				ClusterID = 0;
			}
			break;
		case 15:
		case 16:
		case 17:
			if (m_VeUEAry[UserIdx1].m_locationID<165)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][5];
				if ((m_VeUEAry[UserIdx1].m_locationID<157))  ClusterID = 0;
				else ClusterID = 7;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][6];
				if ((m_VeUEAry[UserIdx1].m_locationID<174))  ClusterID = 1;
				else ClusterID = 0;
			}
			break;
		case 18:
		case 19:
			if (m_VeUEAry[UserIdx1].m_locationID<183)
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][6];
				ClusterID = 0;
			}
			else
			{
				RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][7];
				ClusterID = 2;
			}
			break;
		case 0:
			RSUIdx = c_RSUInRoad[m_VeUEAry[UserIdx1].m_wRoadID][7];
			ClusterID = 1;
			break;
		default:
			break;
		}
		m_VeUEAry[UserIdx1].m_RSUId = RSUIdx;
		m_VeUEAry[UserIdx1].m_ClusterIdx = ClusterID;
		m_RSUAry[RSUIdx].m_VeUEIdList.push_back(UserIdx1);
		location.eType = None;
		location.fDistance = 0;
		location.fDistance1 = 0;
		location.fDistance2 = 0;

		float angle = 0;
		if (location.bManhattan == true)  //计算location distance
		{
			//if (abs(veUE[UserIdx1].m_fAbsX - RSU[RSUIdx].m_fAbsX) <= 10.5 || abs(veUE[UserIdx1].m_fAbsY - RSU[RSUIdx].m_fAbsY) <= 10.5)
			//{
			location.eType = Los;
			location.fDistance = sqrt(pow((m_VeUEAry[UserIdx1].m_fAbsX - m_RSUAry[RSUIdx].m_fAbsX), 2.0f) + pow((m_VeUEAry[UserIdx1].m_fAbsY - m_RSUAry[RSUIdx].m_fAbsY), 2.0f));
			angle = atan2(m_VeUEAry[UserIdx1].m_fAbsY - m_RSUAry[RSUIdx].m_fAbsY, m_VeUEAry[UserIdx1].m_fAbsX - m_RSUAry[RSUIdx].m_fAbsX) / c_Degree2PI;
			//}
			//else
			//{
			//	location.eType = Nlos;
			//	location.fDistance1 = abs(veUE[UserIdx1].m_fAbsX - RSU[RSUIdx].m_fAbsX);
			//	location.fDistance2 = abs(veUE[UserIdx1].m_fAbsY - RSU[RSUIdx].m_fAbsY);
			//	location.fDistance = sqrt(pow(location.fDistance1,2.0f)+pow(location.fDistance2,2.0f)); 

			//}
		}
		//	fprintf(fp, "%f\n", location.fDistance);
		location.feNBAntH = 5;
		location.fUEAntH = 1.5;
		RandomGaussian(location.afPosCor, 5, 0.0f, 1.0f);//产生高斯随机数，为后面信道系数使用。

		antenna.fTxAngle = angle - m_VeUEAry[UserIdx1].m_fantennaAngle;
		antenna.fRxAngle = angle - m_RSUAry[RSUIdx].m_fantennaAngle;
		antenna.fAntGain = 3;
		antenna.fMaxAttenu = 23;
		antenna.byTxAntNum = 1;
		antenna.byRxAntNum = 2;
		antenna.pfTxSlantAngle = new float[antenna.byTxAntNum];
		antenna.pfTxAntSpacing = new float[antenna.byTxAntNum];
		antenna.pfRxSlantAngle = new float[antenna.byRxAntNum];
		antenna.pfRxAntSpacing = new float[antenna.byRxAntNum];
		antenna.pfTxSlantAngle[0] = 90.0f;
		antenna.pfTxAntSpacing[0] = 0.0f;
		antenna.pfRxSlantAngle[0] = 90.0f;
		antenna.pfRxSlantAngle[1] = 90.0f;
		antenna.pfRxAntSpacing[0] = 0.0f;
		antenna.pfRxAntSpacing[1] = 0.5f;


		m_VeUEAry[UserIdx1].imta[RSUIdx].Build(c_FC, location, antenna, m_VeUEAry[UserIdx1].m_fv, m_VeUEAry[UserIdx1].m_fvAngle);//计算了结果代入信道模型计算UE之间信道系数
		bool *flag = new bool();
		*flag = true;
		m_VeUEAry[UserIdx1].imta[RSUIdx].Enable(flag);
		float *H = new float[1 * 2 * 12 * 20 * 2];
		float *ch_buffer = new float[1 * 2 * 12 * 20];
		float *ch_sin = new float[1 * 2 * 12 * 20];
		float *ch_cos = new float[1 * 2 * 12 * 20];
		m_VeUEAry[UserIdx1].imta[RSUIdx].Calculate(0.01f, ch_buffer, ch_sin, ch_cos, H);
		//		for (unsigned char byTempTxAnt = 0; byTempTxAnt != 1; ++ byTempTxAnt)
		//{
		//	for (unsigned char byTempRxAnt = 0; byTempRxAnt !=2; ++ byTempRxAnt)
		//	{
		//    	for (unsigned char byTempPath = 0; byTempPath != 12; ++ byTempPath)
		//    		{
		//       fprintf(fp1, "%f\n", H[byTempTxAnt * 2 * 12 * 2 + byTempRxAnt * 12  * 2 + byTempPath * 2 ]);
		//    fprintf(fp2, "%f\n", H[byTempTxAnt * 2 * 12 * 2 + byTempRxAnt * 12  * 2 + byTempPath * 2 + 1]);
		//   	    }
		//	}
		//}
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
	}
}


void GTAT_Urban::writeVeUELocationUpdateLogInfo(std::ofstream &out) {
	for (int VeUEId = 0; VeUEId < m_Config.VeUENum; VeUEId++) {
		out << "VeUE[ " << left << setw(3) << VeUEId << "]" << endl;
		out << "{" << endl;
		for (const tuple<int, int> &t : m_VeUEAry[VeUEId].m_LocationUpdateLogInfoList)
			out << "    " << "[ RSUId = " << left << setw(2) << get<0>(t) << " , ClusterIdx = " << get<1>(t) << " ]" << endl;
		out << "}" << endl;
	}
}