#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"


class cVeUE {
public:
	static int m_VeUECount;

	cVeUE();
	~cVeUE();
	void initializeUrban(sUEConfigure &t_UEConfigure);
	void initializeHigh(sUEConfigure &t_UEConfigure);

	//����Ƕ�׽ṹ��ǰ������
	struct GTAT;
	struct GTATUrban;
	struct GTATHigh;
	struct RRM;
	struct RRMDRA;
	struct RRMRR;

	//���ڽṹ��ָ�룬ֻ����ָ����ʽ����Ϊ����ǰ�У��ṹ��Ķ�����δ����
	GTAT* m_GTAT = nullptr;//���ڴ洢������ģ��ʹ�õĲ���
	GTATUrban* m_GTATUrban = nullptr;//���ڴ洢���򳡾����ض�����
	GTATHigh* m_GTATHigh = nullptr;//���ڴ洢���ٳ������ض�����
	RRM* m_RRM = nullptr;
	RRMDRA* m_RRMDRA = nullptr;//���ڴ洢DRAģʽ���ض�����
	RRMRR* m_RRMRR = nullptr;//���ڴ洢RRģʽ���ض�����
	

	const int m_VeUEId = m_VeUECount++;//����ID
	
	
	int m_InterVeUENum;//ͬƵ��������
	std::vector<int> m_InterVeUEVec;//ͬƵ���ų���ID����������ǰ����
	
	int  m_PreModulation;//��һ�εĵ��Ʒ�ʽ��WT_Bģ����Ҫ
	std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//����λ�ø�����־��Ϣ


	struct GTAT {
		int m_RSUId;//RRM_DRAģ����Ҫ
		int m_ClusterIdx;//RRM_DRAģ����Ҫ

		int m_Nt;//����������Ŀ��WT_Bģ����Ҫ
		int m_Nr;//����������Ŀ��WT_Bģ����Ҫ
		double m_Ploss;//·����ģ�WT_Bģ����Ҫ
		double *m_H=nullptr;//�ŵ���Ӧ����WT_Bģ����Ҫ

		std::vector<double> m_InterferencePloss;//����·����ģ�WT_Bģ����Ҫ
		double *m_InterferenceH = nullptr;//�����ŵ���Ӧ����WT_Bģ����Ҫ
	};

	struct GTATUrban {
		int m_wRoadID;
		int m_locationID;
		double m_fX;
		double m_fY;
		double m_fAbsX;
		double m_fAbsY;
		double m_fv;
		double m_fvAngle;
		double m_fantennaAngle;
		cIMTA *imta=nullptr;
	};

	struct GTATHigh {
		int m_wLaneID;
		double m_fX;
		double m_fY;
		double m_fAbsX;
		double m_fAbsY;
		double m_fv;
		double m_fvAngle;
		double m_fantennaAngle;
		cIMTA *imta=nullptr;
	};

	struct RRM {

	};

	struct RRMDRA {

	};

	struct RRMRR {

	};
};



