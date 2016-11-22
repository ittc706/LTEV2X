#pragma once

#include<vector>
#include<list>
#include<string>
#include<fstream>
#include<tuple>
#include"Global.h"
#include"VUE.h"
#include"Event.h"
#include"IMTA.h"
#include"Exception.h"

/*
* ǰ������
*/
class GTT_RSU;
class RRM_RSU;
class WT_RSU;
class TMC_RSU;

/*
* ÿһ����Ԫ���иõ�Ԫ��Ӧ��RSU��ͼ
* ��������þ����ṩ��ͬ��Ԫ֮��RSUʵ�����Ϣ����
*/

class RSU {
	friend class System;
	/*------------------��------------------*/
private:
	/*
	* ָ��GTT��ͼ�µ�VeUEʵ�����
	*/
	GTT_RSU * m_GTT = nullptr;

	/*
	* ָ��RRM��ͼ�µ�VeUEʵ�����
	*/
	RRM_RSU * m_RRM = nullptr;

	/*
	* ָ��WT��ͼ�µ�VeUEʵ�����
	*/
	WT_RSU * m_WT = nullptr;

	/*
	* ָ��TMC��ͼ�µ�VeUEʵ�����
	*/
	TMC_RSU * m_TMC = nullptr;

	/*------------------����------------------*/
public:
	/*
	* ָ��GTT��ͼ�µ�VeUE��ָ��
	*/
	GTT_RSU* getGTTPoint() { return m_GTT; }

	/*
	* ָ��RRM��ͼ�µ�VeUE��ָ��
	*/
	RRM_RSU* getRRMPoint() { return m_RRM; }

	/*
	* ָ��WT��ͼ�µ�VeUE��ָ��
	*/
	WT_RSU* getWTPoint() { return m_WT; }

	/*
	* ָ��TMC��ͼ�µ�VeUE��ָ��
	*/
	TMC_RSU* getTMCPoint() { return m_TMC; }
};




