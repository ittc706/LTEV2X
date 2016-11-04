/*
* =====================================================================================
*
*       Filename:  RRM_ICC_DRA.cpp
*
*    Description:  RRM_ICC_DRA模块
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/


#include"RRM_ICC_DRA.h"

using namespace std;

RRM_ICC_DRA::RRM_ICC_DRA(int &t_TTI, Configure& t_Config, RSU* t_RSUAry, VeUE* t_VeUEAry, vector<Event>& t_EventVec, vector<list<int>>& t_EventTTIList, vector<vector<int>>& t_TTIRSUThroughput, GTT_Basic* t_GTTPoint, WT_Basic* t_WTPoint, int t_ThreadNum) :
	RRM_Basic(t_TTI, t_Config, t_RSUAry, t_VeUEAry, t_EventVec, t_EventTTIList, t_TTIRSUThroughput), m_GTTPoint(t_GTTPoint), m_WTPoint(t_WTPoint), m_ThreadNum(t_ThreadNum) {

	m_InterferenceVec = vector<list<int>>(ns_RRM_TDM_DRA::gc_TotalPatternNum);
	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = m_Config.RSUNum - 1;//修正最后一个边界
}


void RRM_ICC_DRA::initialize() {

}


void RRM_ICC_DRA::cleanWhenLocationUpdate() {

}


void RRM_ICC_DRA::schedule() {

}


void RRM_ICC_DRA::informationClean() {

}


void RRM_ICC_DRA::updateAdmitEventIdList(bool clusterFlag) {

}


void RRM_ICC_DRA::processEventList() {

}


void RRM_ICC_DRA::processScheduleInfoTableWhenLocationUpdate() {

}


void RRM_ICC_DRA::processWaitEventIdListWhenLocationUpdate() {

}


void RRM_ICC_DRA::processSwitchListWhenLocationUpdate() {

}


void RRM_ICC_DRA::processWaitEventIdList() {

}


void RRM_ICC_DRA::selectBasedOnP123() {

}


void RRM_ICC_DRA::delaystatistics() {

}


void RRM_ICC_DRA::conflictListener() {

}


void RRM_ICC_DRA::transimitPreparation() {

}


void RRM_ICC_DRA::transimitStart() {

}


void RRM_ICC_DRA::transimitStartThread(int fromRSUId, int toRSUId) {

}


void RRM_ICC_DRA::transimitEnd() {

}


void RRM_ICC_DRA::writeScheduleInfo(ofstream& out) {

}


void RRM_ICC_DRA::writeTTILogInfo(ofstream& out, int TTI, EventLogType type, int eventId, int RSUId, int clusterIdx, int patternIdx) {

}


void RRM_ICC_DRA::writeClusterPerformInfo(ofstream &out) {

}


int RRM_ICC_DRA::getMaxIndex(const vector<double>&clusterSize) {
	return -1;
}


int RRM_ICC_DRA::getPatternType(int patternIdx) {
	return -1;
}


pair<int, int> RRM_ICC_DRA::getOccupiedSubCarrierRange(MessageType messageType, int patternIdx) {
	return pair<int, int>(-1, -1);
}
