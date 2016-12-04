#pragma once
#include<vector>
#include<list>
#include<random>
#include<tuple>
#include"Enumeration.h"

class VeUE;
class RRM_TDM_DRA_VeUE;
class RRM_ICC_DRA_VeUE;
class RRM_RR_VeUE;

class RRM_VeUE {
	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪVeUE���ݽ�����ϵͳ��VeUE����
	*/
	VeUE* m_This;

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
	const ModulationType m_ModulationType;

	/*
	* �ŵ���������
	*/
	const double m_CodeRate;

	/*
	* �ϴμ�����ظɱ�
	* ��Ϊ��Сֵ((std::numeric_limits<double>::min)())��˵��֮ǰû�м����
	*/
	std::vector<double> m_PreSINR;

	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	RRM_VeUE() = delete;
	RRM_VeUE(int t_TotalPatternNum);

	/*
	* ��������
	* û����Ҫ�������Դ
	*/
	~RRM_VeUE() {}

	/*
	* �ж��Ƿ���Ҫ���¼���SINR
	* ȡ���ڸ����б��Ƿ���ͬ
	*/
	bool isNeedRecalculateSINR(int t_PatternIdx);

	/*
	* �ж�֮ǰ�Ƿ��Ѿ����SINR
	*/
	bool isAlreadyCalculateSINR(int t_PatternIdx) { return m_PreSINR[t_PatternIdx] != (std::numeric_limits<double>::min)(); }

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	* ���ھ�̬����ΪRRM_VeUE
	*/
	virtual RRM_TDM_DRA_VeUE *const getTDM_DRAPoint() = 0;
	virtual RRM_ICC_DRA_VeUE *const getICC_DRAPoint() = 0;
	virtual RRM_RR_VeUE *const getRRPoint() = 0;

	/*
	* ȡ��ϵͳ��System��VeUE��ָ��
	*/
	VeUE* getSystemPoint();

	/*
	* ����ϵͳ��System��VeUE��ָ��
	*/
	void setSystemPoint(VeUE* t_Point);
};


class RRM_ICC_DRA_VeUE :public RRM_VeUE {
	/*------------------��̬------------------*/
public:
	static std::default_random_engine s_Engine;

	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	RRM_ICC_DRA_VeUE();

	/*
	* ���ѡ����Դ��
	*/
	int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { return this; }
	RRM_RR_VeUE *const getRRPoint()override { throw std::logic_error("RuntimeException"); }
};


class RRM_RR_VeUE :public RRM_VeUE {
	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	RRM_RR_VeUE();

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_RR_VeUE *const getRRPoint()override { return this; }
};


class RRM_TDM_DRA_VeUE :public RRM_VeUE {
	/*------------------��̬------------------*/
public:
	static std::default_random_engine s_Engine;

	/*------------------��------------------*/
public:
	/*
	* ��VeUE���ڴصĵ�ǰ����λ�õ���Ե�������
	*/
	std::tuple<int, int> m_ScheduleInterval;

	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	RRM_TDM_DRA_VeUE();

	/*
	* ���ѡȡ������Դ��
	*/
	int selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx);

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	RRM_TDM_DRA_VeUE *const getTDM_DRAPoint()override { return this; }
	RRM_ICC_DRA_VeUE *const getICC_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_RR_VeUE *const getRRPoint()override { throw std::logic_error("RuntimeException"); }
};
