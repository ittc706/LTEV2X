#pragma once
#include<vector>
#include<list>
#include<string>

class RSU;
class GTT_Urban_RSU;
class GTT_HighSpeed_RSU;
class IMTA;

class GTT_RSU {
	/*------------------��̬------------------*/
public:
	/*
	* ��������
	*/
	static int s_RSUCount;
	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪRSU���ݽ�����ϵͳ��RSU����
	*/
	RSU* m_This = nullptr;

public:
	/*
	* RSUId
	*/
	int m_RSUId = s_RSUCount++;

	/*
	* ���Ժ����꣬������
	*/
	double m_AbsX;
	double m_AbsY;

	/*
	* <?>
	*/
	IMTA *m_IMTA = nullptr;

	/*
	* <?>
	*/
	double m_FantennaAngle;

	/*
	* ��ǰRSU��Χ�ڵ�VeUEId�������,RRMģ����Ҫ
	*/
	std::list<int> m_VeUEIdList;

	/*
	* һ��RSU���Ƿ�Χ�ڵĴصĸ���,RRMģ����Ҫ
	*/
	int m_ClusterNum;

	/*
	* ���ÿ���ص�VeUE��Id������,�±����صı��
	*/
	std::vector<std::list<int>> m_ClusterVeUEIdList;

	/*
	* һ��RSU���Ƿ�Χ�ڵ�ӵ������ĸ���
	*/
	int m_ZoneNum;

	/*
	* ���ÿ���ص�ӵ������Zone��Id������,�±����Zone�ı��
	*/
	std::vector<std::list<int>> m_ZoneVeUEIdList;

	/*------------------����------------------*/
	/*
	* �����������ͷ�ָ��
	*/
	~GTT_RSU();

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	virtual GTT_Urban_RSU  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_RSU  *const getHighSpeedPoint() = 0;

	/*
	* ȡ��ϵͳ��System��RSU��ָ��
	*/
	RSU* getSystemPoint() { return m_This; }

	/*
	* ����ϵͳ��System��RSU��ָ��
	*/
	void setSystemPoint(RSU* t_Point) { m_This = t_Point; }
};


class GTT_Urban_RSU :public GTT_RSU {
public:
	/*
	* ���캯��
	*/
	GTT_Urban_RSU();

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	GTT_Urban_RSU  *const getUrbanPoint()override { return this; }
	GTT_HighSpeed_RSU  *const getHighSpeedPoint()override { throw std::logic_error("RuntimeException"); }
};



class  GTT_HighSpeed_RSU :public GTT_RSU {
public:
	/*
	* ���캯��
	*/
	GTT_HighSpeed_RSU();

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	GTT_Urban_RSU  *const getUrbanPoint()override { throw std::logic_error("RuntimeException"); }
	GTT_HighSpeed_RSU  *const getHighSpeedPoint()override { return this; }
};
