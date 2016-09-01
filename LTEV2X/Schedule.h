#pragma once
#include<vector>
#include<list>
#include<utility>
#include<iostream>
#include<tuple>
#include "Enumeration.h"
#include"Global.h"

struct sScheduleInfo {
	int userId;//��ǰС���ڸ�RB�ϵ��ȵ����û���
	std::vector<int> assignedSubbandId;//���û���������Ӵ��ı��[0-gc_SubbandNum)
	sScheduleInfo() :userId(-1), assignedSubbandId(std::vector<int>(0)) {}
};


struct sFeedbackInfo {
public:
	std::vector<float> CQIWideband;//���CQI
	std::vector<float> CQISubband;//խ��CQI

	sFeedbackInfo& operator=(const sFeedbackInfo&f) {//������ֵ�����
		CQIWideband = f.CQIWideband;
		CQISubband = f.CQISubband;
		return *this;
	}
};





struct sPFInfo {//������PF���е����㷨����������
	int RSUId;
	int SubbandId;
	double FactorPF;
	sPFInfo(int RSUId, int SubbandId, double FactorPF) {
		this->RSUId = RSUId;
		this->SubbandId = SubbandId;
		this->FactorPF = FactorPF;
	}
	sPFInfo(const sPFInfo& t) {
		this->RSUId = t.RSUId;
		this->SubbandId = t.SubbandId;
		this->FactorPF = t.FactorPF;
	}
	sPFInfo& operator=(const sPFInfo& t) {
		this->RSUId = t.RSUId;
		this->SubbandId = t.SubbandId;
		this->FactorPF = t.FactorPF;
		return *this;
	}
};


struct sRRScheduleInfo {
	int eventId;//�¼����
	eMessageType messageType;//�¼�����
	int VeUEId;//�������
	int RSUId;//RSU���
	int patternIdx;//Ƶ�����
	std::tuple<int, int> occupiedInterval;//��ǰVeUE���д����ʵ��TTI���䣨�����䣩

	sRRScheduleInfo() {}
	sRRScheduleInfo(int eventId, eMessageType messageType, int VeUEId, int RSUId, int patternIdx,const std::tuple<int, int> &occupiedInterval) {
		this->eventId = eventId;
		this->messageType = messageType;
		this->VeUEId = VeUEId;
		this->RSUId = RSUId;
		this->patternIdx = patternIdx;
		this->occupiedInterval = occupiedInterval;
	}

	std::string toLogString(int n);

	/*
	* ���ɱ�ʾ������Ϣ��string����
	* �����¼���Id��������Id���Լ�Ҫ������¼���ռ�õ�TTI����
	*/
	std::string toScheduleString(int n);

};


