#pragma once
#include<string>
#include"Config.h"

class Road;
class GTT_Urban_Road;
class GTT_HighSpeed_Road;

class GTT_Road {
	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪRoad���ݽ�����ϵͳ��Road����
	*/
	Road* m_This = nullptr;
public:
	/*
	* ��·Id
	*/
	int m_RoadId;

	/*
	* ���Ժ�������������
	*/
	double m_AbsX;
	double m_AbsY;

	/*
	* <?>
	*/
	int  m_upr;

	/*------------------����------------------*/
public:
	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	virtual GTT_Urban_Road  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_Road  *const getHighSpeedPoint() = 0;

	/*
	* ȡ��ϵͳ��System��Road��ָ��
	*/
	Road* getSystemPoint() { return m_This; }

	/*
	* ����ϵͳ��System��Road��ָ��
	*/
	void setSystemPoint(Road* t_Point) { m_This = t_Point; }
};


class GTT_eNB;

class GTT_Urban_Road :public GTT_Road {
	/*------------------��------------------*/
public:
	/*
	* ��վ��Ŀ
	*/
	int m_eNBNum;

	/*
	* ��վId
	*/
	int m_eNBId;

	/*
	* ����ָ�������ڻ�վ�����ָ��
	*/
	GTT_eNB *m_eNB;

public:
	/*
	* ��ʼ������
	* ���ù��캯����ԭ���ǹ����ʱ���������û�������
	*/
	GTT_Urban_Road(UrbanRoadConfig &t_RoadConfig);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	GTT_Urban_Road  *const getUrbanPoint()override { return this; }
	GTT_HighSpeed_Road  *const getHighSpeedPoint()override { throw std::logic_error("RuntimeException"); }
};


class GTT_HighSpeed_Road :public GTT_Road {
public:
	/*
	* ���캯��
	*/
	GTT_HighSpeed_Road(HighSpeedRodeConfig &t_RoadHighSpeedConfig);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	GTT_Urban_Road  *const getUrbanPoint()override { throw std::logic_error("RuntimeException"); }
	GTT_HighSpeed_Road  *const getHighSpeedPoint()override { return this; }
};