#pragma once
#include<math.h>
#include<string>
#include<map>
#include"fftw3.h"
#include"Config.h"
#include"Enumeration.h"
#include"Global.h"

/*===========================================
*               IMTA信道模型
* ==========================================*/
class IMTA {
public:
	//路径数
	static const int m_scbySubPathNum = 20;
	static const int m_scbyMidPathNum = 3;
	//相关系数矩阵
	static const double m_sacfConstantInHLoS[25];
	static const double m_sacfConstantInHNLoS[25];
	static const double m_sacfConstantUMiLoS[25];
	static const double m_sacfConstantUMiNLoS[25];
	static const double m_sacfConstantUMiO2I[25];
	static const double m_sacfConstantSMaLoS[25];
	static const double m_sacConstantSMaNLoS[25];
	static const double m_sacfConstantUMaLoS[25];
	static const double m_sacfConstantUMaNLoS[25];
	static const double m_sacfConstantRMaLoS[25];
	static const double m_sacfConstantRMaNLoS[25];
	//角度偏移数组
	static const double m_sacfAngleOffset[m_scbySubPathNum];
	static const double m_sacfMidPathDelayOffset[m_scbyMidPathNum];
	static const int m_sacbyMidPathIndex[m_scbySubPathNum];

	//信道所需基本常量
	double m_fAntGain;
	double m_fMaxAttenu; // dBm
	int m_byTxAntNum;
	int m_byRxAntNum;
	double * m_pfTxSlantAngle; // degree
	double * m_pfRxSlantAngle; // degree
	double * m_pfTxAntSpacing;
	double * m_pfRxAntSpacing;
	double m_fTxAngle;
	double m_fRxAngle;

	//信道所需配置常量
	int m_byPathNum;
	double m_fVelocity;
	double m_fvAngle;
	double m_fC;
	double m_fPathShadowSTD;
	double m_fAoDRatio;
	double m_fAoARatio;
	double m_fXPR;
	double m_fDS;
	double m_fDSRatio;
	double m_fAoD;
	double m_fAoA;
	double m_fK;
	double m_fKDB;
	double m_fD;


	bool m_bLoS;
	bool m_bBuilt;
	bool m_bEnable;
	double m_fPLSF;

	int m_byPathFirst;
	int m_byPathSecond;
	//信道所需存储常量
	double *m_pfGain;
	double *m_pfSinAoD;
	double *m_pfCosAoD;
	double *m_pfSinAoA;
	double *m_pfCosAoA;
	double *m_pfPhase;

	double m_fGainLoS;
	double m_fSinAoDLoS;
	double m_fSinAoALoS;
	double m_fCosAoALoS;
	double *m_pfPhaseLoS;

	//FFT所用变量
	int m_wFFTNum;
	int m_byFFTOrder;
	double m_fFFTTime;
	int m_wHNum;
	int *m_pwFFTIndex;
public:
	IMTA(void);
	~IMTA(void);
	bool Build(double* t_Pl, double t_fFrequency/*Hz*/, Location &t_eLocation, Antenna &t_eAntenna,  double t_fVelocity/*km/h*/, double t_fVAngle/*degree*/);
	bool Enable(bool *t_pbEnable);
	void Calculate(double* t_HAfterFFT, double t_fT/*s*/, double *t_pfTemp, double *t_pfSin, double *t_pfCos, double *t_pfH, double *t_pfHFFT);
	double GetPLSF(void)
	{
		return m_fPLSF;
	}
    double GetAntGain(void)
	{
		return m_fAntGain;
	}
	void Refresh(void);
};