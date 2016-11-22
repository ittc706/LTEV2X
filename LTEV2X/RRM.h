#pragma once
#include"VUE.h"
#include"RSU.h"

//<RRM>: Radio Resource Management

class RRM_TDM_DRA_VeUE;
class RRM_ICC_DRA_VeUE;
class RRM_RR_VeUE;

class RRM_VeUE {
	/*------------------��------------------*/
public:
	VeUE* m_This;

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

	/*------------------����------------------*/
public:
	virtual void initialize() = 0;

	virtual RRM_TDM_DRA_VeUE *const getTDM_DRAPoint() = 0;
	virtual RRM_ICC_DRA_VeUE *const getICC_DRAPoint() = 0;
	virtual RRM_RR_VeUE *const getRRPoint() = 0;

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

class RRM_Basic {
	/*------------------��------------------*/
public:
	/*
	* ��ǰ��TTIʱ��,ָ��ϵͳ�ĸò���
	*/
	int& m_TTI;

	/*
	* ϵͳ���ò���,ָ��ϵͳ�ĸò���
	*/
	SystemConfig& m_Config;

	/*
	* RSU����,ָ��ϵͳ�ĸò���
	*/
	RSU* m_RSUAry;

	/*
	* VeUE����,ָ��ϵͳ�ĸò���
	*/
	RRM_VeUE* m_VeUEAry;

	/*
	* �¼�����,ָ��ϵͳ�ĸò���
	*/
	std::vector<Event>& m_EventVec;

	/*
	* �¼���������,ָ��ϵͳ�ĸò���
	*/
	std::vector<std::list<int>>& m_EventTTIList;

	/*
	* �����ʣ�����±�ΪTTI���ڲ��±�ΪRSUId,ָ��ϵͳ�ĸò���
	*/
	std::vector<std::vector<int>>& m_TTIRSUThroughput;

													  
	/*
	* ����ģ�����ʱ���¼
	*/
	long double m_GTTTimeConsume = 0;
	long double m_WTTimeConsume = 0;

	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	RRM_Basic() = delete;

	/*
	* ���캯��
	* �ù��캯�������˸�ģ�����ͼ
	* ����ָ���Ա����ϵͳ���еĶ�Ӧ��Աָ�룬����ͬһʵ��
	*/
	RRM_Basic(int &t_TTI, SystemConfig& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, std::vector<Event>& t_EventVec, std::vector<std::list<int>>& t_EventTTIList, std::vector<std::vector<int>>& t_TTIRSUThroughput) :
		m_TTI(t_TTI),
		m_Config(t_Config),
		m_RSUAry(t_RSUAry),
		m_EventVec(t_EventVec),
		m_EventTTIList(t_EventTTIList),
		m_TTIRSUThroughput(t_TTIRSUThroughput) {}


	/*
	* ��ʼ��RSU VeUE�ڸõ�Ԫ���ڲ���
	*/
	virtual void initialize() = 0;

	/*
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* �����ܿغ���
	*/
	virtual void schedule() = 0;
};


