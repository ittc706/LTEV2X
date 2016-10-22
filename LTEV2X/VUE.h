#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include<random>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"


class VeUE {
public:
	static int m_VeUECount;

	VeUE();
	~VeUE();
	void initializeUrban(VeUEConfigure &t_UEConfigure);
	void initializeHighSpeed(VeUEConfigure &t_UEConfigure);
	void initializeElse();//��������ģ��ĳ�Ա��ʼ������������GTATģ�飬��˳�ʼ��GTAT��Ϻ��ٵ��øú���

	//����Ƕ�׽ṹ��ǰ������
	struct GTAT;
	struct GTAT_Urban;
	struct GTAT_HighSpeed;
	struct RRM;
	struct RRM_DRA;
	struct RRM_RR;
	struct WT;
	struct TMAC;

	//���ڽṹ��ָ�룬ֻ����ָ����ʽ����Ϊ����ǰ�У��ṹ��Ķ�����δ���֣�ֻ�ܶ��岻��������
	GTAT* m_GTAT = nullptr;//���ڴ洢������ģ��ʹ�õĲ���
	GTAT_Urban* m_GTAT_Urban = nullptr;//���ڴ洢���򳡾����ض�����
	GTAT_HighSpeed* m_GTAT_HighSpeed = nullptr;//���ڴ洢���ٳ������ض�����
	RRM* m_RRM = nullptr;
	RRM_DRA* m_RRM_DRA = nullptr;//���ڴ洢DRAģʽ���ض�����
	RRM_RR* m_RRM_RR = nullptr;//���ڴ洢RRģʽ���ض�����
	WT* m_WT = nullptr;
	TMAC* m_TMAC = nullptr;
	

	const int m_VeUEId = m_VeUECount++;//����ID
	

	//�������ݽṹ����
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

	struct GTAT_Urban {
		int m_RoadId;
		int m_LocationId;
		double m_X;//��Ժ�����
		double m_Y;
		double m_AbsX;//���Ժ�����
		double m_AbsY;
		double m_V;//�ٶ�
		double m_VAngle;
		double m_FantennaAngle;
		IMTA *m_IMTA=nullptr;
	};

	struct GTAT_HighSpeed {
		int m_RoadId;
		double m_X;//��Ժ�����
		double m_Y;
		double m_AbsX;//���Ժ�����
		double m_AbsY;
		double m_V;//�ٶ�
		double m_VAngle;
		double m_FantennaAngle;
		IMTA *m_IMTA=nullptr;
	};

	struct RRM {
		int m_InterferenceVeUENum;//ͬƵ��������
		std::vector<int> m_InterferenceVeUEVec;//ͬƵ���ų���ID����������ǰ����
		int  m_PreModulation = 4;//��һ�εĵ��Ʒ�ʽ��WT_Bģ����Ҫ
	};

	struct RRM_DRA {
		static std::default_random_engine s_Engine;

		VeUE* m_This;//RRM_DRA���õ�GTAT����ز�������C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ
		
		std::tuple<int, int> m_ScheduleInterval;//��VeUE���ڴصĵ�ǰһ�ֵ�������

		RRM_DRA(VeUE* t_this);
		//��Ա����
		int DRARBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, MessageType messageType);
		int DRARBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx);

		std::string toString(int n);//���ڴ�ӡVeUE��Ϣ
	};

	struct RRM_RR {

	};

	struct WT {

	};

	struct TMAC {
		std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//����λ�ø�����־��Ϣ
	};
};


inline
int VeUE::RRM_DRA::DRARBSelectBasedOnP2(const std::vector<std::vector<int>>&curAvaliablePatternIdx, MessageType messageType) {
	int size = static_cast<int>(curAvaliablePatternIdx[messageType].size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return curAvaliablePatternIdx[messageType][u(s_Engine)];
}

inline
int VeUE::RRM_DRA::DRARBEmergencySelectBasedOnP2(const std::vector<int>&curAvaliableEmergencyPatternIdx) {
	int size = static_cast<int>(curAvaliableEmergencyPatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return curAvaliableEmergencyPatternIdx[u(s_Engine)];
}


