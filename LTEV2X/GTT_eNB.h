#pragma once
#include<vector>
#include<list>
#include<string>
#include"Config.h"

class eNB;
class GTT_Urban_eNB;
class GTT_HighSpeed_eNB;

class GTT_eNB {
	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪeNB���ݽ�����ϵͳ��eNB����
	*/
	eNB* m_This = nullptr;
public:
	/*
	* ���ڵ�·Id
	*/
	int m_RoadId;

	/*
	* ��վId
	*/
	int m_eNBId;

	/*
	* ��վ��Ժ�������
	*/
	double m_X, m_Y;

	/*
	* ��վ���Ժ�������
	*/
	double m_AbsX, m_AbsY;

	/*
	* �û�վ�е�RSU����(�洢RSU��Id)
	*/
	std::list<int> m_RSUIdList;

	/*
	* �û�վ�е�VeUE����(�洢VeUE��Id)
	*/
	std::list<int> m_VeUEIdList;


	/*------------------����------------------*/
public:

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ��ʼ������
	* ���ù��캯����ԭ���ǹ����ʱ���������û�������
	*/
	virtual void initialize(eNBConfig &t_eNBConfig) = 0;

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	virtual GTT_Urban_eNB  *const getUrbanPoint() = 0;
	virtual GTT_HighSpeed_eNB  *const getHighSpeedPoint() = 0;

	/*
	* ȡ��ϵͳ��System��eNB��ָ��
	*/
	eNB* getSystemPoint() { return m_This; }

	/*
	* ����ϵͳ��System��eNB��ָ��
	*/
	void setSystemPoint(eNB* t_Point) { m_This = t_Point; }
};


class GTT_Urban_eNB :public GTT_eNB {
public:
	/*
	* ��ʼ������
	* ���ù��캯����ԭ���ǹ����ʱ���������û�������
	*/
	void initialize(eNBConfig &t_eNBConfig)override;

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	GTT_Urban_eNB  *const getUrbanPoint()override { return this; }
	GTT_HighSpeed_eNB  *const getHighSpeedPoint()override { throw std::logic_error("RuntimeException"); }
};


class  GTT_HighSpeed_eNB :public GTT_eNB {
public:
	/*
	* ��ʼ������
	* ���ù��캯����ԭ���ǹ����ʱ���������û�������
	*/
	void initialize(eNBConfig &t_eNBConfig)override;

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	GTT_Urban_eNB  *const getUrbanPoint()override { throw std::logic_error("RuntimeException"); }
	GTT_HighSpeed_eNB  *const getHighSpeedPoint()override { return this; }
};
