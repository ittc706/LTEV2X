#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include<random>
#include<map>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"


class VeUE {
public:
	//����Ƕ�׽ṹ��ǰ������
	struct GTT;
	struct GTT_Urban;
	struct GTT_HighSpeed;
	struct RRM;
	struct RRM_TDM_DRA;
	struct RRM_ICC_DRA;
	struct RRM_RR;
	struct WT;
	struct TMC;

	//���ڽṹ��ָ�룬ֻ����ָ����ʽ����Ϊ����ǰ�У��ṹ��Ķ�����δ���֣�ֻ�ܶ��岻��������
	GTT* m_GTT = nullptr;//���ڴ洢������ģ��ʹ�õĲ���
	GTT_Urban* m_GTT_Urban = nullptr;//���ڴ洢���򳡾����ض�����
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;//���ڴ洢���ٳ������ض�����
	RRM* m_RRM = nullptr;
	RRM_TDM_DRA* m_RRM_TDM_DRA = nullptr;//���ڴ洢RRM_TDM_DRAģʽ���ض�����
	RRM_ICC_DRA* m_RRM_ICC_DRA = nullptr;//���ڴ洢RRM_ICC_DRAģʽ���ض�����
	RRM_RR* m_RRM_RR = nullptr;//���ڴ洢RRģʽ���ض�����
	WT* m_WT = nullptr;
	TMC* m_TMC = nullptr;

	
	~VeUE();
	//ÿ�����ڽṹ�������Ӧģ������ɳ�ʼ��
	void initializeGTT_Urban(VeUEConfigure &t_UEConfigure);
	void initializeGTT_HighSpeed(VeUEConfigure &t_UEConfigure);
	void initializeRRM_TDM_DRA();
	void initializeRRM_ICC_DRA();
	void initializeRRM_RR();
	void initializeWT();
	void initializeTMC();

	static int m_VeUECount;

	const int m_VeUEId = m_VeUECount++;//����ID
	

	//�������ݽṹ����
	struct GTT {
		~GTT();

		int m_RSUId;//�������ڵ�RSUId
		int m_ClusterIdx;//�������ڴر��

		int m_Nt;//����������Ŀ��WT_Bģ����Ҫ
		int m_Nr;//����������Ŀ��WT_Bģ����Ҫ
		double m_Ploss;//·����ģ�WT_Bģ����Ҫ
		double* m_H;//�ŵ���Ӧ����WT_Bģ����Ҫ

		/*
		* �����������Ե�ǰ�����ĸ���·����ģ�WT_Bģ����Ҫ
		* �±꣺VeUEId(����һ��ʼ�Ϳ��ٺ����г����Ĳ�λ���ò��size����)
		*/
		std::vector<double> m_InterferencePloss;
		/*
		* �����������Ե�ǰ�������ŵ���Ӧ����WT_Bģ����Ҫ
		* �±꣺VeUEId��VeUEId(����һ��ʼ�Ϳ��ٺ����г����Ĳ�λ���ò��size����)
		*/
		std::vector<double*> m_InterferenceH;//�±��Ӧ��Pattern�£������ŵ���Ӧ����WT_Bģ����Ҫ
	};

	struct GTT_Urban {
		~GTT_Urban();
		int m_RoadId;
		int m_LocationId;
		double m_X;//��Ժ�����
		double m_Y;
		double m_AbsX;//���Ժ�����
		double m_AbsY;
		double m_V;//�ٶ�
		double m_VAngle;
		double m_FantennaAngle;
		IMTA *m_IMTA = nullptr;
	};

	struct GTT_HighSpeed {
		~GTT_HighSpeed();
		int m_RoadId;
		double m_X;//��Ժ�����
		double m_Y;
		double m_AbsX;//���Ժ�����
		double m_AbsY;
		double m_V;//�ٶ�
		double m_VAngle;
		double m_FantennaAngle;
		IMTA *m_IMTA = nullptr;
	};

	struct RRM {
		std::vector<int> m_InterferenceVeUENum;//�±��Ӧ��Pattern�£�ͬƵ��������
		std::vector<std::vector<int>> m_InterferenceVeUEIdVec;//�±��Ӧ��Pattern�£�ͬƵ���ų���ID����������ǰ������ÿ�����Ͷ�Ӧ����������m_InterferenceVeUENum[patternIdx]
		std::vector<std::vector<int>> m_PreInterferenceVeUEIdVec;//����ͬ�ϣ���һ�εĸ��ų����������ж��Ƿ���ͬ
		/*
		* �±�ΪPattern���
		* tuple��һ��Ԫ�أ����Ʒ�ʽ��Ӧ��2��ָ������QPSKΪ2��16QAMΪ4 64QAMλ6��WTģ����Ҫ
		* tuple�ڶ���Ԫ�أ����Ʒ�ʽ��Ӧ��ÿ���ŵı�����Ŀ(ʵ�����鲿)
		* tuple������Ԫ�أ���������
		*/
		std::vector<std::tuple<ModulationType,int,double>> m_WTInfo;
		std::vector<bool> m_isWTCached;//����Ƿ񻺴��WT������±�ΪpatternIdx,��Ҫ��λ�ø��º����

		bool isNeedRecalculateSINR(int patternIdx);
	};

	struct RRM_TDM_DRA {
		static std::default_random_engine s_Engine;

		VeUE* m_This;//RRM_TDM_DRA���õ�GTT����ز�������C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ
		
		std::tuple<int, int> m_ScheduleInterval;//��VeUE���ڴصĵ�ǰһ�ֵ�������

		RRM_TDM_DRA(VeUE* t_This) :m_This(t_This) {}
		//��Ա����
		int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

		std::string toString(int t_NumTab);//���ڴ�ӡVeUE��Ϣ
	};

	struct RRM_ICC_DRA {
		static std::default_random_engine s_Engine;

		VeUE* m_This;//RRM_ICC_DRA���õ�GTT����ز�������C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ

		RRM_ICC_DRA(VeUE* t_This) :m_This(t_This) {}

		int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);//���ѡ����Դ��

		std::string toString(int t_NumTab);//���ڴ�ӡVeUE��Ϣ
	};

	struct RRM_RR {

	};

	struct WT {

	};

	struct TMC {
		std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;//����λ�ø�����־��Ϣ
	};
};


inline
int VeUE::RRM_TDM_DRA::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}


inline
int VeUE::RRM_ICC_DRA::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}
