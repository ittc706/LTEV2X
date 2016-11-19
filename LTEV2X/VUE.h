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
	/*------------------��̬------------------*/
public:
	/*
	* ��������
	*/
	static int m_VeUECount;

	/*---------------ǰ������---------------*/
public:
	class GTT;
	class GTT_Urban;
	class GTT_HighSpeed;
	class RRM;
	class RRM_TDM_DRA;
	class RRM_ICC_DRA;
	class RRM_RR;
	class WT;
	class TMC;

	/*------------------��------------------*/
public:
	/*
	* ����ID
	*/
	const int m_VeUEId = m_VeUECount++;

	/*
	* GTT�ڲ���ָ��
	* ���ڴ洢�������������Ĳ���
	*/
	GTT* m_GTT = nullptr;

	/*
	* GTT_Urban�ڲ���ָ��
	* ���ڴ洢���򳡾����ض�����
	*/
	GTT_Urban* m_GTT_Urban = nullptr;

	/*
	* GTT_HighSpeed�ڲ���ָ��
	* ���ڴ洢���ٳ������ض�����
	*/
	GTT_HighSpeed* m_GTT_HighSpeed = nullptr;

	/*
	* RRM�ڲ���ָ��
	* ���ڴ洢��ͬʵ�ֵĹ�ͬ����
	*/
	RRM* m_RRM = nullptr;

	/*
	* RRM_TDM_DRA�ڲ���ָ��
	* ���ڴ洢RRM_TDM_DRAģʽ���ض�����
	*/
	RRM_TDM_DRA* m_RRM_TDM_DRA = nullptr;

	/*
	* RRM_ICC_DRA�ڲ���ָ��
	* ���ڴ洢RRM_ICC_DRAģʽ���ض�����
	*/
	RRM_ICC_DRA* m_RRM_ICC_DRA = nullptr;

	/*
	* RRM_RR�ڲ���ָ��
	* ���ڴ洢RRģʽ���ض�����
	*/
	RRM_RR* m_RRM_RR = nullptr;

	/*
	* WT�ڲ���ָ��
	* ���ڴ洢��ͬʵ�ֵĹ�ͬ����
	*/
	WT* m_WT = nullptr;

	/*
	* TMC�ڲ���ָ��
	* ���ڴ洢��ͬʵ�ֵĹ�ͬ����
	*/
	TMC* m_TMC = nullptr;

	/*------------------����------------------*/
public:
	/*
	* ��������
	* �����ڲ���ָ��
	*/
	~VeUE();
	
	/*
	* ���򳡾��£�RSU�����ʼ������
	* ��GTTģ���ʼ��ʱ����
	*/
	void initializeGTT_Urban(VeUEConfig &t_VeUEConfig);

	/*
	* ���ٳ����£�RSU�����ʼ������
	* ��GTTģ���ʼ��ʱ����
	*/
	void initializeGTT_HighSpeed(VeUEConfig &t_VeUEConfig);

	/*
	* TDM_DRAģʽ�£�RSU�����ʼ������
	* ��RRMģ���ʼ��ʱ����
	*/
	void initializeRRM_TDM_DRA();

	/*
	* ICC_DRAģʽ�£�RSU�����ʼ������
	* ��RRMģ���ʼ��ʱ����
	*/
	void initializeRRM_ICC_DRA();

	/*
	* RRģʽ�£�RSU�����ʼ������
	* ��RRMģ���ʼ��ʱ����
	*/
	void initializeRRM_RR();

	/*
	* WTģ����ͼ�У�RSU�����ʼ������
	* ��WTģ���ʼ��ʱ����
	*/
	void initializeWT();

	/*
	* TMCģ����ͼ�У�RSU�����ʼ������
	* ��WTģ���ʼ��ʱ����
	*/
	void initializeTMC();

	/*-----------------�ڲ���-----------------*/
public:
	class GTT {
		/*--------��--------*/
	public:
		/*
		* ���ڵ�·��RoadId
		*/
		int m_RoadId;

		/*
		* ��Ժ����꣬������
		*/
		double m_X;
		double m_Y;

		/*
		* ���Ժ����꣬������
		*/
		double m_AbsX;
		double m_AbsY;

		/*
		* �����ٶ�
		*/
		double m_V;

		/*
		* <?>
		*/
		double m_VAngle;

		/*
		* <?>
		*/
		double m_FantennaAngle;

		/*
		* <?>
		*/
		IMTA *m_IMTA = nullptr;

		/*
		* �������ڵ�RSUId
		*/
		int m_RSUId;

		/*
		* �������ڴر��
		*/
		int m_ClusterIdx;

		/*
		* ����������Ŀ
		*/
		int m_Nt;

		/*
		* ����������Ŀ
		*/
		int m_Nr;

		/*
		* ·�����
		*/
		double m_Ploss;

		/*
		* �ŵ���Ӧ����
		*/
		double* m_H;

		/*
		* ����������RSU֮��ľ���
		*/
		double* m_Distance;

		/*
		* �����������Ե�ǰ�����ĸ���·����ģ�WT_Bģ����Ҫ
		* �±꣺VeUEId(����һ��ʼ�Ϳ��ٺ����г����Ĳ�λ���ò��size����)
		*/
		std::vector<double> m_InterferencePloss;

		/*
		* �����������Ե�ǰ�������ŵ���Ӧ����WT_Bģ����Ҫ
		* �±꣺���ų�����VeUEId��VeUEId(����һ��ʼ�Ϳ��ٺ����г����Ĳ�λ���ò��size����)
		*/
		std::vector<double*> m_InterferenceH;

		/*-------����-------*/
	public:
		/*
		* �����������ͷ�ָ��
		*/
		~GTT();
	};

	class GTT_Urban {
		/*--------��--------*/
	public:
		/*
		* <?>
		*/
		int m_LocationId;
	};

	class GTT_HighSpeed {
	public:

	};

	class RRM {
		/*--------��--------*/
	public:
		/*
		* �±��Ӧ��Pattern�£�ͬƵ��������
		*/
		std::vector<int> m_InterferenceVeUENum;

		/*
		* �±��Ӧ��Pattern�£�ͬƵ���ų���ID
		* ��������ǰ����
		*/
		std::vector<std::vector<int>> m_InterferenceVeUEIdVec;

		/*
		* ����ͬ�ϣ���һ�εĸ��ų���
		* �����ж��Ƿ���ͬ���Ӷ������Ƿ���Ҫ����һ���ŵ��Լ��ظɱ�
		*/
		std::vector<std::vector<int>> m_PreInterferenceVeUEIdVec;
		
		/*
		* ���Ʒ�ʽ
		*/
		const ModulationType m_ModulationType = gc_ModulationType;

		/*
		* �ŵ���������
		*/
		const double m_CodeRate = gc_CodeRate;

		/*
		* �ϴμ�����ظɱ�
		* ��Ϊ��Сֵ((std::numeric_limits<double>::min)())��˵��֮ǰû�м����
		*/
		std::vector<double> m_PreSINR;

		/*-------����-------*/
	public:
		/*
		* �ж��Ƿ���Ҫ���¼���SINR
		* ȡ���ڸ����б��Ƿ���ͬ
		*/
		bool isNeedRecalculateSINR(int t_PatternIdx);

		/*
		* �ж�֮ǰ�Ƿ��Ѿ����SINR
		*/
		bool isAlreadyCalculateSINR(int t_PatternIdx) { return m_PreSINR[t_PatternIdx] != (std::numeric_limits<double>::min)(); }
	};

	class RRM_TDM_DRA {
		/*-------��̬-------*/
	public:
		static std::default_random_engine s_Engine;
		/*--------��--------*/
	public:
		/*
		* RRM_TDM_DRA���õ�GTT����ز���
		* ��C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ
		*/
		VeUE* m_This;

		/*
		* ��VeUE���ڴصĵ�ǰ����λ�õ���Ե�������
		*/
		std::tuple<int, int> m_ScheduleInterval;

		/*-------����-------*/
	public:
		/*
		* Ĭ�Ϲ��캯������Ϊɾ��
		*/
		RRM_TDM_DRA() = delete;

		/*
		* ���캯��
		*/
		RRM_TDM_DRA(VeUE* t_This) :m_This(t_This) {}
		
		/*
		* ���ѡȡ������Դ��
		*/
		int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

		/*
		* ���ɸ�ʽ���ַ���
		*/
		std::string toString(int t_NumTab);
	};

	class RRM_ICC_DRA {
		/*-------��̬-------*/
	public:
		static std::default_random_engine s_Engine;
		/*--------��--------*/
	public:
		/*
		* RRM_ICC_DRA���õ�GTT����ز���
		* ��C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ
		*/
		VeUE* m_This;

		/*-------����-------*/
	public:
		/*
		* Ĭ�Ϲ��캯������Ϊɾ��
		*/
		RRM_ICC_DRA() = delete;

		/*
		* ���캯��
		*/
		RRM_ICC_DRA(VeUE* t_This) :m_This(t_This) {}

		/*
		* ���ѡ����Դ��
		*/
		int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

		/*
		* ���ɸ�ʽ���ַ���
		*/
		std::string toString(int t_NumTab);
	};

	class RRM_RR {
		/*--------��--------*/
	public:
		/*
		* RRM_RR���õ�GTT����ز���
		* ��C++�ڲ����Ǿ�̬�ģ���˴���һ����Χ��ʵ�������ã�������ϵ
		*/
		VeUE* m_This;

		/*-------����-------*/
	public:
		/*
		* Ĭ�Ϲ��캯������Ϊɾ��
		*/
		RRM_RR() = delete;

		/*
		* ���캯��
		*/
		RRM_RR(VeUE* t_This) :m_This(t_This) {}

		/*
		* ���ɸ�ʽ���ַ���
		*/
		std::string toString(int t_NumTab);
	};

	class WT {
	public:
	};

	class TMC {
		/*--------��--------*/
	public:
		/*
		* ����λ�ø�����־��Ϣ
		*/
		std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;
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
