#include<iomanip>
#include<cmath>
#include"Event.h"
#include"Enumeration.h"
#include"Global.h"
#include"System.h"

using namespace std;

int Event::s_EventCount = 0;

default_random_engine Event::s_Engine(0);

Event::Event(int t_VeUEId, int t_TTI, MessageType t_MessageType) :
	m_EventId(s_EventCount++),
	m_VeUEId(t_VeUEId),
	m_TriggerTTI(t_TTI),
	m_MessageType(t_MessageType),
	m_PackageNum(gc_MessagePackageNum[t_MessageType]),
	m_TransimitPackageNum(0),
	m_PackageLossNum(0),
	m_BitNumPerPackage(gc_MessageBitNumPerPackage[t_MessageType]),
	m_IsFinished(false),
	m_CurrentPackageIdx(0),
	m_InitialWindowSize(gc_InitialWindowSize[t_MessageType]),
	m_MaxWindowSize(gc_MaxWindowSize[t_MessageType]),
	m_WithdrawalTime(0),
	m_ConflictNum(0),
	m_SendDelay(0),
	m_QueueDelay(0),
	m_ProcessDelay(0) {
	m_RemainBitNum = m_BitNumPerPackage[0];
	m_PackageIsLoss.assign(m_PackageNum, false);
	m_CurWindowSize = m_InitialWindowSize;
}


void Event::reset() {
	if (m_IsFinished)throw Exp("error");
	m_CurrentPackageIdx = 0;
	m_WithdrawalTime = 0;
	m_RemainBitNum = m_BitNumPerPackage[0];
	m_PackageIsLoss.assign(m_PackageNum, false);
	m_CurWindowSize = m_InitialWindowSize;
}


bool Event::tryAcccess() {
	if (m_WithdrawalTime == 0) return true;
	--m_WithdrawalTime;
	return false;
}

int Event::transimit(int t_TransimitMaxBitNum) {
	if (t_TransimitMaxBitNum >= m_RemainBitNum) {//当前package传输完毕
		++m_TransimitPackageNum;
		int temp = m_RemainBitNum;
		if (++m_CurrentPackageIdx == m_PackageNum) {//若当前package是最后一个package，那么说明传输成功
			m_RemainBitNum = 0;
			m_IsFinished = true;
		}
		else
			m_RemainBitNum = m_BitNumPerPackage[m_CurrentPackageIdx];
		return temp;
	}
	else {//当前package尚未传输完毕，只需更新remainBitNum
		m_RemainBitNum -= t_TransimitMaxBitNum;
		return t_TransimitMaxBitNum;
	}
}

void Event::conflict() {
	uniform_int_distribution<int> u(1, m_CurWindowSize);
	m_CurWindowSize = min(m_MaxWindowSize, m_CurWindowSize * 2);
	m_WithdrawalTime = u(s_Engine);
	m_ConflictNum++;
}

void Event::packetLoss(double t_Distance) {
	if (!m_PackageIsLoss[m_CurrentPackageIdx]) {
		m_PackageIsLoss[m_CurrentPackageIdx] = true;
		m_PackageLossDistance.push_back(t_Distance);
		++m_PackageLossNum;
	}
}

string Event::toString() {
	string s;
	switch (m_MessageType) {
	case PERIOD:
		s = "PERIOD";
		break;
	case EMERGENCY:
		s = "EMERGENCY";
		break;
	case DATA:
		s = "DATA";
		break;
	}
	ostringstream ss;
	ss << "[ byteNum = { ";
	for (int bitNum : m_BitNumPerPackage)
		ss << left << setw(3) << bitNum << ", ";
	ss << "} , MessageType = " << s << " ]";
	string messageString = ss.str();

	ss.str();
	ss << "{ EventId = " << left << setw(3) << m_EventId;
	ss << " , VeUEId = " << left << setw(3) << m_VeUEId;
	ss << "] ， Message = " << messageString << " }";
	return ss.str();
}


string Event::toLogString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	for (string log : m_LogTrackList)
		ss << indent << log << endl;
	return ss.str();
}


void Event::addEventLog(int t_TTI, EventLogType t_EventLogType, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, string t_Description) {
	stringstream ss;
	switch (t_EventLogType) {
	case TRANSIMITTING:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - Transimit At: RSU[" << t_FromRSUId << "] - Cluster[" << t_FromClusterIdx << "] - Pattern[" << t_FromPatternIdx << "] }";
		break;
	case WITHDRAWING:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - Withdraw At: RSU[" << t_FromRSUId << "] - Cluster[" << t_FromClusterIdx << "] }";
		break;
	case SUCCEED:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - Transimit Succeed At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "] }";
		break;
	case EVENT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: EventList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: SwitchList }";
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case WAIT_TO_SWITCH:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: SwitchList }";
		break;
	case WAIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case WAIT_TO_ACCESS:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s AccessEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case ACCESS_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s AccessEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case SWITCH_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: SwitchList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	case TRANSIMIT_TO_WAIT:
		ss << "{ TTI: " << left << setw(3) << t_TTI << " - Description : <" << left << setw(10) << t_Description + ">" << " - From: RSU[" << t_FromRSUId << "]'s TransmitScheduleInfoList[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "] }";
		break;
	}
	m_LogTrackList.push_back(ss.str());
}









