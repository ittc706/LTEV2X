#pragma once
#include<stdexcept>
#include"GTT.h"
#include"GTT_VeUE.h"

#include"Enumeration.h"

// <GTT_Urban>: Geographical Topology and Transport Urban

class GTT_Urban :public GTT {
	/*------------------��̬------------------*/
public:
	/*
	* ��������棬���๲��
	*/
	static std::default_random_engine s_Engine;

	/*
	* ��վ����
	*/
	static const int s_eNB_NUM = 7;

	/*
	* ��·����
	*/
	static const int s_ROAD_NUM = 14;

	/*
	* RSU����
	*/
	static const int s_RSU_NUM = 24;

	/*
	* �ϱ���·��
	*/
	static int s_ROAD_LENGTH_SN;

	/*
	* ������·��
	*/
	static int s_ROAD_LENGTH_EW;

	/*
	* ·��
	*/
	static double s_ROAD_WIDTH;

	/*
	* ����
	*/
	static double s_SPEED;

	/*
	* ???
	*/
	static const double s_ROAD_TOPO_RATIO[s_ROAD_NUM * 2];

	/*
	* ???
	*/
	static const int s_WRAP_AROUND_ROAD[s_ROAD_NUM][9];

	/*
	* ???
	*/
	static const int s_RSU_CLUSTER_NUM[s_RSU_NUM];

	/*
	* ???
	*/
	static const int s_RSU_ZONE_NUM[s_RSU_NUM];

	/*
	* ???
	*/
	static const double s_RSU_TOPO_RATIO[s_RSU_NUM * 2];

	/*
	* ???
	*/
	static const int s_RSU_IN_ROAD[s_ROAD_NUM][4];

	/*
	* �ڽ�RSU
	*/
	static const std::vector<int> s_ADJACENT_RSU[s_RSU_NUM];

	/*
	* ���س��򳡾����ò���
	*/
	static void loadConfig();
	/*------------------��------------------*/
private:
	/*
	* ueTopo x�� ����
	*/
	int m_xNum;

	/*
	* ueTopo y�� ����
	*/
	int m_yNum;

	/*
	* ��
	*/
	int m_ueTopoNum;

	/*
	* һ��Road��ue�������
	*/
	double *m_pueTopo;

	/*
	* user per road array
	*/
	int* m_pupr;

	/*
	* ��������
	*/
	double m_Speed;

public:
	/*
	* ͳ��ÿ��RSU�µĳ�����Ŀ
	* ����±����ڼ���λ�ø���(��0��ʼ)���ڲ��±����RSU���
	*/
	std::vector<std::vector<int>> m_VeUENumPerRSU;

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	GTT_Urban() = delete;

	/*
	* ���캯��
	* ����ָ�붼���������ͣ���Ϊ��Ҫ��ʼ��ϵͳ�ĸ���ʵ������
	* �ù��캯��Ҳ�����˸�ģ�����ͼ
	*/
	GTT_Urban(System* t_Context);

	/*
	* ��������
	*/
	void configure()override;

	/*
	* ������λ�ø���ʱ���������ĵ��������Ϣ
	*/
	void cleanWhenLocationUpdate()override;

	/*
	* ��ʼ������ʵ������
	*/
	void initialize()override;

	/*
	* �ŵ�����
	*/
	void channelGeneration()override;

	/*
	* �ŵ�ˢ��
	*/
	void freshLoc() override;

	/*
	* д�����λ�ø�����־
	*/
	void writeVeUELocationUpdateLogInfo() override;

	/*
	* ������ž���
	* ����Ĳ�������
	*		����±�Ϊ�������
	*		�ڲ��±�ΪPattern���
	*		���ڲ�listΪ�ó����ڸ�Pattern�µĸ����б�
	* Ŀǰ���дؼ���ţ���ΪRSU�����̫С���������Ժ���
	*/
	void calculateInterference(const std::vector<std::vector<std::list<int>>>& t_RRMInterferenceVec) override;

	/*
	* ���ظ���RSU���ڽ�RSU�б�
	*/
	const std::vector<int>& getAdjacentRSUs(int t_RSUId);
};