#include "System.h"
#include <iostream>
#include "Function.h"

using namespace std;

void cSystem::FreshLoc(void)
{
	ofstream fp("H.txt", ofstream::app);
	//FILE *fp;//建立一个文件操作指针
    //fp=fopen("H.txt","w+");//以追加的方式建立或打开1.txt，默认位置在你程序的目录下面
	ofstream fp1("H1.txt", ofstream::app);//建立一个文件操作指针
	//FILE *fp2;//建立一个文件操作指针
    //fp1=fopen("H1.txt","w+");//以追加的方式建立或打开1.txt，默认位置在你程序的目录下面
	ofstream fp2("H2.txt", ofstream::app);//以追加的方式建立或打开1.txt，默认位置在你程序的目录下面
	for(int UserIdx=0;UserIdx!=conf.VeUENum;UserIdx++)
	{
		bool RoadChangeFlag=false;
		int temp;
		switch (veUE[UserIdx].m_locationID)
		{
		case 191:
			veUE[UserIdx].m_locationID=0;
			break;
		case 0:
			temp=rand()%4;
			if(temp==0)//turn left
			{
				RoadChangeFlag=true;
				veUE[UserIdx].m_wRoadID=c_WrapAroundRoad[veUE[UserIdx].m_wRoadID][4];
				veUE[UserIdx].m_locationID=96;
				veUE[UserIdx].m_fvAngle=-90;
			}
			else 
				if(temp==2)//turn right
				{
				veUE[UserIdx].m_locationID=veUE[UserIdx].m_locationID+1;
				veUE[UserIdx].m_fvAngle=90;
				}
			    else//go straight，不改变方向
				{
				RoadChangeFlag=true;
				veUE[UserIdx].m_wRoadID=c_WrapAroundRoad[veUE[UserIdx].m_wRoadID][5];
				veUE[UserIdx].m_locationID=157;
				}
		    break;
		case 61:
			temp=rand()%4;
			if(temp==0)//turn left
			{
				RoadChangeFlag=true;
				veUE[UserIdx].m_wRoadID=c_WrapAroundRoad[veUE[UserIdx].m_wRoadID][6];
				veUE[UserIdx].m_locationID=157;
				veUE[UserIdx].m_fvAngle=-180;
			}
			else 
				if(temp==2)
				{
				veUE[UserIdx].m_locationID=veUE[UserIdx].m_locationID+1;
				veUE[UserIdx].m_fvAngle=0;
				}
				else
				{
				RoadChangeFlag=true;
				veUE[UserIdx].m_wRoadID=c_WrapAroundRoad[veUE[UserIdx].m_wRoadID][7];
				veUE[UserIdx].m_locationID=0;
				veUE[UserIdx].m_fvAngle=270;
				}
			break;
		case 96:
			temp=rand()%4;
			if(temp==0)//turn left
			{
				RoadChangeFlag=true;
				veUE[UserIdx].m_wRoadID=c_WrapAroundRoad[veUE[UserIdx].m_wRoadID][8];
				veUE[UserIdx].m_locationID=0;
				veUE[UserIdx].m_fvAngle=90;
			}
			else 
				if(temp==2)
				{
				veUE[UserIdx].m_locationID=veUE[UserIdx].m_locationID+1;
				veUE[UserIdx].m_fvAngle=-90;
				}
			    else
				{
				RoadChangeFlag=true;
				veUE[UserIdx].m_wRoadID=c_WrapAroundRoad[veUE[UserIdx].m_wRoadID][1];
				veUE[UserIdx].m_locationID=61;
				}
				break;
		case 157:
		    temp=rand()%4;
			if(temp==0)//turn left
			{
				RoadChangeFlag=true;
				veUE[UserIdx].m_wRoadID=c_WrapAroundRoad[veUE[UserIdx].m_wRoadID][2];
				veUE[UserIdx].m_locationID=61;
				veUE[UserIdx].m_fvAngle=0;
			}
			else 
				if(temp==2)
				{
				veUE[UserIdx].m_locationID=veUE[UserIdx].m_locationID+1;
			    veUE[UserIdx].m_fvAngle=-180;
				}
				else
				{
				RoadChangeFlag=true;
				veUE[UserIdx].m_wRoadID=c_WrapAroundRoad[veUE[UserIdx].m_wRoadID][3];
				veUE[UserIdx].m_locationID=96;
				}
				break;
		default:
			 veUE[UserIdx].m_locationID=veUE[UserIdx].m_locationID+1;
				break;
		}
					veUE[UserIdx].m_fX=conf.pueTopo[veUE[UserIdx].m_locationID*2+0];
					veUE[UserIdx].m_fY=conf.pueTopo[veUE[UserIdx].m_locationID*2+1];
					veUE[UserIdx].m_fAbsX=Road[veUE[UserIdx].m_wRoadID].m_fAbsX+veUE[UserIdx].m_fX;
					veUE[UserIdx].m_fAbsY=Road[veUE[UserIdx].m_wRoadID].m_fAbsY+veUE[UserIdx].m_fY;
					
	}


	sLocation location;
	sAntenna antenna;
	location.bManhattan = true;
	
	unsigned short RSUIdx=0;
	unsigned short ClusterID=0;
    for(int UserIdx1=0;UserIdx1!=conf.VeUENum;UserIdx1++)
	{ 
		veUE[UserIdx1].imta=new cIMTA[conf.RSUNum];
		switch (veUE[UserIdx1].m_locationID/10)
		{
		case 1:
			RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][8];
		    ClusterID = 2;
			break;
		case 2:
			RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][8];
			ClusterID = 1;
			break;
		case 3:
		    RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][9];
			ClusterID = 2;
		    break;
		case 4:
		    RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][9];
			ClusterID = 1;
		    break;
		case 5:
			RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][0];
			ClusterID = 4;
			break;
		case 6:
			RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][0];
			ClusterID = 3;
			break;
		case 7:
		case 8:
		case 9:
			if(veUE[UserIdx1].m_locationID<87)
			{
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][1];
				if((veUE[UserIdx1].m_locationID<78))  ClusterID = 3;
				else ClusterID = 2;
			}   
			else
			{
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][2];
		     	if((veUE[UserIdx1].m_locationID<96))  ClusterID = 6;
				else ClusterID = 5;
			}
			break;
		case 10:
		case 11:
		case 12:
			 if(veUE[UserIdx1].m_locationID<106)
			 {
			    RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][2];
			    ClusterID = 5;
		     }
			 else
			 {
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][3];
		     	if((veUE[UserIdx1].m_locationID<116))  ClusterID = 0;
				else ClusterID = 3;
			 }
			break;
		case 13:
			 if(veUE[UserIdx1].m_locationID<126)
			 {
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][3];
			     ClusterID = 3;
			 }
			 else
			 {
				 RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][4];
			     if((veUE[UserIdx1].m_locationID<136))  ClusterID = 0;
				 else ClusterID = 3;
			 }
			 break;
		case 14:
			 if(veUE[UserIdx1].m_locationID<146)
			 {
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][4];
				ClusterID = 3;
			 }
			 else
			 {
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][5];
				ClusterID = 0;
			 }
			 break;
		case 15:
		case 16:
		case 17:
			 if(veUE[UserIdx1].m_locationID<165)
			 {
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][5];
			     if((veUE[UserIdx1].m_locationID<157))  ClusterID = 0;
				 else ClusterID = 7;
			 }
			 else
			 {
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][6];
			     if((veUE[UserIdx1].m_locationID<174))  ClusterID = 1;
				 else ClusterID = 0;
			 }
				break;
		case 18:
		case 19:
			 if(veUE[UserIdx1].m_locationID<183)
			 {
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][6];
			    ClusterID = 0;
			 }
			 else
			 {
				 RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][7];
				 ClusterID  = 2;
			 }
			 break;
	    case 0:
				RSUIdx = c_RSUInRoad[veUE[UserIdx1].m_wRoadID][7];
				ClusterID  = 1;
			 break;
		default:
			break;
		}
			veUE[UserIdx1].m_RSUId = RSUIdx;
			veUE[UserIdx1].m_ClusterIdx = ClusterID;
			RSU[RSUIdx].m_VeUEIdList.push_back(UserIdx1);
			location.eType = None;
			location.fDistance = 0;
			location.fDistance1 = 0;
			location.fDistance2 = 0;
			
			float angle=0;
			if (location.bManhattan == true)  //计算location distance
			{
				//if (abs(veUE[UserIdx1].m_fAbsX - RSU[RSUIdx].m_fAbsX) <= 10.5 || abs(veUE[UserIdx1].m_fAbsY - RSU[RSUIdx].m_fAbsY) <= 10.5)
				//{
					location.eType = Los;
					location.fDistance = sqrt(pow((veUE[UserIdx1].m_fAbsX - RSU[RSUIdx].m_fAbsX), 2.0f) + pow((veUE[UserIdx1].m_fAbsY - RSU[RSUIdx].m_fAbsY), 2.0f));
					angle = atan2(veUE[UserIdx1].m_fAbsY - RSU[RSUIdx].m_fAbsY, veUE[UserIdx1].m_fAbsX - RSU[RSUIdx].m_fAbsX) / c_Degree2PI;
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

	 antenna.fTxAngle=angle - veUE[UserIdx1].m_fantennaAngle;
	 antenna.fRxAngle=angle - RSU[RSUIdx].m_fantennaAngle;
	 antenna.fAntGain = 3;
	 antenna.fMaxAttenu = 23;
	 antenna.byTxAntNum = 1;
	 antenna.byRxAntNum = 2;
	 antenna.pfTxSlantAngle=new float[antenna.byTxAntNum];
	 antenna.pfTxAntSpacing=new float[antenna.byTxAntNum];
	 antenna.pfRxSlantAngle=new float[antenna.byRxAntNum];
	 antenna.pfRxAntSpacing=new float[antenna.byRxAntNum];
	 antenna.pfTxSlantAngle[0] = 90.0f;
	 antenna.pfTxAntSpacing[0] = 0.0f;
	 antenna.pfRxSlantAngle[0] = 90.0f;
	 antenna.pfRxSlantAngle[1] = 90.0f;
	 antenna.pfRxAntSpacing[0] = 0.0f;
	 antenna.pfRxAntSpacing[1] = 0.5f;
	 
	 
	 veUE[UserIdx1].imta[RSUIdx].Build(c_FC,location,antenna,veUE[UserIdx1].m_fv,veUE[UserIdx1].m_fvAngle,fp);//计算了结果代入信道模型计算UE之间信道系数
	 bool *flag=new bool();
	 *flag=true;
	 veUE[UserIdx1].imta[RSUIdx].Enable(flag,fp);
     float *H = new float[1 * 2 * 12 * 20 * 2];
	 float *ch_buffer = new float[1 * 2 * 12 * 20];
	 float *ch_sin = new float[1 * 2 * 12 * 20];
	 float *ch_cos = new float[1 * 2 * 12 * 20];
	 veUE[UserIdx1].imta[RSUIdx].Calculate(0.01,ch_buffer,ch_sin,ch_cos,H);
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
	
	delete [] H;
	delete []ch_buffer;
	delete []ch_sin;
	delete []ch_cos;
   }
}
