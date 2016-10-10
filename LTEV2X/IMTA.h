#pragma once
#include<math.h>
#include<string>
#include<map>
#include"fftw3.h"
#include"Config.h"
#include"Enumeration.h"
#include"Global.h"

/*===========================================
*               IMTA�ŵ�ģ��
* ==========================================*/
class cIMTA {
public:
	//·����
	static const unsigned char m_scbySubPathNum = 20;
	static const unsigned char m_scbyMidPathNum = 3;
	//���ϵ������
	static const float m_sacfConstantInHLoS[25];
	static const float m_sacfConstantInHNLoS[25];
	static const float m_sacfConstantUMiLoS[25];
	static const float m_sacfConstantUMiNLoS[25];
	static const float m_sacfConstantUMiO2I[25];
	static const float m_sacfConstantSMaLoS[25];
	static const float m_sacConstantSMaNLoS[25];
	static const float m_sacfConstantUMaLoS[25];
	static const float m_sacfConstantUMaNLoS[25];
	static const float m_sacfConstantRMaLoS[25];
	static const float m_sacfConstantRMaNLoS[25];
	//�Ƕ�ƫ������
	static const float m_sacfAngleOffset[m_scbySubPathNum];
	static const float m_sacfMidPathDelayOffset[m_scbyMidPathNum];
	static const unsigned char m_sacbyMidPathIndex[m_scbySubPathNum];

	//�ŵ������������
	float m_fAntGain;
	float m_fMaxAttenu; // dBm
	unsigned char m_byTxAntNum;
	unsigned char m_byRxAntNum;
	float * m_pfTxSlantAngle; // degree
	float * m_pfRxSlantAngle; // degree
	float * m_pfTxAntSpacing;
	float * m_pfRxAntSpacing;
	float m_fTxAngle;
	float m_fRxAngle;

	//�ŵ��������ó���
	unsigned short m_byPathNum;
	float m_fVelocity;
	float m_fvAngle;
	float m_fC;
	float m_fPathShadowSTD;
	float m_fAoDRatio;
	float m_fAoARatio;
	float m_fXPR;
	float m_fDS;
	float m_fDSRatio;
	float m_fAoD;
	float m_fAoA;
	float m_fK;
	float m_fKDB;
	float m_fD;


	bool m_bLoS;
	bool m_bBuilt;
	bool m_bEnable;
	float m_fPLSF;

	unsigned char m_byPathFirst;
	unsigned char m_byPathSecond;
	//�ŵ�����洢����
	float *m_pfGain;
	float *m_pfSinAoD;
	float *m_pfCosAoD;
	float *m_pfSinAoA;
	float *m_pfCosAoA;
	float *m_pfPhase;

	float m_fGainLoS;
	float m_fSinAoDLoS;
	float m_fSinAoALoS;
	float m_fCosAoALoS;
	float *m_pfPhaseLoS;

	//FFT���ñ���
	unsigned short m_wFFTNum;
	unsigned char m_byFFTOrder;
	float m_fFFTTime;
	unsigned short m_wHNum;
	unsigned short *m_pwFFTIndex;
public:
	cIMTA(void);
	~cIMTA(void);
	bool Build(float t_fFrequency/*Hz*/, sLocation &t_eLocation, sAntenna &t_eAntenna,  float t_fVelocity/*km/h*/, float t_fVAngle/*degree*/);
	bool Enable(bool *t_pbEnable);
	void Calculate( float t_fT/*s*/, float *t_pfTemp, float *t_pfSin, float *t_pfCos, float *t_pfH, float *t_pfHFFT);
	float GetPLSF(void)
	{
		return m_fPLSF;
	}
    float GetAntGain(void)
	{
		return m_fAntGain;
	}
	void Refresh(void);
};