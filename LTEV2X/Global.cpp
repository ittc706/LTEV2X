#include<iomanip>
#include"Global.h"

using namespace std;


/*===========================================
*            输出日志文件流声明
* ==========================================*/
ofstream g_FileTemp("Log\\Temp.txt");
//GTAT_Urban模块
ofstream g_FileVeUELocationUpdateLogInfo("Log\\RRMLog\\VeUELocationUpdateLogInfo.txt");
ofstream g_FileVeUENumPerRSULogInfo("Log\\TMACLog\\VeUENumPerRSULogInfo.txt");
//RRM_RR模块
ofstream g_FileRRScheduleInfo("Log\\RRMLog\\RRScheduleInfo.txt");
//RRM_TDM_DRA模块
ofstream g_FileDRAScheduleInfo("Log\\RRMLog\\DRAScheduleInfo.txt");
ofstream g_FileClasterPerformInfo("Log\\RRMLog\\ClasterPerformInfo.txt");
ofstream g_FileEventListInfo("Log\\RRMLog\\EventListInfo.txt");
ofstream g_FileTTILogInfo("Log\\RRMLog\\TTILogInfo.txt");
ofstream g_FileEventLogInfo("Log\\RRMLog\\EventLogInfo.txt");
//TMAC_B模块
ofstream g_FileDelayStatistics("Log\\TMACLog\\DelayStatistics.txt");
ofstream g_FileEmergencyPossion("Log\\TMACLog\\EmergencyPossion.txt");
ofstream g_FileDataPossion("Log\\TMACLog\\DataPossion.txt");
ofstream g_FileConflictNum("Log\\TMACLog\\ConflictNum.txt");
ofstream g_FileTTIThroughput("Log\\TMACLog\\TTIThroughput.txt");
ofstream g_FileRSUThroughput("Log\\TMACLog\\RSUThroughput.txt");


/*===========================================
*               全域函数定义
* ==========================================*/
void randomUniform(double *t_pfArray, long t_ulNumber, double t_fUpBound, double t_fDownBound, bool t_bFlagZero)
{
	for (long ulTemp = 0; ulTemp != t_ulNumber; ++ulTemp)
	{
		do
		{
			t_pfArray[ulTemp] = (t_fUpBound - t_fDownBound) * rand() / RAND_MAX + t_fDownBound;
		} while (t_bFlagZero && (t_pfArray[ulTemp] == 0.0f));
	}

	return;
}


void randomGaussian(double *t_pfArray, long t_ulNumber, double t_fMean, double t_fStandardDeviation)
{
	long ulHalfNum = t_ulNumber / 2;
	if (ulHalfNum)
	{
		double *pfTemp1 = new double[ulHalfNum];
		double *pfTemp2 = new double[ulHalfNum];
		randomUniform(pfTemp1, ulHalfNum, 1.0f, 0.0f, true);
		randomUniform(pfTemp2, ulHalfNum, gc_PI, gc_PINeg, false);
		for (long ulTemp = 0; ulTemp != ulHalfNum; ++ulTemp)
		{
			t_pfArray[ulTemp * 2] = sqrt(log(pfTemp1[ulTemp]) * -2.0f) * cos(pfTemp2[ulTemp]) * t_fStandardDeviation + t_fMean;
			t_pfArray[ulTemp * 2 + 1] = sqrt(log(pfTemp1[ulTemp]) * -2.0f) * sin(pfTemp2[ulTemp]) * t_fStandardDeviation + t_fMean;
		}
		delete[] pfTemp1;
		delete[] pfTemp2;
	}
	if (t_ulNumber % 2)
	{
		double fTemp1;
		double fTemp2;
		randomUniform(&fTemp1, 1, 1.0f, 0.0f, true);
		randomUniform(&fTemp2, 1, gc_PI, gc_PINeg, false);
		t_pfArray[t_ulNumber - 1] = sqrt(log(fTemp1) * -2.0f) * cos(fTemp2) * t_fStandardDeviation + t_fMean;
	}

	return;
}


void sortBubble(double *t_pfArray, int t_wNumber, bool t_bFlagDirection, bool t_bFlagFabs)
{
	double fTemp;
	bool bFlagDone;
	for (int i1 = 0; i1 != t_wNumber - 1; ++i1)
	{
		bFlagDone = true;
		for (int i2 = 0; i2 != t_wNumber - 1 - i1; ++i2)
		{
			if (t_bFlagDirection)
			{
				if (t_bFlagFabs)
				{
					if (fabs(t_pfArray[i2]) < fabs(t_pfArray[i2 + 1]))
					{
						fTemp = t_pfArray[i2];
						t_pfArray[i2] = t_pfArray[i2 + 1];
						t_pfArray[i2 + 1] = fTemp;
						bFlagDone = false;
					}
				}
				else
				{
					if (t_pfArray[i2] < t_pfArray[i2 + 1])
					{
						fTemp = t_pfArray[i2];
						t_pfArray[i2] = t_pfArray[i2 + 1];
						t_pfArray[i2 + 1] = fTemp;
						bFlagDone = false;
					}
				}
			}
			else
			{
				if (t_bFlagFabs)
				{
					if (fabs(t_pfArray[i2]) > fabs(t_pfArray[i2 + 1]))
					{
						fTemp = t_pfArray[i2];
						t_pfArray[i2] = t_pfArray[i2 + 1];
						t_pfArray[i2 + 1] = fTemp;
						bFlagDone = false;
					}
				}
				else
				{
					if (t_pfArray[i2] > t_pfArray[i2 + 1])
					{
						fTemp = t_pfArray[i2];
						t_pfArray[i2] = t_pfArray[i2 + 1];
						t_pfArray[i2 + 1] = fTemp;
						bFlagDone = false;
					}
				}
			}
		}
		if (bFlagDone)
		{
			break;
		}
	}

	return;
}

void selectMax(double *t_pfArray, int t_byNumber, int *t_pbyFirst, int *t_pbySecond)
{
	int byFisrtIndex;
	int bySecondIndex;
	if (t_pfArray[0] < t_pfArray[1])
	{
		byFisrtIndex = 1;
		bySecondIndex = 0;
	}
	else
	{
		byFisrtIndex = 0;
		bySecondIndex = 1;
	}
	for (int byTemp = 2; byTemp != t_byNumber; ++byTemp)
	{
		if (t_pfArray[byFisrtIndex] < t_pfArray[byTemp])
		{
			bySecondIndex = byFisrtIndex;
			byFisrtIndex = byTemp;
		}
		else
		{
			if (t_pfArray[bySecondIndex] < t_pfArray[byTemp])
			{
				bySecondIndex = byTemp;
			}
		}
	}
	*t_pbyFirst = byFisrtIndex;
	*t_pbySecond = bySecondIndex;

	return;
}

