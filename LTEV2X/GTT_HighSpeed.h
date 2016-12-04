#pragma once
#include<stdexcept>
#include"GTT.h"
#include"GTT_VeUE.h"

#include"Enumeration.h"

// <GTT_HighSpeed>: Geographical Topology and Transport HighSpeed

class GTT_HighSpeed :public GTT {
	/*------------------��̬------------------*/
public:
	/*
	* ��������棬���๲��
	*/
	static std::default_random_engine s_Engine;

	/*
	* ��վ����
	*/
	static const int s_eNB_NUM = 2;

	/*
	* ��·����
	*/
	static const int s_ROAD_NUM = 6;

	/*
	* RSU����
	*/
	static const int s_RSU_NUM = 35;

	/*
	* RSU�ڴ�����
	*/
	static const int s_RSU_CLUSTER_NUM = 2;

	/*
	* ·��
	*/
	static int s_ROAD_LENGTH;

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
	static const double s_ISD;

	/*
	* ???
	*/
	static const double s_ROAD_TOPO_RATIO[s_ROAD_NUM * 2];

	/*
	* ???
	*/
	static const double s_RSU_TOPO_RATIO[s_RSU_NUM * 2];

	/*
	* ???
	*/
	static const double s_eNB_TOPO[s_eNB_NUM * 2];

	/*
	* ���س��򳡾����ò���
	*/
	static void loadConfig(Platform t_Platform);
	/*------------------��------------------*/
private:
	/*
	* user per road array
	*/
	int* m_pupr;

	/*
	* ��������
	*/
	double m_Speed;//km/h

	/*
	* ͳ��ÿ��RSU�µĳ�����Ŀ
	* ����±����ڼ���λ�ø���(��0��ʼ)
	* �ڲ��±����RSU���
	* ����������ͳ�Ƴ����ֲ����
	*/
	std::vector<std::vector<int>> m_VeUENumPerRSU;

	/*------------------����------------------*/
public:
	/*
	* Ĭ�Ϲ��캯������Ϊɾ��
	*/
	GTT_HighSpeed() = delete;

	/*
	* ���캯��
	* ����ָ�붼���������ͣ���Ϊ��Ҫ��ʼ��ϵͳ�ĸ���ʵ������
	* �ù��캯��Ҳ�����˸�ģ�����ͼ
	*/
	GTT_HighSpeed(System* t_Context);

	/*
	* ģ���������
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
};