#pragma once
#include<random>
#include<vector>

class VeUE;

class TMC_VeUE {
	/*------------------��̬------------------*/
public:
	/*
	* ��������棬���๲��
	*/
	static std::default_random_engine s_Engine;
	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪVeUE���ݽ�����ϵͳ��VeUE����
	*/
	VeUE* m_This;

public:
	/*
	* Ĭ�Ϲ��캯��
	*/
	TMC_VeUE();

	/*
	* ��Ӧӵ���ȼ��£������´������¼�������ʱ��
	*/
	std::vector<int> m_NextPeriodEventTriggerTTI;

	/*------------------����------------------*/
public:
	/*
	* ȡ��ϵͳ��System��VeUE��ָ��
	*/
	VeUE* getSystemPoint() { return m_This; }

	/*
	* ����ϵͳ��System��VeUE��ָ��
	*/
	void setSystemPoint(VeUE* t_Point) { m_This = t_Point; }
};
