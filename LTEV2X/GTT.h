#pragma once
#include<random>
#include<set>
#include<tuple>
#include"RSU.h"
#include"VUE.h"
#include"eNB.h"
#include"Road.h"
#include"Config.h"

// <GTT>: Geographical Topology and Transport

class GTT_Urban_VeUE;
class GTT_HighSpeed_VeUE;

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
	VeUE* m_This=nullptr;

public:
	/*
	* ����ID
	*/
	const int m_VeUEId = s_VeUECount++;

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
	double* m_H = nullptr;

	/*
	* ����������RSU֮��ľ���
	*/
	double* m_Distance = nullptr;

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


class GTT {
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
	* ��վ����,ָ��ϵͳ�ĸò���
	* ����Ϊʲô�������������ͣ���Ϊϵͳ����Щ����ָ����뿿��ģ������ʼ������˲��ܴ��뿽��
	*/
	eNB* &m_eNBAry;

	/*
	* ��·����,ָ��ϵͳ�ĸò���
	*/
	Road* &m_RoadAry;

	/*
	* RSU����,ָ��ϵͳ�ĸò���
	*/
	RSU* &m_RSUAry;

	/*
	* VeUE����
	* ��һά�ȵ�ָ��ָ�����飬��������ָ��GTT_VeUEʵ���ָ��
	* Ϊʲô��������ָ�룬��Ϊ��Ҫʵ�ֶ�̬
	*/
	GTT_VeUE** m_VeUEAry;

	/*------------------�ӿ�------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	GTT() = delete;

	/*
	* ���캯��
	* ����ָ�붼���������ͣ���Ϊ��Ҫ��ʼ��ϵͳ�ĸ���ʵ������
	* �ù��캯��Ҳ�����˸�ģ�����ͼ
	*/
	GTT(int &t_TTI, SystemConfig& t_Config, eNB* &t_eNBAry, Road* &t_RoadAry, RSU* &t_RSUAry) :
		m_TTI(t_TTI), m_Config(t_Config), m_eNBAry(t_eNBAry), m_RoadAry(t_RoadAry), m_RSUAry(t_RSUAry) {}

	/*
	* ��������
	*/
	~GTT();

	/*
	* ģ���������
	*/
	virtual void configure() = 0;

	/*
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	virtual void cleanWhenLocationUpdate() = 0;

	/*
	* ��ʼ������ʵ������
	*/
	virtual void initialize() = 0;

	/*
	* �ŵ�����
	*/
	virtual void channelGeneration() = 0;

	/*
	* �ŵ�ˢ��
	*/
	virtual void freshLoc() = 0;

	/*
	* д�����λ�ø�����־
	*/
	virtual void writeVeUELocationUpdateLogInfo(std::ofstream &t_File1, std::ofstream &t_File2) = 0;

	/*
	* ������ž���
	* ����Ĳ�������
	*		����±�Ϊ�������
	*		�ڲ��±�ΪPattern���
	*		���ڲ�listΪ�ó����ڸ�Pattern�µĸ����б�
	* Ŀǰ���дؼ���ţ���ΪRSU�����̫С���������Ժ���
	*/
	virtual void calculateInterference(const std::vector<std::vector<std::list<int>>>& t_RRMInterferenceVec) = 0;
};
