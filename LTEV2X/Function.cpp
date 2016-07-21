#include "Constant.h"
#include "Definition.h"
//#include "Global.h"
#include <cmath>
#include <cstdlib>
#include "Function.h"


void RandomUniform(float *t_pfArray, unsigned long t_ulNumber, float t_fUpBound, float t_fDownBound, bool t_bFlagZero)
{
	for (unsigned long ulTemp = 0; ulTemp != t_ulNumber; ++ ulTemp)
	{
		do
		{
			t_pfArray[ulTemp] = (t_fUpBound - t_fDownBound) * rand() / RAND_MAX + t_fDownBound;
		}
		while (t_bFlagZero && (t_pfArray[ulTemp] == 0.0f));
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
		for (unsigned long ulTemp = 0; ulTemp != ulHalfNum; ++ ulTemp)
		{
			t_pfArray[ulTemp * 2] = sqrt(log(pfTemp1[ulTemp]) * -2.0f) * cos(pfTemp2[ulTemp]) * t_fStandardDeviation + t_fMean;
			t_pfArray[ulTemp * 2 + 1] = sqrt(log(pfTemp1[ulTemp]) * -2.0f) * sin(pfTemp2[ulTemp]) * t_fStandardDeviation + t_fMean;
		}
		delete []pfTemp1;
		delete []pfTemp2;
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
	for (unsigned char i1 = 0; i1 != t_wNumber - 1; ++ i1)
	{
		bFlagDone = true;
		for (unsigned char i2 = 0; i2 != t_wNumber - 1 - i1; ++ i2)
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

//
//void RandomIndex(unsigned char *t_pdArray, unsigned char t_wNumber)
//{
//	unsigned char wSwap;
//	unsigned char wIndex;
//	for (unsigned char byTemp = 0; byTemp != t_wNumber; ++ byTemp)
//	{
//		t_pdArray[byTemp] = byTemp;
//	}
//	for (unsigned char byTemp = 0; byTemp != t_wNumber; ++ byTemp)
//	{
//		wIndex = rand() % t_wNumber;
//		wSwap = t_pdArray[wIndex];
//		t_pdArray[wIndex] = t_pdArray[byTemp];
//		t_pdArray[byTemp] = wSwap;
//	}
//}
//
//
//void FFTRadix2DIT(float *t_fdFft, unsigned short t_wFFTOrder, unsigned short t_wFFTNumber)
//{
//	unsigned short wTemp1 = 0;
//	unsigned short wTemp2 = 0;
//	float fTempReal;
//	float fTempImag;
//	unsigned short wNumRight = t_wFFTNumber >> 1;
//	unsigned short wTempNum = t_wFFTNumber - 1;
//	unsigned short wButterflyNum;
//	unsigned short wButterflyGap = 1;
//	float fPhase;
//	float fPhaseFactorReal;
//	float fPhaseFactorImag;
//	float fPhaseShiftReal;
//	float fPhaseShiftImag;
//	for (unsigned short i1 = 0; i1 != wTempNum; ++ i1)
//	{
//		if (i1 < wTemp1)
//		{
//			fTempReal = t_fdFft[i1 * 2];
//			fTempImag = t_fdFft[i1 * 2 + 1];
//			t_fdFft[i1 * 2] = t_fdFft[wTemp1 * 2];
//			t_fdFft[i1 * 2 + 1] = t_fdFft[wTemp1 * 2 + 1];
//			t_fdFft[wTemp1 * 2] = fTempReal;
//			t_fdFft[wTemp1 * 2 + 1] = fTempImag;
//		}
//		wTemp2 = wNumRight;
//		while (wTemp2 <= wTemp1)
//		{
//			wTemp1 -= wTemp2;
//			wTemp2 >>= 1;
//		}
//		wTemp1 += wTemp2;
//	}
//	for (unsigned char i1 = 0; i1 != t_wFFTOrder; ++ i1)
//	{
//		wButterflyNum = wButterflyGap;
//		wButterflyGap <<= 1;
//		fPhase = c_PI / wButterflyNum;
//		fPhaseShiftReal = cos(fPhase);
//		fPhaseShiftImag = -sin(fPhase);
//		fPhaseFactorReal = 1.0f;
//		fPhaseFactorImag = 0.0f;
//		for (unsigned short i2 = 0; i2 != wButterflyNum; ++ i2)
//		{
//			for (unsigned short i3 = i2; i3 < t_wFFTNumber; i3 += wButterflyGap)
//			{
//				wTemp1 = i3 + wButterflyNum;
//				fTempReal = t_fdFft[wTemp1 * 2] * fPhaseFactorReal - t_fdFft[wTemp1 * 2 + 1] * fPhaseFactorImag;
//				fTempImag = t_fdFft[wTemp1 * 2] * fPhaseFactorImag + t_fdFft[wTemp1 * 2 + 1] * fPhaseFactorReal;
//				t_fdFft[wTemp1 * 2] = t_fdFft[i3 * 2] - fTempReal;
//				t_fdFft[wTemp1 * 2 + 1] = t_fdFft[i3 * 2 + 1] - fTempImag;
//				t_fdFft[i3 * 2] = t_fdFft[i3 * 2] + fTempReal;
//				t_fdFft[i3 * 2 + 1] = t_fdFft[i3 * 2 + 1] + fTempImag;
//			}
//			fTempReal = fPhaseFactorReal * fPhaseShiftReal - fPhaseFactorImag * fPhaseShiftImag;
//			fTempImag = fPhaseFactorReal * fPhaseShiftImag + fPhaseFactorImag * fPhaseShiftReal;
//			fPhaseFactorReal = fTempReal;
//			fPhaseFactorImag = fTempImag;
//		}
//	}
//
//	return;
//}
//
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
	for (unsigned char byTemp = 2; byTemp != t_byNumber; ++ byTemp)
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

//
//void RandomScatterSector(float *x, float *y, float centerAngle)
//{
//	if (centerAngle > 180.0f)
//	{
//		centerAngle -= 360.0f;
//	}
//	else
//	{
//		if (centerAngle < -180.0f)
//		{
//			centerAngle += 360.0f;
//		}
//	}
//	if (centerAngle >= 0.0f && centerAngle <= 60.0f)
//	{
//		do
//		{
//			RandomUniform(y, 1, c_SqrtThree / 2.0f, tan(-60.0f * c_Degree2PI) * tan((centerAngle - 60.0f) * c_Degree2PI) / (tan(-60.0f * c_Degree2PI) + tan((centerAngle - 60.0f) * c_Degree2PI)), false);
//			if (*y >= 0)
//			{
//				RandomUniform(x, 1, 1.0f - *y / c_SqrtThree, tan((30.0f - centerAngle) * c_Degree2PI) * *y, false);
//			}
//			else
//			{
//				RandomUniform(x, 1, 1.0f + *y / c_SqrtThree, tan((-30.0f - centerAngle) * c_Degree2PI) * *y, false);
//			}
//			*y *= c_cellRadius;
//			*x *= c_cellRadius;
//		}
//		while(*y * *y + *x * *x < c_cellMinum * c_cellMinum);
//		return;
//	}
//	if (centerAngle >= -60.0f && centerAngle <= 0.0f)
//	{
//		do
//		{
//			RandomUniform(y, 1, tan(60.0f * c_Degree2PI) * tan((centerAngle + 60.0f) * c_Degree2PI) / (tan(60.0f * c_Degree2PI) + tan((centerAngle + 60.0f) * c_Degree2PI)), c_SqrtThree / -2.0f, false);
//			if (*y > 0)
//			{
//				RandomUniform(x, 1, 1.0f - *y / c_SqrtThree, tan((30.0f - centerAngle) * c_Degree2PI) * *y, false);
//			}
//			else
//			{
//				RandomUniform(x, 1, 1.0f + *y / c_SqrtThree, tan((-30.0f - centerAngle) * c_Degree2PI) * *y, false);
//			}
//			*y *= c_cellRadius;
//			*x *= c_cellRadius;
//		}while(*y * *y + *x * *x < c_cellMinum * c_cellMinum);
//		return;
//	}
//	if (centerAngle >= 120.0f && centerAngle <= 180.0f)
//	{
//		do
//		{
//			RandomUniform(y, 1, c_SqrtThree / 2.0f, tan(-60.0f * c_Degree2PI) * tan((centerAngle + 60.0f) * c_Degree2PI) / (tan(60.0f * c_Degree2PI) + tan((centerAngle + 60.0f) * c_Degree2PI)), false);
//			if (*y >= 0)
//			{
//				RandomUniform(x, 1, tan((-30.0f - centerAngle) * c_Degree2PI) * *y, *y / c_SqrtThree - 1.0f, false);
//			}
//			else
//			{
//				RandomUniform(x, 1, tan((30.0f - centerAngle) * c_Degree2PI) * *y, -1.0f - *y / c_SqrtThree, false);
//			}
//			*y *= c_cellRadius;
//			*x *= c_cellRadius;
//		}
//		while(*y * *y + *x * *x < c_cellMinum * c_cellMinum);
//		return;
//	}
//	if (centerAngle >= -180.0f && centerAngle <= -120.0f)
//	{
//		do
//		{
//			RandomUniform(y, 1, tan(60.0f * c_Degree2PI) * tan((60.0f - centerAngle) * c_Degree2PI) / (tan(60.0f * c_Degree2PI) + tan((60.0f - centerAngle) * c_Degree2PI)), c_SqrtThree / -2.0f, false);
//			if (*y > 0)
//			{
//				RandomUniform(x, 1, tan((-30.0f - centerAngle) * c_Degree2PI) * *y, *y / c_SqrtThree - 1.0f, false);
//			}
//			else
//			{
//				RandomUniform(x, 1, tan((30.0f - centerAngle) * c_Degree2PI) * *y, -1.0f - *y / c_SqrtThree, false);
//			}
//			*y *= c_cellRadius;
//			*x *= c_cellRadius;
//		}
//		while(*y * *y + *x * *x < c_cellMinum * c_cellMinum);
//		return;
//	}
//	if (centerAngle > 60.0f && centerAngle < 120.0f)
//	{
//		do
//		{
//			RandomUniform(y, 1, c_SqrtThree / 2.0f, 0.0f, false);
//			if (*y < tan(60.0f * c_Degree2PI) * tan((centerAngle - 60.0f) * c_Degree2PI) / (tan(60.0f * c_Degree2PI) + tan((centerAngle - 60.0f) * c_Degree2PI)))
//			{
//				RandomUniform(x, 1, tan((-30.0f - centerAngle) * c_Degree2PI) * *y, tan((30.0f - centerAngle) * c_Degree2PI) * *y, false);
//			}
//			else
//			{
//				if (*y > tan(60.0f * c_Degree2PI) * tan((-60.0f - centerAngle) * c_Degree2PI) / (tan(60.0f * c_Degree2PI) + tan((-60.0f - centerAngle) * c_Degree2PI)))
//				{
//					RandomUniform(x, 1, 1.0f - *y / c_SqrtThree, -1.0f + *y / c_SqrtThree, false);
//				}
//				else
//				{
//					RandomUniform(x, 1, 1.0f - *y / c_SqrtThree, tan((30.0f - centerAngle) * c_Degree2PI) * *y, false);
//				}
//			}
//			*y *= c_cellRadius;
//			*x *= c_cellRadius;
//		}
//		while(*y * *y + *x * *x < c_cellMinum * c_cellMinum);
//		return;
//	}
//	if (centerAngle > -120.0f && centerAngle < -60.0f)
//	{
//		do
//		{
//			RandomUniform(y, 1, 0.0f, c_SqrtThree / -2.0f, false);
//			if (*y < tan(-60.0f * c_Degree2PI) * tan((60.0f - centerAngle) * c_Degree2PI) / (tan(-60.0f * c_Degree2PI) + tan((60.0f - centerAngle) * c_Degree2PI)))
//			{
//				RandomUniform(x, 1, 1.0f + *y / c_SqrtThree, -1.0f - *y / c_SqrtThree, false);
//			}
//			else
//			{
//				if (*y > tan(-60.0f * c_Degree2PI) * tan((60.0f + centerAngle) * c_Degree2PI) / (tan(-60.0f * c_Degree2PI) + tan((60.0f + centerAngle) * c_Degree2PI)))
//				{
//					RandomUniform(x, 1, tan((30.0f - centerAngle) * c_Degree2PI) * *y, tan((-30.0f - centerAngle) * c_Degree2PI) * *y, false);
//				}
//				else
//				{
//					RandomUniform(x, 1, 1.0f + *y / c_SqrtThree, tan((-30.0f - centerAngle) * c_Degree2PI) * *y, false);
//				}
//			}
//			*y *= c_cellRadius;
//			*x *= c_cellRadius;
//		}
//		while(*y * *y + *x * *x < c_cellMinum * c_cellMinum);
//		return;
//	}
//}
//
//
//void RandomScatter(float *x, float *y, float radius)
//{
//	do
//	{
//		RandomUniform(x, 1, -1.0f, 1.0f, false);
//		RandomUniform(y, 1, -0.5f * c_SqrtThree, 0.5f * c_SqrtThree, false);
//	}
//	while (*x * c_SqrtThree + *y - c_SqrtThree > 0 || *x * c_SqrtThree - *y + c_SqrtThree < 0 || *x *c_SqrtThree + *y + c_SqrtThree < 0 || *x * c_SqrtThree - *y - c_SqrtThree > 0);
//	
//
//		*x *= radius;
//		*y *= radius;
//	
//}
//
//void RandomScatter(float *x, float *y, float radius,float mim)
//{
//	do
//	{
//		RandomUniform(y, 1, c_SqrtThree / 2.0f, c_SqrtThree / -2.0f, false);
//		if (*y >= 0)
//		{
//			RandomUniform(x, 1, 1.0f - *y / c_SqrtThree, -1.0f + *y / c_SqrtThree, false);
//		}
//		else
//		{
//			RandomUniform(x, 1, 1.0f + *y / c_SqrtThree, -1.0f - *y / c_SqrtThree, false);
//		}
//		*x *= radius;
//		*y *= radius;
//	}
//	while(*y * *y + *x * *x < mim * mim);
//}
//
//void RandomScatterInCircle(float *x, float *y, float radius,float mim)
//{
//	do 
//	{
//		RandomUniform(x, 1, radius, -1.0f * radius, false);
//		RandomUniform(y, 1, radius, -1.0f * radius, false);
//	}
//	while ((*x * *x + *y * *y) > (radius * radius) || (*x * *x + *y * *y) < (mim * mim));
//
//}
//
//map<string, eLinkType> eLinkTypeMap = eLinkTypeMapInitial();
//map<string, eLinkType> eLinkTypeMapInitial(void)
//{
//	map<string, eLinkType> ret;
//	ret["DOWNLINK"] = DOWNLINK;
//	ret["UPLINK"] = UPLINK;
//	return ret;
//}
//map<string, eScheduleType> eScheduleTypeMap = eScheduleTypeMapInitial();
//map<string, eScheduleType> eScheduleTypeMapInitial(void)
//{
//	map<string, eScheduleType> ret;
//	ret["ROUNDROBIN"] = ROUNDROBIN;
//	ret["PF"] = PF;
//	return ret;
//}
//map<string, eNet> eNetMap = eNetMapInitial();
//map<string, eNet> eNetMapInitial(void)
//{
//	map<string, eNet> ret;
//	ret["HONET"] = HONET;
//	ret["HENET"] = HENET;
//	return ret;
//}
//map<string, eTransMode> eTransModeMap = eTransModeMapInitial();
//map<string, eTransMode> eTransModeMapInitial(void)
//{
//	map<string, eTransMode> ret;
//	ret["INVALID_TM"] = INVALID_TM;
//	ret["TM_2"] = TM_2;
//	ret["TM_1"] = TM_1;
//	ret["CODEBOOK"] = CODEBOOK;
//	ret["BEAMFORMING"] = BEAMFORMING;		
//	return ret;
//}
//map<string, eFreqReuse> eFreqReuseMap = eFreqReuseMapInitial();
//map<string, eFreqReuse> eFreqReuseMapInitial(void)
//{
//	map<string, eFreqReuse> ret;
//	ret["STATIC"] = STATIC;
//	ret["SOFT"] = SOFT;
//	ret["NONE"] = NONE;	
//	return ret;
//}
//map<string, eCoordinateMode> eCoordinateModeMap = eCoordinateModeMapInitial();
//map<string, eCoordinateMode> eCoordinateModeMapInitial(void)
//{
//	map<string, eCoordinateMode> ret;
//	ret["SCSU"] = SCSU;
//	ret["SCMU"] = SCMU;
//	ret["JT_MCSU"] = JT_MCSU;
//	ret["JT_MCMU"] = JT_MCMU;
//	ret["CS_MCSU"] = CS_MCSU;
//	ret["CS_MCMU"] = CS_MCMU;
//	return ret;
//}
//map<string, eDuplexing> eDuplexingMap = eDuplexingMapInitial();
//map<string, eDuplexing> eDuplexingMapInitial(void)
//{
//	map<string, eDuplexing> ret;
//	ret["TDD"] = TDD;
//	ret["FDD"] = FDD;
//	return ret;
//}
//map<string, eRxType> eRxTypeMap = eRxTypeMapInitial();
//map<string, eRxType> eRxTypeMapInitial(void)
//{
//	map<string, eRxType> ret;
//	ret["MMSE_IRC"] = MMSE_IRC;
//	ret["MMSE_OPT1"] = MMSE_OPT1;
//	ret["MRC"] = MRC;
//	ret["PRE"] = PRE;
//	return ret;
//}
//map<string, eChannelType> eChannelTypeMap = eChannelTypeMapInitial();
//map<string, eChannelType> eChannelTypeMapInitial(void)
//{
//	map<string, eChannelType> ret;
//	ret["IMT"] = IMT;
//	ret["SCME_MACRO"] = SCME_MACRO;
//	ret["SCME_MICRO"] = SCME_MICRO;
//	return ret;
//}
//map<string, eChannelEstimateType> eChannelEstimateTypeMap = eChannelEstimateTypeMapInitial();
//map<string, eChannelEstimateType> eChannelEstimateTypeMapInitial(void)
//{
//	map<string, eChannelEstimateType> ret;
//	ret["IDEAL_ESTIMATE"] = IDEAL_ESTIMATE;
//	ret["NONIDEAL_ESTIMATE"] = NONIDEAL_ESTIMATE;
//	return ret;
//}
//
//
//
//map<string, eScenario> eScenarioMap = eScenarioMapInitial();
//map<string, eScenario> eScenarioMapInitial(void)
//{
//	map<string, eScenario> ret;
//	ret["IMTA_INDOOR_HOTSPOT"] = IMTA_INDOOR_HOTSPOT;
//	ret["IMTA_URBAN_MICRO"] = IMTA_URBAN_MICRO;
//	ret["IMTA_SUBURBAN_MACRO"] = IMTA_SUBURBAN_MACRO;
//	ret["IMTA_URBAN_MACRO"] = IMTA_URBAN_MACRO;
//	ret["IMTA_RURAL_MACRO"] = IMTA_RURAL_MACRO;
//	ret["SCME_SUBURBAN_MACRO"] = SCME_SUBURBAN_MACRO;
//	ret["SCME_URBAN_MACRO_LOW"] = SCME_URBAN_MACRO_LOW;
//	ret["SCME_URBAN_MACRO_HIGH"] = SCME_URBAN_MACRO_HIGH;
//	ret["SCME_URBAN_MICRO"] = SCME_URBAN_MICRO;
//	return ret;
//}
//// map<string, eIMT> eIMTMap = eIMTMapInitial();
//// map<string, eIMT> eIMTMapInitial(void)
//// {
//// 	map<string, eIMT> ret;
//// 	ret["URBAN_MICRO"] = URBAN_MICRO;
//// 	ret["URBAN_MACRO"] = URBAN_MACRO;
//// 	ret["SUBURBAN_MACRO"] = SUBURBAN_MACRO;
//// 	ret["RURAL_MACRO"] = RURAL_MACRO;
//// 	return ret;
//// }
//// map<string, eSCMEMacro> eSCMEMacroMap = eSCMEMacroMapInitial();
//// map<string, eSCMEMacro> eSCMEMacroMapInitial(void)
//// {
//// 	map<string, eSCMEMacro> ret;
//// 	ret["SUBURBAN"] = SUBURBAN;
//// 	ret["URBAN_HIGH"] = URBAN_HIGH;
//// 	ret["URBAN_LOW"] = URBAN_LOW;		
//// 	return ret;
//// }
//// map<string, eSCMEMicro> eSCMEMicroMap = eSCMEMicroMapInitial();
//// map<string, eSCMEMicro> eSCMEMicroMapInitial(void)
//// {
//// 	map<string, eSCMEMicro> ret;
//// 	ret["URBAN_LOS"] = URBAN_LOS;
//// 	ret["URBAN_NLOS"] = URBAN_NLOS;
//// 	return ret;
//// }
//double Quantization(const double &t_dValue, const double &t_dMax, const double &t_dMin, const unsigned short &t_wLevel)
//{
//        double dMiddle = (t_dMax - t_dMin) / (t_wLevel - 1);
//        double dLevel = (t_dValue - t_dMin) / dMiddle;
//        unsigned short wLevel = (unsigned short)(dLevel);
//        if ((dLevel - wLevel) > 0.5)
//        {
//                return (wLevel + 1) * dMiddle + t_dMin;
//        }
//        else
//        {
//                return wLevel * dMiddle + t_dMin;
//        }
//   
//}
//double QuantizationCQI(double sinr)
//{
//		double temp=QCQIList[0];
//        for(int i=0;i!=15;i++)
//        {
//                if(QCQIList[i]<=sinr)
//                        temp=QCQIList[i];
//                else
//                        break;
//        }
//        return temp;
//}