#include<vector>
#include"Global.h"
#include"Exception.h"
#include"Definition.h"

using namespace std;

Codebook codebook;
SubbandInfo subbandInfo[10000];
int _H_2_SubbandTable[10000];

double g_MCS_SNR_BLER_UPLINK_B[29] = { 0.162398,0.275259,0.331273,0.387216,0.500396,0.632055,0.757767,0.976645,1.181258,1.450097,1.891839,2.021082,2.310004,2.959804,3.490522,4.358722,5.116073,6.160274,6.791948,8.179674,9.797276,12.584850,14.960581,17.775798,22.079626,26.184039,30.701693,35.817768,66.378959 };
double g_MCS_SNR_BLER_UPLINK_C[29] = { 0.054680,0.057094,0.070231,0.088679,0.080189,0.090378,0.097123,0.112113,0.130679,0.148960,0.207821,0.208571,0.244529,0.283471,0.329564,0.370230,0.471146,0.537504,0.609931,0.696236,0.804291,1.015040,1.199394,1.402330,1.786923,2.067856,2.337520,2.848863,4.948657 };
double g_MCS_SNR_BLER_DOWNLINK_B[28] = { 0.201611,0.330616,0.404240,0.485208,0.597435,0.767101,0.977429,1.263688,1.584667,2.046083,2.177324,2.530804,3.157812,4.040255,4.862093,6.475634,7.845347,8.988165,10.016494,12.164375,15.101923,20.634566,25.288302,30.601480,39.150185,47.717250,59.473203,78.885856 };
double g_MCS_SNR_BLER_DOWNLINK_C[28] = { 0.055577,0.068676,0.086868,0.091287,0.107688,0.110339,0.126987,0.147531,0.184659,0.237735,0.243952,0.277518,0.319784,0.404033,0.444956,0.594147,0.756443,0.842522,0.917497,1.086768,1.312919,1.773723,2.141489,2.722980,3.392748,3.939159,4.876424,7.307271 };

//上行mcs 等级
sMCS _MCS_Level_UP[UPLINK_MCS_LEVELS] = {
	{ QPSK,	0.079f },
	{ QPSK,	0.129f },
	{ QPSK,	0.157f },
	{ QPSK,	0.186f },
	{ QPSK,	0.229f },
	{ QPSK,	0.293f },
	{ QPSK,	0.350f },
	{ QPSK,	0.421f },
	{ QPSK,	0.479f },
	{ QPSK,	0.550f },
	{ _16QAM,	0.275f },
	{ _16QAM,	0.304f },
	{ _16QAM,	0.346f },
	{ _16QAM,	0.404f },
	{ _16QAM,	0.446f },
	{ _16QAM,	0.504f },
	{ _16QAM,	0.546f },
	{ _64QAM,	0.364f },
	{ _64QAM,	0.383f },
	{ _64QAM,	0.421f },
	{ _64QAM,	0.460f },
	{ _64QAM,	0.517f },
	{ _64QAM,	0.555f },
	{ _64QAM,	0.593f },
	{ _64QAM,	0.640f },
	{ _64QAM,	0.679f },
	{ _64QAM,	0.717f },
	{ _64QAM,	0.755f },
	{ _64QAM,	0.888f },
};
//下行mcs等级
sMCS _MCS_Level_DL[DOWNLINK_MCS_LEVELS] = {
	{ QPSK,0.095f },
	{ QPSK,0.155f },
	{ QPSK,0.190f },
	{ QPSK,0.224f },
	{ QPSK,0.276f },
	{ QPSK,0.353f },
	{ QPSK,0.422f },
	{ QPSK,0.509f },
	{ QPSK,0.578f },
	{ QPSK,0.664f },
	{ _16QAM,0.332f },
	{ _16QAM,0.366f },
	{ _16QAM,0.418f },
	{ _16QAM,0.487f },
	{ _16QAM,0.539f },
	{ _16QAM,0.608f },
	{ _16QAM,0.659f },
	{ _64QAM,0.440f },
	{ _64QAM,0.463f },
	{ _64QAM,0.509f },
	{ _64QAM,0.555f },
	{ _64QAM,0.624f },
	{ _64QAM,0.670f },
	{ _64QAM,0.716f },
	{ _64QAM,0.773f },
	{ _64QAM,0.819f },
	{ _64QAM,0.865f },
	{ _64QAM,0.911f },
};



double positiveNumber_2_dB(double x) {
	return 10 * log10(x);
}

double g_dB2Real(double x) {
	return pow(10, x / 10);
}






int gt_H2Subband(int _H_Idx) {
	return _H_2_SubbandTable[_H_Idx];
}


int gt_LUTCodewords2LayersNum(eTransMode mod, int layers, int codewords, int codewordsIdx) {
	int ret = 0;
	if (codewords < 1 || codewords>2) throw Exp("Global.cpp->LUT_Codewords2LayersNum");
	//assert(codewords >= 1 && codewords <= 2);
	if (layers < 1 || layers>8) throw Exp("Global.cpp->LUT_Codewords2LayersNum");
	//assert(layers >= 1 && layers <= 8);
	if (codewordsIdx < 0 || codewordsIdx >= codewords) throw Exp("Global.cpp->LUT_Codewords2LayersNum");
	//assert(codewordsIdx >= 0 && codewordsIdx<codewords);
	if (mod == TM_1)
		ret = 1;
	else if (mod == CODEBOOK || mod == BEAMFORMING) {
		if (codewords == 1)
			ret = layers;
		else {
			int first = layers >> 1;
			int second = layers - first;
			ret = codewordsIdx == 0 ? first : second;
		}
	}
	return ret;
}

Matrix Codebook::getPrecodingMatrix(int nTx, int PMI, int RI) {
	Matrix ret;
	if (nTx == 4) {
		if (!(PMI <= 16))throw Exp("GetPrecodingMatrix'nTx==4 Error");
		ret = precoding_4[RI - 1][PMI];
	}
	else if (nTx == 2)
		ret = precoding_2[RI - 1][PMI];
	else if (nTx == 8)
		ret = _W[RI - 1][PMI];
	else
		throw Exp("GetPrecodingMatrix'nTx==else Error");
	return ret;
}


double gt_LUTMCSIndex2Effective(int McsIndex, eLinkType link) {
	float mod;
	ModulationType modtype;
	float rate;
	if (link == DOWNLINK) {
		if (McsIndex == -1) {
			rate = 0;
			modtype = BPSK;
		}
		else if (McsIndex >= 0 && McsIndex<DOWNLINK_MCS_LEVELS) {
			rate = _MCS_Level_DL[McsIndex].rate;
			modtype = _MCS_Level_DL[McsIndex].modulationType;
		}
		else
			throw Exp("lut mcs index to effective fail\n");

	}
	else if (link == UPLINK) {
		if (McsIndex == -1) {
			rate = 0;
			modtype = BPSK;
		}
		else if (McsIndex >= 0 && McsIndex<UPLINK) {
			rate = _MCS_Level_UP[McsIndex].rate;
			modtype = _MCS_Level_UP[McsIndex].modulationType;
		}
		else
			throw Exp("lut mcs index to effective fail\n");

	}
	switch (modtype)
	{
	case BPSK:
		mod = 1.0f;
		break;
	case QPSK:
		mod = 2.0f;
		break;
	case _16QAM:
		mod = 4.0f;
		break;
	case _64QAM:
		mod = 6.0f;
		break;
	}
	return rate*mod;
}

int gt_LUTSearchMCSIndex(eLinkType link, double sinr, double target_bler) {
	int ret;
	if (link == DOWNLINK) {
		int mcs_idx;
		for (mcs_idx = DOWNLINK_MCS_LEVELS - 1; mcs_idx != -1; mcs_idx--) {
			if (gt_LUTSNR2BLER(sinr, mcs_idx, DOWNLINK)<target_bler + 0.0001)
				break;
		}
		ret = mcs_idx;
	}
	else if (link == UPLINK) {
		int mcs_idx;
		for (mcs_idx = UPLINK_MCS_LEVELS - 1; mcs_idx != -1; mcs_idx--) {
			if (gt_LUTSNR2BLER(sinr, mcs_idx, UPLINK)<target_bler + 0.0001)
				break;
		}
		ret = mcs_idx;
	}
	return ret;
}

double gt_LUTSNR2BLER(double snr, int mcs_level, eLinkType link) {
	double bmcs = 0;
	double cmcs = 0;
	gt_LUTBlerPara(mcs_level, link, &bmcs, &cmcs);
	return 0.5f*(1.0f - (float)erf(((snr - bmcs) / cmcs)));
}


void gt_LUTBlerPara(int mcs_level, eLinkType link, double* bmcs, double* cmcs) {
	if (link == DOWNLINK) {
		*bmcs = g_MCS_SNR_BLER_DOWNLINK_B[mcs_level];
		*cmcs = g_MCS_SNR_BLER_DOWNLINK_C[mcs_level];
	}
	if (link == UPLINK)
	{
		*bmcs = g_MCS_SNR_BLER_UPLINK_B[mcs_level];
		*cmcs = g_MCS_SNR_BLER_UPLINK_C[mcs_level];
	}
}


int gt_LUTLayer2Codeword(eTransMode mod, int layers, int codewords, int layerIdx)
{
	int ret = INVALID_CODEWORD;
	if (codewords < 1 || codewords>2) throw Exp("Global.cpp->LUT_Layer2Codeword");
	//assert(codewords >= 1 && codewords <= 2);
	if (layers < 1 || layers>8) throw Exp("Global.cpp->LUT_Layer2Codeword");
	//assert(layers >= 1 && layers <= 8);
	if (layerIdx < 0 || layerIdx >= codewords) throw Exp("Global.cpp->LUT_Layer2Codeword");
	//assert(layerIdx >= 0 && layerIdx<layers);
	if (mod == TM_1 || mod == TM_2) {
		//assert(layers == 1);
		//assert(codewords == 1);
		if (layers != 1)throw Exp("Global.cpp->LUT_Layer2Codeword");
		if (codewords != 1)throw Exp("Global.cpp->LUT_Layer2Codeword");
		return 0;
	}
	else if (mod == CODEBOOK || mod == BEAMFORMING) {
		switch (codewords) {
		case 1:
			ret = 0;
			break;
		case 2:
			ret = layerIdx;
			break;
		default:
			throw Exp("LUT_Layer2Codeword");
		}
	}
	return ret;
}

static int col_index[4][16][4] = { \
{ {1},{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 },{ 1 }},
{ { 1,4 },{ 1,2 },{ 1,2 },{ 1,2 },{ 1,4 },{ 1,4 },{ 1,3 },{ 1,3 },{ 1,2 },{ 1,4 },{ 1,3 },{ 1,3 },{ 1,2 },{ 1,3 },{ 1,3 },{ 1,2 } },
{ { 1,2,4 },{ 1,2,3 },{ 1,2,3 },{ 1,2,3 },{ 1,2,4 },{ 1,2,4 },{ 1,3,4 },{ 1,3,4 },{ 1,2,4 },{ 1,3,4 },{ 1,2,3 },{ 1,3,4 },{ 1,2,3 },{ 1,2,3 },{ 1,2,3 },{ 1,2,3 } },
{ { 1,2,3,4 },{ 1,2,3,4 },{ 3,2,1,4 },{ 3,2,1,4 },{ 1,2,3,4 },{ 1,2,3,4 },{ 1,3,2,4 },{ 1,3,2,4 },{ 1,2,3,4 },{ 1,2,3,4 },{ 1,3,2,4 },{ 1,3,2,4 },{ 1,2,3,4 },{ 1,3,2,4 },{ 3,2,1,4 },{ 1,2,3,4 } }
};

Codebook::Codebook()
{
	for (int i = 0; i != 16; ++i)
	{
		this->u[i].initialize(4, 1);
	}

	this->u[0].set(Complex(1, 0), 0, 0);
	this->u[0].set(Complex(-1, 0), 1, 0);
	this->u[0].set(Complex(-1, 0), 2, 0);
	this->u[0].set(Complex(-1, 0), 3, 0);

	this->u[1].set(Complex(1, 0), 0, 0);
	this->u[1].set(Complex(0, -1), 1, 0);
	this->u[1].set(Complex(1, 0), 2, 0);
	this->u[1].set(Complex(0, 1), 3, 0);

	this->u[2].set(Complex(1, 0), 0, 0);
	this->u[2].set(Complex(1, 0), 1, 0);
	this->u[2].set(Complex(-1, 0), 2, 0);
	this->u[2].set(Complex(1, 0), 3, 0);

	this->u[3].set(Complex(1, 0), 0, 0);
	this->u[3].set(Complex(0, 1), 1, 0);
	this->u[3].set(Complex(1, 0), 2, 0);
	this->u[3].set(Complex(0, -1), 3, 0);

	this->u[4].set(Complex(1, 0), 0, 0);
	this->u[4].set(Complex(-sqrt(2.0) / 2, -sqrt(2.0) / 2), 1, 0);
	this->u[4].set(Complex(0, -1), 2, 0);
	this->u[4].set(Complex(sqrt(2.0) / 2, -sqrt(2.0) / 2), 3, 0);

	this->u[5].set(Complex(1, 0), 0, 0);
	this->u[5].set(Complex(sqrt(2.0) / 2, -sqrt(2.0) / 2), 1, 0);
	this->u[5].set(Complex(0, 1), 2, 0);
	this->u[5].set(Complex(-sqrt(2.0) / 2, -sqrt(2.0) / 2), 3, 0);

	this->u[6].set(Complex(1, 0), 0, 0);
	this->u[6].set(Complex(sqrt(2.0) / 2, sqrt(2.0) / 2), 1, 0);
	this->u[6].set(Complex(0, -1), 2, 0);
	this->u[6].set(Complex(-sqrt(2.0) / 2, sqrt(2.0) / 2), 3, 0);

	this->u[7].set(Complex(1, 0), 0, 0);
	this->u[7].set(Complex(-sqrt(2.0) / 2, sqrt(2.0) / 2), 1, 0);
	this->u[7].set(Complex(0, 1), 2, 0);
	this->u[7].set(Complex(sqrt(2.0) / 2, sqrt(2.0) / 2), 3, 0);

	this->u[8].set(Complex(1, 0), 0, 0);
	this->u[8].set(Complex(-1, 0), 1, 0);
	this->u[8].set(Complex(1, 0), 2, 0);
	this->u[8].set(Complex(1, 0), 3, 0);

	this->u[9].set(Complex(1, 0), 0, 0);
	this->u[9].set(Complex(0, -1), 1, 0);
	this->u[9].set(Complex(-1, 0), 2, 0);
	this->u[9].set(Complex(0, -1), 3, 0);

	this->u[10].set(Complex(1, 0), 0, 0);
	this->u[10].set(Complex(1, 0), 1, 0);
	this->u[10].set(Complex(1, 0), 2, 0);
	this->u[10].set(Complex(-1, 0), 3, 0);

	this->u[11].set(Complex(1, 0), 0, 0);
	this->u[11].set(Complex(0, 1), 1, 0);
	this->u[11].set(Complex(-1, 0), 2, 0);
	this->u[11].set(Complex(0, 1), 3, 0);

	this->u[12].set(Complex(1, 0), 0, 0);
	this->u[12].set(Complex(-1, 0), 1, 0);
	this->u[12].set(Complex(-1, 0), 2, 0);
	this->u[12].set(Complex(1, 0), 3, 0);

	this->u[13].set(Complex(1, 0), 0, 0);
	this->u[13].set(Complex(-1, 0), 1, 0);
	this->u[13].set(Complex(1, 0), 2, 0);
	this->u[13].set(Complex(-1, 0), 3, 0);

	this->u[14].set(Complex(1, 0), 0, 0);
	this->u[14].set(Complex(1, 0), 1, 0);
	this->u[14].set(Complex(-1, 0), 2, 0);
	this->u[14].set(Complex(-1, 0), 3, 0);

	this->u[15].set(Complex(1, 0), 0, 0);
	this->u[15].set(Complex(1, 0), 1, 0);
	this->u[15].set(Complex(1, 0), 2, 0);
	this->u[15].set(Complex(1, 0), 3, 0);

	Matrix I = Matrix(4, 4);
	I.eye();
	for (int i = 0; i != 16; ++i)
	{
		Complex uu = (u[i].hermitian()*(u[i])).m_Mem[0];
		uu = uu / 2.0;
		this->w[i] = I - u[i] * u[i].hermitian() / uu;
	}

	for (int layer = 0; layer != 4; ++layer)
	{
		for (int pmi = 0; pmi != 16; ++pmi)
		{
			precoding_4[layer][pmi].initialize(4, layer + 1);

			for (int c = 0; c != layer + 1; ++c)
			{
				for (int r = 0; r != 4; ++r)
				{
					precoding_4[layer][pmi].set(w[pmi].get(r, col_index[layer][pmi][c] - 1), r, c);
				}
			}
			if (layer == 1)
			{
				precoding_4[layer][pmi] = precoding_4[layer][pmi] / Complex(sqrt(2.0), 0.0);
			}
			if (layer == 2)
			{
				precoding_4[layer][pmi] = precoding_4[layer][pmi] / Complex(sqrt(3.0), 0.0);
			}
			if (layer == 3)
			{
				precoding_4[layer][pmi] = precoding_4[layer][pmi] / Complex(2.0, 0.0);
			}

		}

	}
	// for (int i = 0;i!=16;i++)
	// {
	//         fprintf(stderr,"\nPMI = %d\n",i);
	//    
	//        
	//         for(int j = 0;j!=precoding4[0][i].rows*precoding4[0][i].cols;j++)
	//         {
	//                 fprintf(stderr,"%f + %f j\n",precoding4[0][i].mem[j].real,precoding4[0][i].mem[j].imag);
	//         }
	//        
	// }

	precoding_2[0][0].initialize(2, 1);
	precoding_2[0][0].set(Complex(sqrt(2.0) / 2, 0.0), 0, 0);
	precoding_2[0][0].set(Complex(sqrt(2.0) / 2, 0.0), 1, 0);

	precoding_2[0][1].initialize(2, 1);
	precoding_2[0][1].set(Complex(sqrt(2.0) / 2, 0.0), 0, 0);
	precoding_2[0][1].set(Complex(-sqrt(2.0) / 2, 0.0), 1, 0);

	precoding_2[0][2].initialize(2, 1);
	precoding_2[0][2].set(Complex(sqrt(2.0) / 2, 0.0), 0, 0);
	precoding_2[0][2].set(Complex(0.0, sqrt(2.0) / 2), 1, 0);

	precoding_2[0][3].initialize(2, 1);
	precoding_2[0][3].set(Complex(sqrt(2.0) / 2, 0.0), 0, 0);
	precoding_2[0][3].set(Complex(0.0, -sqrt(2.0) / 2), 1, 0);

	precoding_2[1][0].initialize(2, 2);
	precoding_2[1][0].set(Complex(sqrt(2.0) / 2, 0.0), 0, 0);
	precoding_2[1][0].set(Complex(0.0, 0.0), 0, 1);
	precoding_2[1][0].set(Complex(0.0, 0.0), 1, 0);
	precoding_2[1][0].set(Complex(sqrt(2.0) / 2, 0.0), 1, 1);

	precoding_2[1][1].initialize(2, 2);
	precoding_2[1][1].set(Complex(0.5, 0.0), 0, 0);
	precoding_2[1][1].set(Complex(0.5, 0.0), 0, 1);
	precoding_2[1][1].set(Complex(0.5, 0.0), 1, 0);
	precoding_2[1][1].set(Complex(-0.5, 0.0), 1, 1);



	precoding_2[1][2].initialize(2, 2);
	precoding_2[1][2].set(Complex(0.5, 0.0), 0, 0);
	precoding_2[1][2].set(Complex(0.5, 0.0), 0, 1);
	precoding_2[1][2].set(Complex(0.0, 0.5), 1, 0);
	precoding_2[1][2].set(Complex(0.0, -0.5), 1, 1);

	//8*8码本
	int i1, i2, layer; //i1,i2索引号
	for (layer = 1; layer <= 8; layer++)
	{
		switch (layer)
		{
		case 1: //layer=1
		{
			Matrix w1(8, layer);
			Complex * c1 = new Complex;
			for (i2 = 0; i2 <= 3; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 4)*gc_PI / 2), sin((i2 % 4)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n)=W(2*i1,i2%4)
					for (int rr1 = 0, cc1 = 0; rr1 <= 3; rr1++)
					{

						c1->m_Real = 1 / sqrt(8.0)*cos(2 * rr1*gc_PI * 2 * i1 / 32);
						c1->m_Imag = 1 / sqrt(8.0)*sin(2 * rr1*gc_PI * 2 * i1 / 32);
						w1.set(*c1, rr1, cc1);
					}
					for (int rr1 = 4, cc1 = 0; rr1 <= 7; rr1++)
					{

						c1->m_Real = 1 / sqrt(8.0)*cos(2 * (rr1 - 4)*gc_PI * 2 * i1 / 32);
						c1->m_Imag = 1 / sqrt(8.0)*sin(2 * (rr1 - 4)*gc_PI * 2 * i1 / 32);
						*c1 = *c1*(*fain);
						w1.set(*c1, rr1, cc1);

					}
					_W[layer - 1][i2 * 16 + i1] = w1;
				}
				delete fain;
			}
			for (i2 = 4; i2 <= 7; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 4)*gc_PI / 2), sin((i2 % 4)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n)=W(2*i1+1,i2%4)
					for (int rr1 = 0, cc1 = 0; rr1 <= 3; rr1++)
					{

						c1->m_Real = 1 / sqrt(8.0)*cos(2 * rr1*gc_PI*(2 * i1 + 1) / 32);
						c1->m_Imag = 1 / sqrt(8.0)*sin(2 * rr1*gc_PI*(2 * i1 + 1) / 32);
						w1.set(*c1, rr1, cc1);//void set(cComplex& a,int row,int col);
					}
					for (int rr1 = 4, cc1 = 0; rr1 <= 7; rr1++)
					{

						c1->m_Real = 1 / sqrt(8.0)*cos(2 * (rr1 - 4)*gc_PI*(2 * i1 + 1) / 32);
						c1->m_Imag = 1 / sqrt(8.0)*sin(2 * (rr1 - 4)*gc_PI*(2 * i1 + 1) / 32);
						*c1 = *c1*(*fain);
						w1.set(*c1, rr1, cc1);//void set(cComplex& a,int row,int col);
					}
					_W[layer - 1][i2 * 16 + i1] = w1;
				}
				delete fain;
			}
			for (i2 = 8; i2 <= 11; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 4)*gc_PI / 2), sin((i2 % 4)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n)=W(2*i1+2,i2%4)
					for (int rr1 = 0, cc1 = 0; rr1 <= 3; rr1++)
					{

						c1->m_Real = 1 / sqrt(8.0)*cos(2 * rr1*gc_PI*(2 * i1 + 2) / 32);
						c1->m_Imag = 1 / sqrt(8.0)*sin(2 * rr1*gc_PI*(2 * i1 + 2) / 32);
						w1.set(*c1, rr1, cc1);//void set(cComplex& a,int row,int col);
					}
					for (int rr1 = 4, cc1 = 0; rr1 <= 7; rr1++)
					{
						c1->m_Real = 1 / sqrt(8.0)*cos(2 * (rr1 - 4)*gc_PI*(2 * i1 + 2) / 32);
						c1->m_Imag = 1 / sqrt(8.0)*sin(2 * (rr1 - 4)*gc_PI*(2 * i1 + 2) / 32);
						*c1 = *c1*(*fain);
						w1.set(*c1, rr1, cc1);
					}
					_W[layer - 1][i2 * 16 + i1] = w1;
				}
				delete fain;
			}
			for (i2 = 12; i2 <= 15; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 4)*gc_PI / 2), sin((i2 % 4)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n)=W(2*i1+3,i2%4)
					for (int rr1 = 0, cc1 = 0; rr1 <= 3; rr1++)
					{
						c1->m_Real = 1 / sqrt(8.0)*cos(2 * rr1*gc_PI*(2 * i1 + 3) / 32);
						c1->m_Imag = 1 / sqrt(8.0)*sin(2 * rr1*gc_PI*(2 * i1 + 3) / 32);
						w1.set(*c1, rr1, cc1);
					}
					for (int rr1 = 4, cc1 = 0; rr1 <= 7; rr1++)
					{
						c1->m_Real = 1 / sqrt(8.0)*cos(2 * (rr1 - 4)*gc_PI*(2 * i1 + 3) / 32);
						c1->m_Imag = 1 / sqrt(8.0)*sin(2 * (rr1 - 4)*gc_PI*(2 * i1 + 3) / 32);
						*c1 = *c1*(*fain);
						w1.set(*c1, rr1, cc1);//void set(cComplex& a,int row,int col);
					}
					_W[layer - 1][i2 * 16 + i1] = w1;
				}
				delete fain;
			}
			delete c1;
			break;
		}
		case 2: //layer=2
		{
			Matrix w2(8, 2);
			Complex * c2 = new Complex;
			for (i2 = 0; i2 <= 1; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n,p)=W(2*i1,2*i1,i2%2)
					for (int rr2 = 0, cc2 = 0; rr2 <= 3; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							c2->m_Real = 0.25*cos(2 * rr2*gc_PI * 2 * i1 / 32);
							c2->m_Imag = 0.25*sin(2 * rr2*gc_PI * 2 * i1 / 32);
							w2.set(*c2, rr2, cc2);
						}
					}
					for (int rr2 = 4, cc2 = 0; rr2 <= 7; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI * 2 * i1 / 32);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI * 2 * i1 / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI * 2 * i1 / 32)*(-1);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI * 2 * i1 / 32)*(-1);
							}
							*c2 = (*fain)*(*c2);
							w2.set(*c2, rr2, cc2);
						}
					}
					_W[layer - 1][i2 * 16 + i1] = w2;
				}
				delete fain;
			}
			for (i2 = 2; i2 <= 3; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n,p)=W(2*i1+1,2*i1+1,i2%2)
					for (int rr2 = 0, cc2 = 0; rr2 <= 3; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 1) / 32);
							c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 1) / 32);
							w2.set(*c2, rr2, cc2);//void set(cComplex& a,int row,int col);
						}
					}
					for (int rr2 = 4, cc2 = 0; rr2 <= 7; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32)*(-1);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32)*(-1);
							}
							*c2 = (*fain)*(*c2);
							w2.set(*c2, rr2, cc2);
						}
					}
					_W[layer - 1][i2 * 16 + i1] = w2;
				}
				delete fain;
			}
			for (i2 = 4; i2 <= 5; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n,p)=W(2*i1+2,2*i1+2,i2%2)
					for (int rr2 = 0, cc2 = 0; rr2 <= 3; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 2) / 32);
							c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 2) / 32);
							w2.set(*c2, rr2, cc2);
						}
					}
					for (int rr2 = 4, cc2 = 0; rr2 <= 7; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 2) / 32);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 2) / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 2) / 32)*(-1);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 2) / 32)*(-1);
							}
							*c2 = (*fain)**c2;
							w2.set(*c2, rr2, cc2);
						}
					}
					_W[layer - 1][i2 * 16 + i1] = w2;
				}
				delete fain;
			}
			for (i2 = 6; i2 <= 7; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n,p)=W(2*i1+3,2*i1+3,i2%2)
					for (int rr2 = 0, cc2 = 0; rr2 <= 3; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 3) / 32);
							c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 3) / 32);
							w2.set(*c2, rr2, cc2);
						}
					}
					for (int rr2 = 4, cc2 = 0; rr2 <= 7; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 3) / 32);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 3) / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 3) / 32)*(-1);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 3) / 32)*(-1);
							}
							*c2 = (*fain)**c2;
							w2.set(*c2, rr2, cc2);
						}
					}
					_W[layer - 1][i2 * 16 + i1] = w2;
				}
				delete fain;
			}
			for (i2 = 8; i2 <= 9; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n,p)=W(2*i1,2*i1+1,i2%2)
					for (int rr2 = 0, cc2 = 0; rr2 <= 3; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * rr2*gc_PI * 2 * i1 / 32);
								c2->m_Imag = 0.25*sin(2 * rr2*gc_PI * 2 * i1 / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 1) / 32);
								c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 1) / 32);
							}
							w2.set(*c2, rr2, cc2);
						}
					}
					for (int rr2 = 4, cc2 = 0; rr2 <= 7; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI * 2 * i1 / 32);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI * 2 * i1 / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32)*(-1);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32)*(-1);
							}
							*c2 = (*fain)**c2;
							w2.set(*c2, rr2, cc2);
						}
					}
					_W[layer - 1][i2 * 16 + i1] = w2;
				}
				delete fain;
			}
			for (i2 = 10; i2 <= 11; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n,p)=W(2*i1+1,2*i1+2,i2%2)
					for (int rr2 = 0, cc2 = 0; rr2 <= 3; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 1) / 32);
								c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 1) / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 2) / 32);
								c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 2) / 32);
							}
							w2.set(*c2, rr2, cc2);//void set(cComplex& a,int row,int col);
						}
					}
					for (int rr2 = 4, cc2 = 0; rr2 <= 7; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 2) / 32)*(-1);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 2) / 32)*(-1);
							}
							*c2 = (*fain)**c2;
							w2.set(*c2, rr2, cc2);
						}
					}
					_W[layer - 1][i2 * 16 + i1] = w2;
				}
				delete fain;
			}
			for (i2 = 12; i2 <= 13; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n,p)=W(2*i1,2*i1+3,i2%2)
					for (int rr2 = 0, cc2 = 0; rr2 <= 3; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * rr2*gc_PI * 2 * i1 / 32);
								c2->m_Imag = 0.25*sin(2 * rr2*gc_PI * 2 * i1 / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 3) / 32);
								c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 3) / 32);
							}
							w2.set(*c2, rr2, cc2);
						}
					}
					for (int rr2 = 4, cc2 = 0; rr2 <= 7; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI * 2 * i1 / 32);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI * 2 * i1 / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 3) / 32)*(-1);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 3) / 32)*(-1);
							}
							*c2 = (*fain)**c2;
							w2.set(*c2, rr2, cc2);
						}
					}
					_W[layer - 1][i2 * 16 + i1] = w2;
				}
				delete fain;
			}
			for (i2 = 14; i2 <= 15; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 15; i1++)
				{
					//W(m,n,p)=W(2*i1+1,2*i1+3,i2%2)
					for (int rr2 = 0, cc2 = 0; rr2 <= 3; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 1) / 32);
								c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 1) / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * rr2*gc_PI*(2 * i1 + 3) / 32);
								c2->m_Imag = 0.25*sin(2 * rr2*gc_PI*(2 * i1 + 3) / 32);
							}
							w2.set(*c2, rr2, cc2);//void set(cComplex& a,int row,int col);
						}
					}
					for (int rr2 = 4, cc2 = 0; rr2 <= 7; rr2++)
					{
						for (cc2 = 0; cc2 <= 1; cc2++)
						{
							if (cc2 == 0)
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 1) / 32);
							}
							else
							{
								c2->m_Real = 0.25*cos(2 * (rr2 - 4)*gc_PI*(2 * i1 + 3) / 32)*(-1);
								c2->m_Imag = 0.25*sin(2 * (rr2 - 4)*gc_PI*(2 * i1 + 3) / 32)*(-1);
							}
							*c2 = (*fain)**c2;
							w2.set(*c2, rr2, cc2);
						}
					}
					_W[layer - 1][i2 * 16 + i1] = w2;
				}
				delete fain;
			}
			delete c2;
			break;
		}
		case 3: //layer=3   
		{
			Matrix  w3(8, 3);
			Complex * c3 = new Complex;
			for (i2 = 0; i2 <= 15; i2++)
			{
				switch (i2)
				{
				case 0:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1,8*i1,8*i1+8)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0 || cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI * 8 * i1 / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI * 8 * i1 / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 8) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 8) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32)*(-1);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 1:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+8,8*i1,8*i1+8)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI * 8 * i1 / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI * 8 * i1 / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 8) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 8) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32)*(-1);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 2:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1,8*i1+8,8*i1+8)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI * 8 * i1 / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI * 8 * i1 / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 8) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 8) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 3:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+8,8*i1,8*i1)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 1 || cc3 == 2)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI * 8 * i1 / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI * 8 * i1 / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 8) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 8) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 8) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI * 8 * i1 / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 4:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+2,8*i1+2,8*i1+10)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0 || cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 2) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 2) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 10) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 10) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32)*(-1);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 5:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+10,8*i1+2,8*i1+10)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 2) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 2) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 10) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 10) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32)*(-1);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 6:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+2,8*i1+10,8*i1+10)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 2) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 2) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 10) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 10) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 7:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+10,8*i1+2,8*i1+2)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 10) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 10) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 2) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 2) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 10) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 2) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 8:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+4,8*i1+4,8*i1+12)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 2)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 12) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 12) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 4) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 4) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32)*(-1);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 9:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+12,8*i1+4,8*i1+12)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 4) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 4) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 12) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 12) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32)*(-1);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 10:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+4,8*i1+12,8*i1+12)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 4) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 4) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 12) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 12) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 11:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+12,8*i1+4,8*i1+4)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 12) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 12) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 4) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 4) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 12) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 4) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 12:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+6,8*i1+6,8*i1+14)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 2)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 14) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 14) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 6) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 6) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32)*(-1);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 13:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+14,8*i1+6,8*i1+14)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 6) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 6) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 14) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 14) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32)*(-1);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 14:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+6,8*i1+14,8*i1+14)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 6) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 6) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 14) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 14) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				case 15:
				{
					for (i1 = 0; i1 <= 3; i1++)
					{
						//W(m,n,p)=W(8*i1+14,8*i1+6,8*i1+6)
						for (int rr3 = 0, cc3 = 0; rr3 <= 3; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 14) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 14) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * rr3*gc_PI*(8 * i1 + 6) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * rr3*gc_PI*(8 * i1 + 6) / 32);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						for (int rr3 = 4, cc3 = 0; rr3 <= 7; rr3++)
						{
							for (cc3 = 0; cc3 <= 2; cc3++)
							{
								if (cc3 == 0)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 14) / 32);
								}
								else if (cc3 == 1)
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32);
								}
								else
								{
									c3->m_Real = 1 / sqrt(24.0)*cos(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32)*(-1);
									c3->m_Imag = 1 / sqrt(24.0)*sin(2 * (rr3 - 4)*gc_PI*(8 * i1 + 6) / 32)*(-1);
								}
								w3.set(*c3, rr3, cc3);
							}
						}
						_W[layer - 1][i2 * 4 + i1] = w3;
					}
					break;
				}
				}
			}
			delete c3;
			break;
		}
		case 4: //layer=4   
		{
			Matrix  w4(8, 4);
			Complex * c4 = new Complex;
			for (i2 = 0; i2 <= 1; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 3; i1++)
				{
					//W(m,n,p)=W(8*i1,8*i1+8,i2%2)
					for (int rr4 = 0, cc4 = 0; rr4 <= 3; rr4++)
					{
						for (cc4 = 0; cc4 <= 3; cc4++)
						{
							if (cc4 == 0 || cc4 == 2)
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * rr4*gc_PI * 8 * i1 / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * rr4*gc_PI * 8 * i1 / 32);
							}
							else
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * rr4*gc_PI*(8 * i1 + 8) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * rr4*gc_PI*(8 * i1 + 8) / 32);
							}
							w4.set(*c4, rr4, cc4);
						}
					}
					for (int rr4 = 4, cc4 = 0; rr4 <= 7; rr4++)
					{
						for (cc4 = 0; cc4 <= 3; cc4++)
						{
							switch (cc4)
							{
							case 0:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI * 8 * i1 / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI * 8 * i1 / 32);
								break;
							}
							case 1:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 8) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 8) / 32);
								break;
							}
							case 2:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI * 8 * i1 / 32)*(-1);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI * 8 * i1 / 32)*(-1);
								break;
							}
							case 3:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 8) / 32)*(-1);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 8) / 32)*(-1);
								break;
							}
							}
							*c4 = (*fain)**c4;
							w4.set(*c4, rr4, cc4);
						}
					}
					_W[layer - 1][i2 * 4 + i1] = w4;
				}
				delete fain;
			}
			for (i2 = 2; i2 <= 3; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 3; i1++)
				{
					//W(m,n,p)=W(8*i1+2,8*i1+10,i2%2)
					for (int rr4 = 0, cc4 = 0; rr4 <= 3; rr4++)
					{
						for (cc4 = 0; cc4 <= 3; cc4++)
						{
							if (cc4 == 0 || cc4 == 2)
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * rr4*gc_PI*(8 * i1 + 2) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * rr4*gc_PI*(8 * i1 + 2) / 32);
							}
							else
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * rr4*gc_PI*(8 * i1 + 10) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * rr4*gc_PI*(8 * i1 + 10) / 32);
							}
							w4.set(*c4, rr4, cc4);
						}
					}
					for (int rr4 = 4, cc4 = 0; rr4 <= 7; rr4++)
					{
						for (cc4 = 0; cc4 <= 3; cc4++)
						{
							switch (cc4)
							{
							case 0:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 2) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 2) / 32);
								break;
							}
							case 1:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 10) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 10) / 32);
								break;
							}
							case 2:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 2) / 32)*(-1);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 2) / 32)*(-1);
								break;
							}
							case 3:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 10) / 32)*(-1);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 10) / 32)*(-1);
								break;
							}
							}
							*c4 = (*fain)**c4;
							w4.set(*c4, rr4, cc4);
						}
					}
					_W[layer - 1][i2 * 4 + i1] = w4;
				}
				delete fain;
			}
			for (i2 = 4; i2 <= 5; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 3; i1++)
				{
					//W(m,n,p)=W(8*i1+4,8*i1+12,i4%2)
					for (int rr4 = 0, cc4 = 0; rr4 <= 3; rr4++)
					{
						for (cc4 = 0; cc4 <= 3; cc4++)
						{
							if (cc4 == 0 || cc4 == 2)
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * rr4*gc_PI*(8 * i1 + 4) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * rr4*gc_PI*(8 * i1 + 4) / 32);
							}
							else
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * rr4*gc_PI*(8 * i1 + 12) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * rr4*gc_PI*(8 * i1 + 12) / 32);
							}
							w4.set(*c4, rr4, cc4);
						}
					}
					for (int rr4 = 4, cc4 = 0; rr4 <= 7; rr4++)
					{
						for (cc4 = 0; cc4 <= 3; cc4++)
						{
							switch (cc4)
							{
							case 0:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 4) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 4) / 32);
								break;
							}
							case 1:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 12) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 12) / 32);
								break;
							}
							case 2:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 4) / 32)*(-1);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 4) / 32)*(-1);
								break;
							}
							case 3:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 12) / 32)*(-1);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 12) / 32)*(-1);
								break;
							}
							}
							*c4 = (*fain)**c4;
							w4.set(*c4, rr4, cc4);
						}
					}
					_W[layer - 1][i2 * 4 + i1] = w4;
				}
				delete fain;
			}
			for (i2 = 6; i2 <= 7; i2++)
			{
				Complex * fain = new Complex(cos((i2 % 2)*gc_PI / 2), sin((i2 % 2)*gc_PI / 2));
				for (i1 = 0; i1 <= 3; i1++)
				{
					//W(m,n,p)=W(8*i1+6,8*i1+14,i4%2)
					for (int rr4 = 0, cc4 = 0; rr4 <= 3; rr4++)
					{
						for (cc4 = 0; cc4 <= 3; cc4++)
						{
							if (cc4 == 0 || cc4 == 2)
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * rr4*gc_PI*(8 * i1 + 6) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * rr4*gc_PI*(8 * i1 + 6) / 32);
							}
							else
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * rr4*gc_PI*(8 * i1 + 14) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * rr4*gc_PI*(8 * i1 + 14) / 32);
							}
							w4.set(*c4, rr4, cc4);
						}
					}
					for (int rr4 = 4, cc4 = 0; rr4 <= 7; rr4++)
					{
						for (cc4 = 0; cc4 <= 3; cc4++)
						{
							switch (cc4)
							{
							case 0:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 6) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 6) / 32);
								break;
							}
							case 1:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 14) / 32);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 14) / 32);
								break;
							}
							case 2:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 6) / 32)*(-1);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 6) / 32)*(-1);
								break;
							}
							case 3:
							{
								c4->m_Real = 1 / sqrt(32.0)*cos(2 * (rr4 - 4)*gc_PI*(8 * i1 + 14) / 32)*(-1);
								c4->m_Imag = 1 / sqrt(32.0)*sin(2 * (rr4 - 4)*gc_PI*(8 * i1 + 14) / 32)*(-1);
								break;
							}
							}
							*c4 = (*fain)**c4;
							w4.set(*c4, rr4, cc4);
						}
					}
					_W[layer - 1][i2 * 4 + i1] = w4;
				}
				delete fain;
			}
			delete c4;
			break;
		}
		case 5: //layer=5
		{
			Matrix  w5(8, 5);
			Complex * c5 = new Complex;
			for (i1 = 0, i2 = 0; i1 <= 3; i1++)
			{
				//W(m)=W(i1)
				for (int rr5 = 0, cc5 = 0; rr5 <= 3; rr5++)
				{
					for (cc5 = 0; cc5 <= 4; cc5++)
					{
						if (cc5 == 0 || cc5 == 1)
						{
							c5->m_Real = 1 / sqrt(40.0)*cos(2 * rr5*gc_PI * 2 * i1 / 32);
							c5->m_Imag = 1 / sqrt(40.0)*sin(2 * rr5*gc_PI * 2 * i1 / 32);
						}
						else if (cc5 == 2 || cc5 == 3)
						{
							c5->m_Real = 1 / sqrt(40.0)*cos(2 * rr5*gc_PI*(2 * i1 + 8) / 32);
							c5->m_Imag = 1 / sqrt(40.0)*sin(2 * rr5*gc_PI*(2 * i1 + 8) / 32);
						}
						else
						{
							c5->m_Real = 1 / sqrt(40.0)*cos(2 * rr5*gc_PI*(2 * i1 + 16) / 32);
							c5->m_Imag = 1 / sqrt(40.0)*sin(2 * rr5*gc_PI*(2 * i1 + 16) / 32);
						}
						w5.set(*c5, rr5, cc5);
					}
				}
				for (int rr5 = 4, cc5 = 0; rr5 <= 7; rr5++)
				{
					for (cc5 = 0; cc5 <= 4; cc5++)
					{
						switch (cc5)
						{
						case 0:
						{
							c5->m_Real = 1 / sqrt(40.0)*cos(2 * (rr5 - 4)*gc_PI * 2 * i1 / 32);
							c5->m_Imag = 1 / sqrt(40.0)*sin(2 * (rr5 - 4)*gc_PI * 2 * i1 / 32);
							break;
						}
						case 1:
						{
							c5->m_Real = 1 / sqrt(40.0)*cos(2 * (rr5 - 4)*gc_PI * 2 * i1 / 32)*(-1);
							c5->m_Imag = 1 / sqrt(40.0)*sin(2 * (rr5 - 4)*gc_PI * 2 * i1 / 32)*(-1);
							break;
						}
						case 2:
						{
							c5->m_Real = 1 / sqrt(40.0)*cos(2 * (rr5 - 4)*gc_PI*(2 * i1 + 8) / 32);
							c5->m_Imag = 1 / sqrt(40.0)*sin(2 * (rr5 - 4)*gc_PI*(2 * i1 + 8) / 32);
							break;
						}
						case 3:
						{
							c5->m_Real = 1 / sqrt(40.0)*cos(2 * (rr5 - 4)*gc_PI*(2 * i1 + 8) / 32)*(-1);
							c5->m_Imag = 1 / sqrt(40.0)*sin(2 * (rr5 - 4)*gc_PI*(2 * i1 + 8) / 32)*(-1);
							break;
						}
						case 4:
						{
							c5->m_Real = 1 / sqrt(40.0)*cos(2 * (rr5 - 4)*gc_PI*(2 * i1 + 16) / 32);
							c5->m_Imag = 1 / sqrt(40.0)*sin(2 * (rr5 - 4)*gc_PI*(2 * i1 + 16) / 32);
							break;
						}
						}
						w5.set(*c5, rr5, cc5);
					}
				}
				_W[layer - 1][i1] = w5;
			}
			delete c5;
			break;
		}
		case 6: //layer=6
		{
			Matrix w6(8, 6);
			Complex * c6 = new Complex;
			for (i1 = 0, i2 = 0; i1 <= 3; i1++)
			{
				//W(m)=W(i1)
				for (int rr6 = 0, cc6 = 0; rr6 <= 3; rr6++)
				{
					for (cc6 = 0; cc6 <= 5; cc6++)
					{
						if (cc6 == 0 || cc6 == 1)
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * rr6*gc_PI * 2 * i1 / 32);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * rr6*gc_PI * 2 * i1 / 32);
						}
						else if (cc6 == 2 || cc6 == 3)
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * rr6*gc_PI*(2 * i1 + 8) / 32);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * rr6*gc_PI*(2 * i1 + 8) / 32);
						}
						else
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * rr6*gc_PI*(2 * i1 + 16) / 32);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * rr6*gc_PI*(2 * i1 + 16) / 32);
						}
						w6.set(*c6, rr6, cc6);
					}
				}
				for (int rr6 = 4, cc6 = 0; rr6 <= 7; rr6++)
				{
					for (cc6 = 0; cc6 <= 5; cc6++)
					{
						switch (cc6)
						{
						case 0:
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * (rr6 - 4)*gc_PI * 2 * i1 / 32);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * (rr6 - 4)*gc_PI * 2 * i1 / 32);
							break;
						}
						case 1:
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * (rr6 - 4)*gc_PI * 2 * i1 / 32)*(-1);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * (rr6 - 4)*gc_PI * 2 * i1 / 32)*(-1);
							break;
						}
						case 2:
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * (rr6 - 4)*gc_PI*(2 * i1 + 8) / 32);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * (rr6 - 4)*gc_PI*(2 * i1 + 8) / 32);
							break;
						}
						case 3:
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * (rr6 - 4)*gc_PI*(2 * i1 + 8) / 32)*(-1);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * (rr6 - 4)*gc_PI*(2 * i1 + 8) / 32)*(-1);
							break;
						}
						case 4:
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * (rr6 - 4)*gc_PI*(2 * i1 + 16) / 32);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * (rr6 - 4)*gc_PI*(2 * i1 + 16) / 32);
							break;
						}
						case 5:
						{
							c6->m_Real = 1 / sqrt(48.0)*cos(2 * (rr6 - 4)*gc_PI*(2 * i1 + 16) / 32)*(-1);
							c6->m_Imag = 1 / sqrt(48.0)*sin(2 * (rr6 - 4)*gc_PI*(2 * i1 + 16) / 32)*(-1);
							break;
						}
						}
						w6.set(*c6, rr6, cc6);
					}
				}
				_W[layer - 1][i1] = w6;
			}
			delete c6;
			break;
		}
		case 7: //layer=7
		{
			Matrix  w7(8, 7);
			Complex * c7 = new Complex;
			for (i1 = 0, i2 = 0; i1 <= 3; i1++)
			{
				//W(m)=W(i1)
				for (int rr7 = 0, cc7 = 0; rr7 <= 3; rr7++)
				{
					for (cc7 = 0; cc7 <= 6; cc7++)
					{
						switch (cc7)
						{
						case 0:
						case 1:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * rr7*gc_PI * 2 * i1 / 32);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * rr7*gc_PI * 2 * i1 / 32);
							break;
						}
						case 2:
						case 3:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * rr7*gc_PI*(2 * i1 + 8) / 32);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * rr7*gc_PI*(2 * i1 + 8) / 32);
							break;
						}
						case 4:
						case 5:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * rr7*gc_PI*(2 * i1 + 16) / 32);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * rr7*gc_PI*(2 * i1 + 16) / 32);
							break;
						}
						case 6:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * rr7*gc_PI*(2 * i1 + 24) / 32);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * rr7*gc_PI*(2 * i1 + 24) / 32);
							break;
						}
						}
						w7.set(*c7, rr7, cc7);
					}
				}
				for (int rr7 = 4, cc7 = 0; rr7 <= 7; rr7++)
				{
					for (cc7 = 0; cc7 <= 6; cc7++)
					{
						switch (cc7)
						{
						case 0:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * (rr7 - 4)*gc_PI * 2 * i1 / 32);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * (rr7 - 4)*gc_PI * 2 * i1 / 32);
							break;
						}
						case 1:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * (rr7 - 4)*gc_PI * 2 * i1 / 32)*(-1);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * (rr7 - 4)*gc_PI * 2 * i1 / 32)*(-1);
							break;
						}
						case 2:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * (rr7 - 4)*gc_PI*(2 * i1 + 8) / 32);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * (rr7 - 4)*gc_PI*(2 * i1 + 8) / 32);
							break;
						}
						case 3:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * (rr7 - 4)*gc_PI*(2 * i1 + 8) / 32)*(-1);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * (rr7 - 4)*gc_PI*(2 * i1 + 8) / 32)*(-1);
							break;
						}
						case 4:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * (rr7 - 4)*gc_PI*(2 * i1 + 16) / 32);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * (rr7 - 4)*gc_PI*(2 * i1 + 16) / 32);
							break;
						}
						case 5:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * (rr7 - 4)*gc_PI*(2 * i1 + 16) / 32)*(-1);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * (rr7 - 4)*gc_PI*(2 * i1 + 16) / 32)*(-1);
							break;
						}
						case 6:
						{
							c7->m_Real = 1 / sqrt(56.0)*cos(2 * (rr7 - 4)*gc_PI*(2 * i1 + 24) / 32);
							c7->m_Imag = 1 / sqrt(56.0)*sin(2 * (rr7 - 4)*gc_PI*(2 * i1 + 24) / 32);
						}
						}
						w7.set(*c7, rr7, cc7);
					}
				}
				_W[layer - 1][i1] = w7;
			}
			delete c7;
			break;
		}
		case 8: //layer=8
		{
			Matrix w8(8, 8);
			Complex * c8 = new Complex;
			for (i1 = 0, i2 = 0; i1 <= 0; i1++)
			{
				//W(m)=W(i1)
				for (int rr8 = 0, cc8 = 0; rr8 <= 3; rr8++)
				{
					for (cc8 = 0; cc8 <= 7; cc8++)
					{
						switch (cc8)
						{
						case 0:
						case 1:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI * 2 * i1 / 32);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI * 2 * i1 / 32);
							break;
						}
						case 2:
						case 3:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 8) / 32);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 8) / 32);
							break;
						}
						case 4:
						case 5:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 16) / 32);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 16) / 32);
							break;
						}
						case 6:
						case 7:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 24) / 32);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 24) / 32);
							break;
						}
						}
						w8.set(*c8, rr8, cc8);
					}
				}
				for (int rr8 = 4, cc8 = 0; rr8 <= 7; rr8++)
				{
					for (cc8 = 0; cc8 <= 7; cc8++)
					{
						switch (cc8)
						{
						case 0:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI * 2 * i1 / 32);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI * 2 * i1 / 32);
							break;
						}
						case 1:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI * 2 * i1 / 32)*(-1);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI * 2 * i1 / 32)*(-1);
							break;
						}
						case 2:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 8) / 32);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 8) / 32);
							break;
						}
						case 3:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 8) / 32)*(-1);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 8) / 32)*(-1);
							break;
						}
						case 4:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 16) / 32);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 16) / 32);
							break;
						}
						case 5:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 16) / 32)*(-1);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 16) / 32)*(-1);
							break;
						}
						case 6:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 24) / 32);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 24) / 32);
							break;
						}
						case 7:
						{
							c8->m_Real = 0.125*cos(2 * rr8*gc_PI*(2 * i1 + 24) / 32)*(-1);
							c8->m_Imag = 0.125*sin(2 * rr8*gc_PI*(2 * i1 + 24) / 32)*(-1);
							break;
						}
						}
						w8.set(*c8, rr8, cc8);
					}
				}
				_W[layer - 1][i1] = w8;
			}
			delete c8;
			break;
		}//case 8
		}//switch
	}//for

	pmis_2[0] = 4;
	pmis_2[1] = 3;

	pmis_4[0] = 16;
	pmis_4[1] = 16;
	pmis_4[2] = 16;
	pmis_4[3] = 16;

	pmis_8[0] = 256;
	pmis_8[1] = 256;
	pmis_8[2] = 64;
	pmis_8[3] = 32;
	pmis_8[4] = 4;
	pmis_8[5] = 4;
	pmis_8[6] = 4;
	pmis_8[7] = 1;
}