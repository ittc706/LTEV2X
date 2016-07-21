#pragma once
#include "Constant.h"
#include "Definition.h"
#include <time.h>
#include <math.h>
//#include "Numerical.h"

//产生特定分布随机数
void RandomUniform(float *t_pfArray, unsigned long t_ulNumber, float t_fUpBound, float t_fDownBound, bool t_bFlagZero);
void RandomGaussian(float *t_pfArray, unsigned long t_ulNumber, float t_fMean, float t_fStandardDeviation);
void SortBubble(float *t_pfArray, unsigned short t_wNumber, bool t_bFlagDirection, bool t_bFlagFabs);
void RandomIndex(unsigned char *t_byArray, unsigned char t_wNumber);
void FFTRadix2DIT(float *t_pfFft, unsigned short t_wFFTOrder, unsigned short t_wFFTNumber);
void SelectMax(float *t_pfArray, unsigned char t_byNumber, unsigned char *t_pbyFirst, unsigned char *t_pbySecond);
void RandomScatterSector(float *x, float *y, float centerAngle);
void RandomScatter(float *x, float *y, float radius);
void RandomScatter(float *x, float *y, float radius,float mim);
void RandomScatterInCircle(float *x, float *y, float radius,float mim);
double Quantization(const double &t_dValue, const double &t_dMax, const double &t_dMin, const unsigned short &t_wLevel);
double QuantizationCQI(double sinr);
