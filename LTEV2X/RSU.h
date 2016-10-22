#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include<tuple>
#include"Global.h"
#include"VUE.h"
#include"Event.h"
#include"IMTA.h"

class cRSU {
public:
	//����Ƕ�׽ṹ��ǰ������
	struct GTAT;
	struct GTATUrban;
	struct GTATHigh;
	struct RRM;
	struct RRMDRA;
	struct RRMRR;
	struct WT;
	struct TMAC;


	//���ڽṹ��ָ�룬ֻ����ָ����ʽ����Ϊ����ǰ�У��ṹ��Ķ�����δ���֣�ֻ�ܶ��岻��������
	GTAT* m_GTAT = nullptr;//���ڴ洢������ģ��ʹ�õĲ���
	GTATUrban* m_GTATUrban = nullptr;//���ڴ洢���򳡾����ض�����
	GTATHigh* m_GTATHigh = nullptr;//���ڴ洢���ٳ������ض�����
	RRM* m_RRM = nullptr;
	RRMDRA* m_RRMDRA = nullptr;//���ڴ洢DRAģʽ���ض�����
	RRMRR* m_RRMRR = nullptr;//���ڴ洢RRģʽ���ض�����
	WT* m_WT = nullptr;
	TMAC* m_TMAC = nullptr;


	cRSU();
	void initializeUrban(sRSUConfigure &t_RSUConfigure);
	void initializeHigh(sRSUConfigure &t_RSUConfigure);
	~cRSU();

	//�������ݽṹ����
	struct GTAT {
		int m_RSUId;
		std::list<int> m_VeUEIdList;//��ǰRSU��Χ�ڵ�VeUEId�������,RRM_DRAģ����Ҫ
		int m_DRAClusterNum;//һ��RSU���Ƿ�Χ�ڵĴصĸ���,RRM_DRAģ����Ҫ
		std::vector<std::list<int>> m_DRAClusterVeUEIdList;//���ÿ���ص�VeUE��Id������,�±����صı��
	};

	struct GTATUrban {
		double m_fAbsX;
		double m_fAbsY;
		cIMTA *imta;
		double m_fantennaAngle;
	};

	struct GTATHigh {
		double m_fAbsX;
		double m_fAbsY;
		cIMTA *imta;
		double m_fantennaAngle;
	};

	struct RRM {

	};

	struct RRMDRA {

	};

	struct RRMRR {

	};

	struct WT {

	};

	struct TMAC {

	};
};


