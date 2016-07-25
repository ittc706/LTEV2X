#include "Global.h"

using namespace std;


/*===========================================
*            输出日志文件流声明
* ==========================================*/
ofstream g_OutTemp("Output\\Temp.txt");
//DRA单元
ofstream g_OutDRAScheduleInfo("Output\\DRAOutput\\DRAScheduleInfo.txt");
ofstream g_OutClasterPerformInfo("Output\\DRAOutput\\ClasterPerformInfo.txt");
ofstream g_OutEventListInfo("Output\\DRAOutput\\EventListInfo.txt");
ofstream g_OutTTILogInfo("Output\\DRAOutput\\TTILogInfo.txt");
ofstream g_OutEventLogInfo("Output\\DRAOutput\\EventLogInfo.txt");
ofstream g_OutVeUELocationUpdateLogInfo("Output\\DRAOutput\\VeUELocationUpdateLogInfo.txt");
//Traffic单元
ofstream g_OutDelayStatistics("Output\\TrafficOutput\\DelayStatistics.txt");
ofstream g_OutEmergencyPossion("Output\\TrafficOutput\\EmergencyPossion.txt");


/*===========================================
*               全域函数定义
* ==========================================*/
void RandomUniform(float *t_pfArray, unsigned long t_ulNumber, float t_fUpBound, float t_fDownBound, bool t_bFlagZero)
{
	for (unsigned long ulTemp = 0; ulTemp != t_ulNumber; ++ulTemp)
	{
		do
		{
			t_pfArray[ulTemp] = (t_fUpBound - t_fDownBound) * rand() / RAND_MAX + t_fDownBound;
		} while (t_bFlagZero && (t_pfArray[ulTemp] == 0.0f));
	}

	return;
}


void RandomGaussian(float *t_pfArray, unsigned long t_ulNumber, float t_fMean, float t_fStandardDeviation)
{
	unsigned long ulHalfNum = t_ulNumber / 2;
	if (ulHalfNum)
	{
		float *pfTemp1 = new float[ulHalfNum];
		float *pfTemp2 = new float[ulHalfNum];
		RandomUniform(pfTemp1, ulHalfNum, 1.0f, 0.0f, true);
		RandomUniform(pfTemp2, ulHalfNum, c_PI, c_PINeg, false);
		for (unsigned long ulTemp = 0; ulTemp != ulHalfNum; ++ulTemp)
		{
			t_pfArray[ulTemp * 2] = sqrt(log(pfTemp1[ulTemp]) * -2.0f) * cos(pfTemp2[ulTemp]) * t_fStandardDeviation + t_fMean;
			t_pfArray[ulTemp * 2 + 1] = sqrt(log(pfTemp1[ulTemp]) * -2.0f) * sin(pfTemp2[ulTemp]) * t_fStandardDeviation + t_fMean;
		}
		delete[]pfTemp1;
		delete[]pfTemp2;
	}
	if (t_ulNumber % 2)
	{
		float fTemp1;
		float fTemp2;
		RandomUniform(&fTemp1, 1, 1.0f, 0.0f, true);
		RandomUniform(&fTemp2, 1, c_PI, c_PINeg, false);
		t_pfArray[t_ulNumber - 1] = sqrt(log(fTemp1) * -2.0f) * cos(fTemp2) * t_fStandardDeviation + t_fMean;
	}

	return;
}


void SortBubble(float *t_pfArray, unsigned short t_wNumber, bool t_bFlagDirection, bool t_bFlagFabs)
{
	float fTemp;
	bool bFlagDone;
	for (unsigned char i1 = 0; i1 != t_wNumber - 1; ++i1)
	{
		bFlagDone = true;
		for (unsigned char i2 = 0; i2 != t_wNumber - 1 - i1; ++i2)
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

void SelectMax(float *t_pfArray, unsigned char t_byNumber, unsigned char *t_pbyFirst, unsigned char *t_pbySecond)
{
	unsigned char byFisrtIndex;
	unsigned char bySecondIndex;
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
	for (unsigned char byTemp = 2; byTemp != t_byNumber; ++byTemp)
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




vector<int> makeContinuousSequence(int begin, int end) {
	vector<int> res;
	for (int i = begin; i <= end; i++)
		res.push_back(i);
	return res;
}