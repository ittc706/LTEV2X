#pragma once
#include<list>
#include<vector>
#include<tuple>

class VeUE;
class GTT_Urban_VeUE;
class GTT_HighSpeed_VeUE;
class IMTA;

class GTT_VeUE {
	/*------------------��̬------------------*/
public:
	/*
	* ��������
	*/
	static int s_VeUECount;
	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪVeUE���ݽ�����ϵͳ��VeUE����
	*/
	VeUE* m_This = nullptr;

public:
	/*
	* ����ID
	*/
	const int m_VeUEId = s_VeUECount++;

	/*
	* ���ڵ�·��RoadId
	*/
	int m_RoadId = -1;

	/*
	* ��Ժ����꣬������
	*/
	double m_X = -1;
	double m_Y = -1;

	/*
	* ���Ժ����꣬������
	*/
	double m_AbsX = -1;
	double m_AbsY = -1;

	/*
	* �����ٶ�
	*/
	double m_V = -1;

	/*
	* <?>
	*/
	double m_VAngle = -1;

	/*
	* <?>
	*/
	double m_FantennaAngle = -1;

	/*
	* <?>
	*/
	IMTA *m_IMTA = nullptr;

	/*
	* �������ڵ�RSUId
	*/
	int m_RSUId = -1;

	/*
	* �������ڴر��
	*/
	int m_ClusterIdx = -1;

	/*
	* ��������ӵ��������
	*/
	int m_ZoneIdx = -1;

	/*
	* ӵ���ȼ�
	*/
	int m_CongestionLevel = 0;

	/*
	* ����������Ŀ
	*/
	int m_Nt = -1;

	/*
	* ����������Ŀ
	*/
	int m_Nr = -1;

	/*
	* ·�����
	*/
	double m_Ploss = -1;

	/*
	* �ŵ���Ӧ����
	*/
	double* m_H = nullptr;

	/*
	* ����������RSU֮��ľ���
	*/
	double* m_Distance = nullptr;

	/*
	* �����������Ե�ǰ�����ĸ���·����ģ�WTģ����Ҫ
	* �±꣺VeUEId(����һ��ʼ�Ϳ��ٺ����г����Ĳ�λ���ò��size����)
	*/
	std::vector<double> m_InterferencePloss;

	/*
	* �����������Ե�ǰ�������ŵ���Ӧ����WTģ����Ҫ
	* �±꣺���ų�����VeUEId��VeUEId(����һ��ʼ�Ϳ��ٺ����г����Ĳ�λ���ò��size����)
	*/
	std::vector<double*> m_InterferenceH;

	/*
	* ����λ�ø�����־��Ϣ
	*/
	std::list<std::tuple<int, int>> m_LocationUpdateLogInfoList;

	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	GTT_VeUE() {}

	/*
	* �����������ͷ�ָ��
	*/
	~GTT_VeUE();

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	virtual GTT_Urban_VeUE  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_VeUE  *const getHighSpeedPoint() = 0;

	/*
	* ȡ��ϵͳ��System��VeUE��ָ��
	*/
	VeUE* getSystemPoint() { return m_This; }

	/*
	* ����ϵͳ��System��VeUE��ָ��
	*/
	void setSystemPoint(VeUE* t_Point) { m_This = t_Point; }
};


//VeUE���ò���
struct VeUEConfig {
	int roadId;
	double X;
	double Y;
	double AbsX;
	double AbsY;
	double V;
	int VeUENum;
	double angle;
};


class GTT_Urban_VeUE :public GTT_VeUE {
	/*------------------��------------------*/
public:
	/*
	* <?>
	*/
	int m_LocationId;
	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	GTT_Urban_VeUE() = delete;
	GTT_Urban_VeUE(VeUEConfig &t_VeUEConfig);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	GTT_Urban_VeUE  *const getUrbanPoint()override { return this; }
	GTT_HighSpeed_VeUE  *const getHighSpeedPoint()override { throw std::logic_error("RuntimeException"); }
};



class GTT_HighSpeed_VeUE :public GTT_VeUE {
	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	GTT_HighSpeed_VeUE() = delete;
	GTT_HighSpeed_VeUE(VeUEConfig &t_VeUEConfig);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	GTT_Urban_VeUE  *const getUrbanPoint()override { throw std::logic_error("RuntimeException"); }
	GTT_HighSpeed_VeUE  *const getHighSpeedPoint()override { return this; }
};