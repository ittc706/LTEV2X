#pragma once
#include "Constant.h"
#include "Config.h"

class cChannelModel
{
public:
	cChannelModel(void);
	virtual ~cChannelModel(void);
	virtual bool Build(float t_fFrequency/*Hz*/, unsigned short t_wHNum, float t_fHBandWidth, sLocation &t_eLocation, sAntenna &t_eAntenna, float t_fVelocity/*km/h*/, float t_fVAngle/*degree*/) = 0;
	virtual bool Enable(bool *t_pbEnable) = 0;
	virtual void Calculate(float **t_ppfStore, float t_fT/*s*/, float *t_pfTemp, float *t_pfSin, float *t_pfCos, float *t_pfH, float *t_pfHFFT) = 0;
	virtual float GetPLSF(void) = 0;
	virtual float GetAntGain(void) = 0;
	virtual void Refresh(void) = 0;
};
