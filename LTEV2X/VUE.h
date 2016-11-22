#pragma once
#include<vector>
#include<list>
#include<string>
#include<utility>
#include<tuple>
#include<random>
#include<map>
#include"Global.h"
#include"Enumeration.h"
#include"Config.h"
#include"IMTA.h"

/*
* ǰ������
*/
class GTT_VeUE;
class RRM_VeUE;
class WT_VeUE;
class TMC_VeUE;

/*
* ÿһ����Ԫ���иõ�Ԫ��Ӧ��VeUE��ͼ
* ��������þ����ṩ��ͬ��Ԫ֮��VeUEʵ�����Ϣ����
*/

class VeUE {
	/*------------------��------------------*/
public:
	GTT_VeUE * m_GTT = nullptr;
	RRM_VeUE * m_RRM = nullptr;
	WT_VeUE * m_WT = nullptr;
	TMC_VeUE * m_TMC = nullptr;
};






