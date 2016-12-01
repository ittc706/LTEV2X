/*
* =====================================================================================
*
*       Filename:  RRM_TDM_DRA.cpp
*
*    Description:  RRM_TDM_DRA模块
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

#include<tuple>
#include<vector>
#include<list>
#include<sstream>
#include<iomanip>
#include<set>
#include<stdexcept>
#include"System.h"

#include"GTT.h"
#include"RRM_TDM_DRA.h"
#include"TMC.h"
#include"WT.h"

#include"VUE.h"
#include"RSU.h"

#include"Function.h"

using namespace std;


default_random_engine RRM_TDM_DRA_VeUE::s_Engine((unsigned)time(NULL));


RRM_TDM_DRA_VeUE::RRM_TDM_DRA_VeUE() :RRM_VeUE(RRM_TDM_DRA::s_TOTAL_PATTERN_NUM) {}


int RRM_TDM_DRA_VeUE::selectRBBasedOnP2(const std::vector<int>&t_CurAvaliablePatternIdx) {
	int size = static_cast<int>(t_CurAvaliablePatternIdx.size());
	if (size == 0) return -1;
	std::uniform_int_distribution<int> u(0, size - 1);
	return t_CurAvaliablePatternIdx[u(s_Engine)];
}

string RRM_TDM_DRA_VeUE::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	ss << indent << "{ VeUEId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_VeUEId;
	ss << " , RSUId = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_RSUId;
	ss << " , ClusterIdx = " << left << setw(3) << getSystemPoint()->getGTTPoint()->m_ClusterIdx;
	ss << " , ScheduleInterval = [" << left << setw(3) << get<0>(m_ScheduleInterval) << "," << left << setw(3) << get<1>(m_ScheduleInterval) << "] }";
	return ss.str();
}


RRM_TDM_DRA_RSU::RRM_TDM_DRA_RSU() {}


void RRM_TDM_DRA_RSU::initialize() {
	m_AccessEventIdList = vector<pair<list<int>, list<int>>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_WaitEventIdList = vector<pair<list<int>, list<int>>>(getSystemPoint()->getGTTPoint()->m_ClusterNum);
	m_PatternIsAvailable = vector<vector<bool>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<bool>(RRM_TDM_DRA::s_TOTAL_PATTERN_NUM, true));
	m_ScheduleInfoTable = vector<vector<ScheduleInfo*>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<ScheduleInfo*>(RRM_TDM_DRA::s_TOTAL_PATTERN_NUM, nullptr));
	m_TransimitScheduleInfoList = vector<vector<list<ScheduleInfo*>>>(getSystemPoint()->getGTTPoint()->m_ClusterNum, vector<list<ScheduleInfo*>>(RRM_TDM_DRA::s_TOTAL_PATTERN_NUM));
}


int RRM_TDM_DRA_RSU::getClusterIdx(int t_TTI) {
	int roundATTI = t_TTI%RRM_TDM_DRA::s_NTTI; //将TTI映射到[0-s_NTTI)的范围
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++)
		if (roundATTI <= get<1>(m_ClusterTDRInfo[clusterIdx])) return clusterIdx;
	return -1;
}


string RRM_TDM_DRA_RSU::toString(int t_NumTab) {
	string indent;
	for (int i = 0; i < t_NumTab; i++)
		indent.append("    ");

	ostringstream ss;
	//主干信息
	ss << indent << "RSU[" << getSystemPoint()->getGTTPoint()->m_RSUId << "] :" << endl;
	ss << indent << "{" << endl;

	//开始打印VeUEIdList
	ss << indent << "    " << "VeUEIdList :" << endl;
	ss << indent << "    " << "{" << endl;
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		ss << indent << "        " << "Cluster[" << clusterIdx << "] :" << endl;
		ss << indent << "        " << "{" << endl;
		int cnt = 0;
		for (int RSUId : getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx]) {
			if (cnt % 10 == 0)
				ss << indent << "            [ ";
			ss << left << setw(3) << RSUId << " , ";
			if (cnt % 10 == 9)
				ss << " ]" << endl;
			cnt++;
		}
		if (cnt != 0 && cnt % 10 != 0)
			ss << " ]" << endl;
		ss << indent << "        " << "}" << endl;
	}
	ss << indent << "    " << "}" << endl;

	//开始打印clusterInfo
	ss << indent << "    " << "clusterInfo :" << endl;
	ss << indent << "    " << "{" << endl;

	ss << indent << "        ";
	for (const tuple<int, int, int>&t : getTDM_DRAPoint()->m_ClusterTDRInfo)
		ss << "[ " << get<0>(t) << " , " << get<1>(t) << " ] ,";
	ss << endl;
	ss << indent << "    " << "}" << endl;


	//主干信息
	ss << indent << "}" << endl;
	return ss.str();
}


void RRM_TDM_DRA_RSU::pushToAccessEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId) {
	if (t_IsEmergency)
		m_AccessEventIdList[t_ClusterIdx].first.push_back(t_EventId);
	else
		m_AccessEventIdList[t_ClusterIdx].second.push_back(t_EventId);
}


void RRM_TDM_DRA_RSU::pushToWaitEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId) {
	if (t_IsEmergency)
		m_WaitEventIdList[t_ClusterIdx].first.push_back(t_EventId);
	else
		m_WaitEventIdList[t_ClusterIdx].second.push_back(t_EventId);
}


void RRM_TDM_DRA_RSU::pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId) {
	t_SwitchVeUEIdList.push_back(t_EventId);
}


void RRM_TDM_DRA_RSU::pushToTransimitScheduleInfoList(ScheduleInfo* t_Info) {
	m_TransimitScheduleInfoList[t_Info->clusterIdx][t_Info->patternIdx].push_back(t_Info);
}


void RRM_TDM_DRA_RSU::pushToScheduleInfoTable(ScheduleInfo* t_Info) {
	m_ScheduleInfoTable[t_Info->clusterIdx][t_Info->patternIdx] = t_Info;
}


void RRM_TDM_DRA_RSU::pullFromScheduleInfoTable(int t_TTI) {
	/*将处于调度表中当前可以传输的信息压入m_TransimitEventIdList*/

	/*  EMERGENCY  */
	for (int clusterIdx = 0; clusterIdx < getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
		for (int patternIdx = 0; patternIdx < RRM_TDM_DRA::s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
			if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
				m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
				m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
			}
		}
	}
	/*  EMERGENCY  */

	int clusterIdx = getClusterIdx(t_TTI);
	for (int patternIdx = RRM_TDM_DRA::s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < RRM_TDM_DRA::s_TOTAL_PATTERN_NUM; patternIdx++) {
		if (m_ScheduleInfoTable[clusterIdx][patternIdx] != nullptr) {
			m_TransimitScheduleInfoList[clusterIdx][patternIdx].push_back(m_ScheduleInfoTable[clusterIdx][patternIdx]);
			m_ScheduleInfoTable[clusterIdx][patternIdx] = nullptr;
		}
	}
}


const int RRM_TDM_DRA::s_RB_NUM_PER_PATTERN_TYPE[s_PATTERN_TYPE_NUM] = { 2,10,10 };

const int RRM_TDM_DRA::s_PATTERN_NUM_PER_PATTERN_TYPE[s_PATTERN_TYPE_NUM] = { 0,5,0 };

const int RRM_TDM_DRA::s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[s_PATTERN_TYPE_NUM][2] = {
	{ 0,s_PATTERN_NUM_PER_PATTERN_TYPE[0] - 1 },
	{ s_PATTERN_NUM_PER_PATTERN_TYPE[0],s_PATTERN_NUM_PER_PATTERN_TYPE[0] + s_PATTERN_NUM_PER_PATTERN_TYPE[1] - 1 },
	{ s_PATTERN_NUM_PER_PATTERN_TYPE[0] + s_PATTERN_NUM_PER_PATTERN_TYPE[1],s_PATTERN_NUM_PER_PATTERN_TYPE[0] + s_PATTERN_NUM_PER_PATTERN_TYPE[1] + s_PATTERN_NUM_PER_PATTERN_TYPE[2] - 1 },
};

const int RRM_TDM_DRA::s_TOTAL_PATTERN_NUM = [&]() {
	int res = 0;
	for (int num : s_PATTERN_NUM_PER_PATTERN_TYPE)
		res += num;
	return res;
}();


RRM_TDM_DRA::RRM_TDM_DRA(System* t_Context) :
	RRM(t_Context) {
	m_ThreadNum = t_Context->m_Config.threadNum;

	m_InterferenceVec = vector<vector<list<int>>>(getContext()->m_Config.VeUENum, vector<list<int>>(s_TOTAL_PATTERN_NUM));
	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = getContext()->m_Config.RSUNum / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = getContext()->m_Config.RSUNum - 1;//修正最后一个边界
}


void RRM_TDM_DRA::initialize() {
	//初始化VeUE的该模块参数部分
	m_VeUEAry = new RRM_VeUE*[getContext()->m_Config.VeUENum];
	for (int VeUEId = 0; VeUEId <getContext()->m_Config.VeUENum; VeUEId++) {
		m_VeUEAry[VeUEId] = new RRM_TDM_DRA_VeUE();
	}

	//初始化RSU的该模块参数部分
	m_RSUAry = new RRM_RSU*[getContext()->m_Config.RSUNum];
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		m_RSUAry[RSUId] = new RRM_TDM_DRA_RSU();
	}
}


void RRM_TDM_DRA::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId]->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();

		m_VeUEAry[VeUEId]->m_PreSINR.assign(s_TOTAL_PATTERN_NUM, (numeric_limits<double>::min)());
	}
}


void RRM_TDM_DRA::schedule() {
	bool clusterFlag = getContext()->m_TTI  %getContext()->m_Config.locationUpdateNTTI == 0;

	//资源分配信息清空:包括每个RSU内的接入链表等
	informationClean();

	//根据簇大小进行时域资源的划分
	//groupSizeBasedTDM(clusterFlag);
	uniformTDM(clusterFlag);

	//更新等待链表
	updateAccessEventIdList(clusterFlag);

	//资源选择
	selectRBBasedOnP123();

	//统计时延信息
	delaystatistics();

	//帧听冲突
	conflictListener();

	//请求地理拓扑单元计算干扰响应矩阵
	transimitPreparation();

	//模拟传输开始，更新调度信息
	transimitStart();

	//写入调度信息
	writeScheduleInfo();

	//模拟传输结束，统计吞吐量
	transimitEnd();
}


void RRM_TDM_DRA::informationClean() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			_RSU->getTDM_DRAPoint()->m_AccessEventIdList[clusterIdx].first.clear();
			_RSU->getTDM_DRAPoint()->m_AccessEventIdList[clusterIdx].second.clear();
		}
	}
}


void RRM_TDM_DRA::groupSizeBasedTDM(bool t_ClusterFlag) {
	if (!t_ClusterFlag)return;
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		//特殊情况，当该RSU内无一辆车时
		if (_RSU->getSystemPoint()->getGTTPoint()->m_VeUEIdList.size() == 0) {
			/*-----------------------ATTENTION-----------------------
			* 若赋值为(0,-1,0)会导致获取当前簇编号失败，导致其他地方需要讨论
			* 因此直接给每个簇都赋值为整个区间，反正也没有任何作用，免得其他部分讨论
			*------------------------ATTENTION-----------------------*/
			int tempNTTI = s_NTTI;//不知道为啥，直接在下面的式子中用s_NTTI，用g++/gcc编译后链接会出现s_NTTI未定义的情况
			_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum, tuple<int, int, int>(0, tempNTTI - 1, tempNTTI));
			continue;
		}

		//初始化
		_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		//计算每个TTI时隙对应的VeUE数目(double)，!!!浮点数!!！
		double VeUESizePerTTI = static_cast<double>(_RSU->getSystemPoint()->getGTTPoint()->m_VeUEIdList.size()) / static_cast<double>(s_NTTI);

		//clusterSize存储每个簇的VeUE数目(double)，!!!浮点数!!！
		vector<double> clusterSize(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum, 0);

		//初始化clusterSize
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++)
			clusterSize[clusterIdx] = static_cast<double>(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx].size());

		//首先给至少有一辆车的簇分配一份TTI
		int basicNTTI = 0;
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			//如果该簇内至少有一辆VeUE，直接分配给一个单位的时域资源
			if (_RSU->getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx].size() != 0) {
				get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) = 1;
				clusterSize[clusterIdx] -= VeUESizePerTTI;
				basicNTTI++;
			}
		}

		//除了给不为空的簇分配的一个TTI外，剩余可分配的TTI数量
		int remainNTTI = s_NTTI - basicNTTI;

		//对于剩下的资源块，循环将下一时隙分配给当前比例最高的簇，分配之后，更改对应的比例（减去该时隙对应的VeUE数）
		while (remainNTTI > 0) {
			int dex = getMaxIndex(clusterSize);
			if (dex == -1) throw logic_error("还存在没有分配的时域资源，但是每个簇内的VeUE已经为负数");
			get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[dex])++;
			remainNTTI--;
			clusterSize[dex] -= VeUESizePerTTI;
		}

		//开始生成区间范围，闭区间
		get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[0]) = 0;
		get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[0]) = get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[0]) + get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[0]) - 1;
		for (int clusterIdx = 1; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) = get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx - 1]) + 1;
			get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) = get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) + get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) - 1;
		}


		//将调度区间写入该RSU内的每一个车辆
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU->getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId]->getTDM_DRAPoint()->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]), get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]));
		}
	}
	writeClusterPerformInfo();
}


void RRM_TDM_DRA::uniformTDM(bool t_ClusterFlag) {
	if (!t_ClusterFlag)return;
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		//初始化
		_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		int equalTimeLength = s_NTTI / _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum;

		int lastClusterLength = s_NTTI - equalTimeLength*_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum + equalTimeLength;

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx] = tuple<int, int, int>(equalTimeLength*clusterIdx, equalTimeLength*(clusterIdx + 1) - 1, equalTimeLength);
		}

		//修复最后一个簇的时域长度，因为平均簇长可能被四舍五入了，因此，平均簇长度*簇数并不等于总调度时间，因此将差异填入最后一个簇
		get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum - 1]) = s_NTTI - 1;
		get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum - 1]) = lastClusterLength;



		//将调度区间写入该RSU内的每一个车辆
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU->getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId]->getTDM_DRAPoint()->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]), get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]));
		}
	}
	writeClusterPerformInfo();
}


void RRM_TDM_DRA::updateAccessEventIdList(bool t_ClusterFlag) {
	//首先，处理System级别的事件触发链表
	processEventList();
	//其次，如果当前TTI进行了位置更新，需要处理调度表
	if (t_ClusterFlag) {
		//处理RSU级别的调度链表
		processScheduleInfoTableWhenLocationUpdate();

		//处理RSU级别的等待链表
		processWaitEventIdListWhenLocationUpdate();

		//处理System级别的切换链表
		processSwitchListWhenLocationUpdate();
	}
	//最后，由等待表生成接入表
	processWaitEventIdList();
}


void RRM_TDM_DRA::processEventList() {
	for (int eventId : getContext()->m_TMCPoint->m_EventTTIList[getContext()->m_TTI]) {
		Event &event = getContext()->m_TMCPoint->m_EventVec[eventId];
		int VeUEId = event.getVeUEId();
		int RSUId = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId;
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx;

		//将该事件压入紧急事件等待链表
		bool isEmergency = event.getMessageType() == EMERGENCY;
		_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//更新日志
		getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, EVENT_TO_WAIT, -1, -1, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Trigger");
		writeTTILogInfo(getContext()->m_TTI, EVENT_TO_WAIT, eventId, -1, -1, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Trigger");
	}
}


void RRM_TDM_DRA::processScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		//开始处理 m_ScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {
				if (_RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[clusterIdx][patternIdx] == nullptr) continue;

				int eventId = _RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[clusterIdx][patternIdx]->eventId;
				int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();
				//该VeUE不在当前RSU中，应将其压入System级别的切换链表
				if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId != _RSU->getSystemPoint()->getGTTPoint()->m_RSUId) {
					//压入Switch链表
					_RSU->getTDM_DRAPoint()->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//将剩余待传bit重置
					getContext()->m_TMCPoint->m_EventVec[eventId].reset();

					//并释放该调度信息的资源
					Delete::safeDelete(_RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[clusterIdx][patternIdx]);

					//释放Pattern资源
					_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					//更新日志
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "LocationUpdate");
				}
				else {
					//RSU内部发生了簇切换，将其从调度表中取出，压入等待链表
					if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx != clusterIdx) {
						//压入该RSU的等待链表
						bool isEmergency = getContext()->m_TMCPoint->m_EventVec[eventId].getMessageType() == EMERGENCY;
						_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(isEmergency, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, eventId);

						//并释放该调度信息的资源
						Delete::safeDelete(_RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[clusterIdx][patternIdx]);

						//释放Pattern资源
						_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

						//更新日志
						getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, SCHEDULETABLE_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
						writeTTILogInfo(getContext()->m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
					}
				}
			}
		}
	}
}


void RRM_TDM_DRA::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		//开始处理 m_WaitEventIdList

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			/*  EMERGENCY  */
			list<int>::iterator it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.begin();
			while (it != _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.end()) {
				int eventId = *it;
				int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();
				//该VeUE已经不在该RSU范围内
				if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId != _RSU->getSystemPoint()->getGTTPoint()->m_RSUId) {

					//将其添加到System级别的RSU切换链表中
					_RSU->getTDM_DRAPoint()->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//将其从等待链表中删除
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.erase(it);

					//将剩余待传bit重置
					getContext()->m_TMCPoint->m_EventVec[eventId].reset();

					//更新日志
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_SWITCH, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_SWITCH, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				//仍然处于当前RSU范围内，但位于不同的簇
				else if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx != clusterIdx) {

					//将其添加到所在簇的等待链表
					_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(true, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, eventId);

					//将其从等待链表中的当前簇删除
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.erase(it);

					//更新日志
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
				}
				else {
					it++;
					continue; //继续留在当前RSU的等待链表
				}
			}
			/*  EMERGENCY  */

			it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.begin();
			while (it != _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.end()) {
				int eventId = *it;
				int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();
				//该VeUE已经不在该RSU范围内
				if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId != _RSU->getSystemPoint()->getGTTPoint()->m_RSUId) {

					//将其添加到System级别的RSU切换链表中
					_RSU->getTDM_DRAPoint()->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//将其从等待链表中删除
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.erase(it);

					//将剩余待传bit重置
					getContext()->m_TMCPoint->m_EventVec[eventId].reset();

					//更新日志
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_SWITCH, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_SWITCH, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				//仍然处于当前RSU范围内，但位于不同的簇
				else if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx != clusterIdx) {

					//将其添加到所在簇的等待链表
					_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(false, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, eventId);

					//将其从等待链表中的当前簇删除
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.erase(it);

					//更新日志
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
				}
				else {
					it++;
					continue; //继续留在当前RSU的等待链表
				}
			}
		}

	}
}


void RRM_TDM_DRA::processSwitchListWhenLocationUpdate() {
	list<int>::iterator it = m_SwitchEventIdList.begin();
	while (it != m_SwitchEventIdList.end()) {
		int eventId = *it;
		int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();
		int RSUId = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId;
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx;

		//转入等待链表
		bool isEmergency = getContext()->m_TMCPoint->m_EventVec[eventId].getMessageType() == EMERGENCY;
		_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//从Switch表中将其删除
		it = m_SwitchEventIdList.erase(it);

		//更新日志
		getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, SWITCH_TO_WAIT, -1, -1, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "LocationUpdate");
		writeTTILogInfo(getContext()->m_TTI, SWITCH_TO_WAIT, eventId, -1, -1, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "LocationUpdate");
	}
}


void RRM_TDM_DRA::processWaitEventIdList() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			list<int>::iterator it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.begin();
			while (it != _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.end()) {
				int eventId = *it;
				//如果该事件不需要退避，则转入接入表
				if (getContext()->m_TMCPoint->m_EventVec[eventId].tryAcccess()) {

					//压入接入链表
					_RSU->getTDM_DRAPoint()->pushToAccessEventIdList(true, clusterIdx, eventId);

					//更新日志
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_ACCESS, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_ACCESS, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");

					//将该事件从等代表中删除
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.erase(it);
				}
				else {
					//更新日志
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WITHDRAWING, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "Withdraw");
					writeTTILogInfo(getContext()->m_TTI, WITHDRAWING, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "Withdraw");

					it++;
				}
			}
		}
		/*  EMERGENCY  */
		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);
		list<int>::iterator it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.begin();
		while (it != _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.end()) {
			int eventId = *it;
			//如果该事件不需要退避，则转入接入表
			if (getContext()->m_TMCPoint->m_EventVec[eventId].tryAcccess()) {

				//压入接入链表
				_RSU->getTDM_DRAPoint()->pushToAccessEventIdList(false, clusterIdx, eventId);

				//更新日志
				getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_ACCESS, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");
				writeTTILogInfo(getContext()->m_TTI, WAIT_TO_ACCESS, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");

				//将该事件从等代表中删除
				it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.erase(it);
			}
			else {
				//更新日志
				getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WITHDRAWING, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "Withdraw");
				writeTTILogInfo(getContext()->m_TTI, WITHDRAWING, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "Withdraw");

				it++;
			}
		}
	}
}


void RRM_TDM_DRA::selectRBBasedOnP123() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			vector<int> curAvaliableEmergencyPatternIdx;//当前簇可用的EmergencyPattern编号

			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
				if (_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliableEmergencyPatternIdx.push_back(patternIdx);
				}
			}

			for (int eventId : _RSU->getTDM_DRAPoint()->m_AccessEventIdList[clusterIdx].first) {
				int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();

				//为当前用户在可用的EmergencyPattern块中随机选择一个，每个用户自行随机选择可用EmergencyPattern块
				int patternIdx = m_VeUEAry[VeUEId]->getTDM_DRAPoint()->selectRBBasedOnP2(curAvaliableEmergencyPatternIdx);

				//无对应Pattern类型的pattern资源可用
				if (patternIdx == -1) {
					//更新日志
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, ACCESS_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");
					writeTTILogInfo(getContext()->m_TTI, ACCESS_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");

					continue;
				}
				//将资源标记为占用
				_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

				//将调度信息压入m_EmergencyTransimitEventIdList中
				_RSU->getTDM_DRAPoint()->pushToTransimitScheduleInfoList(new RRM_RSU::ScheduleInfo(eventId, VeUEId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, -1, patternIdx));

			}

		}
		/*  EMERGENCY  */

		//开始处理非Emergency的事件
		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);

		/*
		* 当前TTI可用的Pattern块编号
		* 下标代表的Pattern种类的编号，其中Emergency那一项是空的，但是保留，想避免编号产生偏移
		* 每个内层vector代表该种类Pattern可用的Pattern编号(绝对Pattern编号)
		*/
		vector<vector<int>> curAvaliablePatternIdx(s_PATTERN_TYPE_NUM);

		for (int patternTypeIdx = 1; patternTypeIdx < s_PATTERN_TYPE_NUM; patternTypeIdx++) {
			for (int patternIdx = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternTypeIdx][0]; patternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternTypeIdx][1]; patternIdx++) {
				if (_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		for (int eventId : _RSU->getTDM_DRAPoint()->m_AccessEventIdList[clusterIdx].second) {
			int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();

			//为当前用户在可用的对应其事件类型的Pattern块中随机选择一个，每个用户自行随机选择可用Pattern块
			MessageType messageType = getContext()->m_TMCPoint->m_EventVec[eventId].getMessageType();
			int patternIdx = m_VeUEAry[VeUEId]->getTDM_DRAPoint()->selectRBBasedOnP2(curAvaliablePatternIdx[messageType]);

			//该用户传输的信息类型没有pattern剩余了
			if (patternIdx == -1) {
				//更新日志
				getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, ACCESS_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");
				writeTTILogInfo(getContext()->m_TTI, ACCESS_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");

				continue;
			}

			//将资源标记为占用
			_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

			//将调度信息压入m_TransimitEventIdList中
			_RSU->getTDM_DRAPoint()->pushToTransimitScheduleInfoList(new RRM_RSU::ScheduleInfo(eventId, VeUEId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx));

		}
		//将调度表中当前可以继续传输的用户压入传输链表中(包括紧急和非紧急)
		_RSU->getTDM_DRAPoint()->pullFromScheduleInfoTable(getContext()->m_TTI);
	}
}


void RRM_TDM_DRA::delaystatistics() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			//处理等待链表
			for (int eventId : _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first)
				getContext()->m_TMCPoint->m_EventVec[eventId].increaseQueueDelay();
			//处理此刻正在将要传输的传输链表
			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++)
				for (RRM_RSU::ScheduleInfo* &p : _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
					getContext()->m_TMCPoint->m_EventVec[p->eventId].increaseSendDelay();
		}
		/*  EMERGENCY  */


		//处理等待链表
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int eventId : _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second)
				getContext()->m_TMCPoint->m_EventVec[eventId].increaseQueueDelay();
		}

		//处理此刻正在将要传输的传输链表
		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);
		for (int patternIdx = s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {
			for (RRM_RSU::ScheduleInfo* &p : _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
				getContext()->m_TMCPoint->m_EventVec[p->eventId].increaseSendDelay();
		}

		//处理此刻位于调度表中，但不属于当前簇的事件
		for (int otherClusterIdx = 0; otherClusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; otherClusterIdx++) {
			if (otherClusterIdx == clusterIdx) continue;
			for (RRM_RSU::ScheduleInfo *p : _RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[otherClusterIdx]) {
				if (p == nullptr) continue;
				getContext()->m_TMCPoint->m_EventVec[p->eventId].increaseQueueDelay();
			}
		}
	}
}

void RRM_TDM_DRA::conflictListener() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
				list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
					for (RRM_RSU::ScheduleInfo* &info : lst) {
						//首先将事件压入等待列表
						_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(true, clusterIdx, info->eventId);

						//冲突后更新事件的状态
						getContext()->m_TMCPoint->m_EventVec[info->eventId].conflict();

						//更新日志
						getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Conflict");
						writeTTILogInfo(getContext()->m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Conflict");

						//释放调度信息对象的内存资源
						Delete::safeDelete(info);
					}

					//释放Pattern资源
					_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					lst.clear();
				}
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);
		for (int patternIdx = s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {

			list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
			if (lst.size() > 1) {//多于一个VeUE在当前TTI，该Pattern上传输，即发生了冲突，将其添加到等待列表
				for (RRM_RSU::ScheduleInfo* &info : lst) {
					//首先将事件压入等待列表
					_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(false, clusterIdx, info->eventId);

					//冲突后更新事件的状态
					getContext()->m_TMCPoint->m_EventVec[info->eventId].conflict();

					//更新日志
					getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Conflict");
					writeTTILogInfo(getContext()->m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Conflict");

					//释放调度信息对象的内存资源
					Delete::safeDelete(info);
				}
				//释放Pattern资源
				_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

				lst.clear();
			}
		}
	}
}


void RRM_TDM_DRA::transimitPreparation() {
	//首先清空上一次干扰信息
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++)
		for (int patternIdx = 0; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++)
			m_InterferenceVec[VeUEId][patternIdx].clear();

	//统计本次的干扰信息
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {
				list<RRM_RSU::ScheduleInfo*> &curList = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (curList.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
					RRM_RSU::ScheduleInfo *curInfo = *curList.begin();
					int curVeUEId = curInfo->VeUEId;
					for (int otherClusterIdx = 0; otherClusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; otherClusterIdx++) {
						if (otherClusterIdx == clusterIdx)continue;
						list<RRM_RSU::ScheduleInfo*> &otherList = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[otherClusterIdx][patternIdx];
						if (otherList.size() == 1) {//其他簇中该pattern下有车辆在传输，那么将该车辆作为干扰车辆
							RRM_RSU::ScheduleInfo *otherInfo = *otherList.begin();
							int otherVeUEId = otherInfo->VeUEId;
							m_InterferenceVec[curVeUEId][patternIdx].push_back(otherVeUEId);
						}
					}
				}
			}
		}
	}

	//更新每辆车的干扰车辆列表	
	for (int patternIdx = 0; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {
		for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
			list<int>& interList = m_InterferenceVec[VeUEId][patternIdx];

			m_VeUEAry[VeUEId]->m_InterferenceVeUENum[patternIdx] = (int)interList.size();//写入干扰数目

			m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx].assign(interList.begin(), interList.end());//写入干扰车辆ID

			/*if (m_VeUEAry[VeUEId]->m_InterferenceVeUENum[patternIdx]>0) {
				g_FileTemp << "VeUEId: " << VeUEId << " [";
				for (auto c : m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx])
					g_FileTemp << c << ", ";
				g_FileTemp << " ]" << endl;
			}*/
		}
	}

	//请求地理拓扑单元计算干扰响应矩阵
	long double start = clock();
	getContext()->m_GTTPoint->calculateInterference(m_InterferenceVec);
	long double end = clock();
	m_GTTTimeConsume += end - start;
}


void RRM_TDM_DRA::transimitStart() {
	long double start = clock();
	m_Threads.clear();
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads.push_back(thread(&RRM_TDM_DRA::transimitStartThread, &*this, m_ThreadsRSUIdRange[threadIdx].first, m_ThreadsRSUIdRange[threadIdx].second));
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++)
		m_Threads[threadIdx].join();
	long double end = clock();
	m_WTTimeConsume += end - start;
}

void RRM_TDM_DRA::transimitStartThread(int t_FromRSUId, int t_ToRSUId) {
	WT* copyWTPoint = getContext()->m_WTPoint->getCopy();//由于每个线程的该模块会有不同的状态且无法共享，因此这里拷贝该模块用于本次计算
	for (int RSUId = t_FromRSUId; RSUId <= t_ToRSUId; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
				list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {
					RRM_RSU::ScheduleInfo *info = *lst.begin();
					int VeUEId = info->VeUEId;

					//计算SINR，获取调制编码方式
					pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(getContext()->m_TMCPoint->m_EventVec[info->eventId].getMessageType(), patternIdx);
					//g_FileTemp << "Emergency PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

					double factor = m_VeUEAry[VeUEId]->m_ModulationType * m_VeUEAry[VeUEId]->m_CodeRate;

					//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
					int maxEquivalentBitNum = (int)((double)(s_RB_NUM_PER_PATTERN_TYPE[EMERGENCY] * s_BIT_NUM_PER_RB)* factor);

					//计算SINR
					double curSINR = 0;
					if (m_VeUEAry[VeUEId]->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId]->isAlreadyCalculateSINR(patternIdx)) {//调制编码方式需要更新时
						curSINR = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
						m_VeUEAry[VeUEId]->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx];
						m_VeUEAry[VeUEId]->m_PreSINR[patternIdx] = curSINR;
					}
					else
						curSINR = m_VeUEAry[VeUEId]->m_PreSINR[patternIdx];

					//记录调度信息
					double tmpDistance = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_Distance[RSUId];
					if (curSINR < s_DROP_SINR_BOUNDARY) {
						//记录丢包
						getContext()->m_TMCPoint->m_EventVec[info->eventId].packetLoss(tmpDistance);
					}
					info->transimitBitNum = maxEquivalentBitNum;
					info->currentPackageIdx = getContext()->m_TMCPoint->m_EventVec[info->eventId].getCurrentPackageIdx();
					info->remainBitNum = getContext()->m_TMCPoint->m_EventVec[info->eventId].getRemainBitNum();

					//该编码方式下，该Pattern在一个TTI传输的实际的有效信息bit数量，并更新信息状态
					int realEquivalentBitNum = getContext()->m_TMCPoint->m_EventVec[info->eventId].transimit(maxEquivalentBitNum, tmpDistance);

					//累计吞吐率
					getContext()->m_TMCPoint->m_TTIRSUThroughput[getContext()->m_TTI][_RSU->getSystemPoint()->getGTTPoint()->m_RSUId] += realEquivalentBitNum;

					//更新日志
					getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMITTING, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
					writeTTILogInfo(getContext()->m_TTI, TRANSIMITTING, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
				}
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);
		for (int patternIdx = s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {

			list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
			if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
				RRM_RSU::ScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				int patternType = getPatternType(patternIdx);

				//计算SINR，获取调制编码方式
				pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(getContext()->m_TMCPoint->m_EventVec[info->eventId].getMessageType(), patternIdx);
				//g_FileTemp << "NonEmergencyPatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << ((patternType == 0) ? "Emergency" : (patternType == 1 ? "Period" : "Data")) << endl;

				double factor = m_VeUEAry[VeUEId]->m_ModulationType * m_VeUEAry[VeUEId]->m_CodeRate;

				//该编码方式下，该Pattern在一个TTI最多可传输的有效信息bit数量
				int maxEquivalentBitNum = (int)((double)(s_RB_NUM_PER_PATTERN_TYPE[patternType] * s_BIT_NUM_PER_RB)* factor);

				//计算SINR
				double curSINR = 0;
				if (m_VeUEAry[VeUEId]->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId]->isAlreadyCalculateSINR(patternIdx)) {//调制编码方式需要更新时
					curSINR = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId]->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId]->m_PreSINR[patternIdx] = curSINR;
				}
				else
					curSINR = m_VeUEAry[VeUEId]->m_PreSINR[patternIdx];

				//记录调度信息
				double tmpDistance = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_Distance[RSUId];
				if (curSINR < s_DROP_SINR_BOUNDARY) {
					//记录丢包
					getContext()->m_TMCPoint->m_EventVec[info->eventId].packetLoss(tmpDistance);
				}
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = getContext()->m_TMCPoint->m_EventVec[info->eventId].getCurrentPackageIdx();
				info->remainBitNum = getContext()->m_TMCPoint->m_EventVec[info->eventId].getRemainBitNum();

				//该编码方式下，该Pattern在一个TTI传输的实际的有效信息bit数量，并更新信息状态
				int realEquivalentBitNum = getContext()->m_TMCPoint->m_EventVec[info->eventId].transimit(maxEquivalentBitNum, tmpDistance);

				//累计吞吐率
				getContext()->m_TMCPoint->m_TTIRSUThroughput[getContext()->m_TTI][_RSU->getSystemPoint()->getGTTPoint()->m_RSUId] += realEquivalentBitNum;

				//更新日志
				getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMITTING, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
				writeTTILogInfo(getContext()->m_TTI, TRANSIMITTING, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
			}
		}
	}
	copyWTPoint->freeCopy();//getCopy是通过new创建的，因此这里释放资源
}

void RRM_TDM_DRA::writeScheduleInfo() {
	if (!getContext()->m_Config.scheduleLogIsOn)return;
	m_FileScheduleInfo << "[ TTI = " << left << setw(3) << getContext()->m_TTI << "]" << endl;
	m_FileScheduleInfo << "{" << endl;
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		int NonEmergencyClusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);

		m_FileScheduleInfo << "    RSU[" << _RSU->getSystemPoint()->getGTTPoint()->m_RSUId << "][TTI = " << getContext()->m_TTI << "]" << endl;
		m_FileScheduleInfo << "    {" << endl;
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			m_FileScheduleInfo << "        Cluster[" << clusterIdx << "]" << endl;
			m_FileScheduleInfo << "        {" << endl;

			m_FileScheduleInfo << "            EMERGENCY:" << endl;
			for (int patternIdx = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[EMERGENCY][0]; patternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[EMERGENCY][1]; patternIdx++) {
				bool isAvaliable = _RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx];
				m_FileScheduleInfo << "                Pattern[" << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
				if (!isAvaliable) {
					RRM_RSU::ScheduleInfo *info = *(_RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					m_FileScheduleInfo << info->toScheduleString(5) << endl;
				}
			}
			if (clusterIdx != NonEmergencyClusterIdx)continue;
			m_FileScheduleInfo << "            PERIOD:" << endl;
			for (int patternIdx = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[PERIOD][0]; patternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[PERIOD][1]; patternIdx++) {
				bool isAvaliable = _RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx];
				m_FileScheduleInfo << "                Pattern[" << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
				if (!isAvaliable) {
					RRM_RSU::ScheduleInfo *info = *(_RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					m_FileScheduleInfo << info->toScheduleString(5) << endl;
				}
			}
			m_FileScheduleInfo << "            DATA:" << endl;
			for (int patternIdx = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[DATA][0]; patternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[DATA][1]; patternIdx++) {
				bool isAvaliable = _RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx];
				m_FileScheduleInfo << "                Pattern[" << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
				if (!isAvaliable) {
					RRM_RSU::ScheduleInfo *info = *(_RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					m_FileScheduleInfo << info->toScheduleString(5) << endl;
				}
			}
			m_FileScheduleInfo << "        }" << endl;
		}
		m_FileScheduleInfo << "{" << endl;
	}
	m_FileScheduleInfo << "}" << endl;
	m_FileScheduleInfo << "\n\n" << endl;
}

void RRM_TDM_DRA::transimitEnd() {
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
				list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {
					RRM_RSU::ScheduleInfo* &info = *lst.begin();
					//已经传输完毕，将资源释放
					if (getContext()->m_TMCPoint->m_EventVec[info->eventId].isFinished()) {

						//更新日志
						getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, SUCCEED, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
						writeTTILogInfo(getContext()->m_TTI, SUCCEED, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

						//释放调度信息对象的内存资源
						Delete::safeDelete(info);

						//释放Pattern资源
						_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;
					}
					else {//尚未传输完毕
						_RSU->getTDM_DRAPoint()->pushToScheduleInfoTable(info);
						info = nullptr;
					}
				}
				lst.clear();
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);
		for (int patternIdx = s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {

			list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
			if (lst.size() == 1) {//只有一个用户在传输，该用户会正确的传输所有数据（在离开簇之前）
				RRM_RSU::ScheduleInfo* &info = *lst.begin();
				//说明该数据已经传输完毕
				if (getContext()->m_TMCPoint->m_EventVec[info->eventId].isFinished()) {

					//更新日志								
					getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, SUCCEED, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
					writeTTILogInfo(getContext()->m_TTI, SUCCEED, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

					//释放调度信息对象的内存资源
					Delete::safeDelete(info);

					//释放Pattern资源
					_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;
				}
				else {//该数据仍未传完，将其压回m_ScheduleInfoTable
					_RSU->getTDM_DRAPoint()->pushToScheduleInfoTable(info);
					info = nullptr;
				}
			}
			//处理完后，将该pattern上的数据清空（此时要不本身就是空，要不就是nullptr指针）
			lst.clear();
		}
	}
}


void RRM_TDM_DRA::writeTTILogInfo(int t_TTI, EventLogType t_EventLogType, int t_EventId, int t_FromRSUId, int t_FromClusterIdx, int t_FromPatternIdx, int t_ToRSUId, int t_ToClusterIdx, int t_ToPatternIdx, std::string t_Description) {
	if (!getContext()->m_Config.scheduleLogIsOn)return;
	stringstream ss;
	switch (t_EventLogType) {
	case TRANSIMITTING:
		ss << " - Transimiting  At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WITHDRAWING:
		ss << " - Withdrawing  At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SUCCEED:
		ss << " - Transimit Succeed At: RSU[" << t_FromRSUId << "] - ClusterIdx[" << t_FromClusterIdx << "] - PatternIdx[" << t_FromPatternIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case EVENT_TO_WAIT:
		ss << " - From: EventList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SCHEDULETABLE_TO_SWITCH:
		ss << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: SwitchList";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SCHEDULETABLE_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s ScheduleTable[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WAIT_TO_SWITCH:
		ss << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: SwitchList";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WAIT_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case WAIT_TO_ACCESS:
		ss << " - From: RSU[" << t_FromRSUId << "]'s WaitEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s AccessEventIdList[" << t_ToClusterIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case ACCESS_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s AccessEventIdList[" << t_FromClusterIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case SWITCH_TO_WAIT:
		ss << " - From: SwitchList - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	case TRANSIMIT_TO_WAIT:
		ss << " - From: RSU[" << t_FromRSUId << "]'s TransimitScheduleInfoList[" << t_FromClusterIdx << "][" << t_FromPatternIdx << "] - To: RSU[" << t_ToRSUId << "]'s WaitEventIdList[" << t_ToClusterIdx << "]";
		m_FileTTILogInfo << "{ TTI : " << left << setw(3) << t_TTI << " - EventId = " << left << setw(3) << t_EventId << " - Description : <" << left << setw(10) << t_Description + ">" << ss.str() << " }" << endl;
		break;
	}
}


void RRM_TDM_DRA::writeClusterPerformInfo() {
	m_FileClasterPerformInfo << "[ TTI = " << left << setw(3) << getContext()->m_TTI << "]" << endl;
	m_FileClasterPerformInfo << "{" << endl;

	//打印VeUE信息
	m_FileClasterPerformInfo << "    VUE Info: " << endl;
	m_FileClasterPerformInfo << "    {" << endl;
	for (int VeUEId = 0; VeUEId < getContext()->m_Config.VeUENum; VeUEId++) {
		RRM_VeUE &_VeUE = *(m_VeUEAry[VeUEId]);
		m_FileClasterPerformInfo << _VeUE.getTDM_DRAPoint()->toString(2) << endl;
	}
	m_FileClasterPerformInfo << "    }\n" << endl;

	////打印基站信息
	//out << "    eNB Info: " << endl;
	//out << "    {" << endl;
	//for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
	//	ceNB &_eNB = m_eNBAry[eNBId];
	//	out << _eNB.toString(2) << endl;
	//}
	//out << "    }\n" << endl;

	//打印RSU信息
	m_FileClasterPerformInfo << "    RSU Info: " << endl;
	m_FileClasterPerformInfo << "    {" << endl;
	for (int RSUId = 0; RSUId < getContext()->m_Config.RSUNum; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		m_FileClasterPerformInfo << _RSU->getTDM_DRAPoint()->toString(2) << endl;
	}
	m_FileClasterPerformInfo << "    }" << endl;

	m_FileClasterPerformInfo << "}\n\n";
}


int RRM_TDM_DRA::getMaxIndex(const vector<double>&t_ClusterSize) {
	double max = 0;
	int dex = -1;
	for (int i = 0; i < static_cast<int>(t_ClusterSize.size()); i++) {
		if (t_ClusterSize[i] > max) {
			dex = i;
			max = t_ClusterSize[i];
		}
	}
	return dex;
}


int RRM_TDM_DRA::getPatternType(int t_PatternIdx) {
	//patternIdx指所有事件类型的Pattern的绝对序号，从0开始编号，包括Emergency
	for (int patternType = 0; patternType < s_PATTERN_TYPE_NUM; patternType++) {
		if (t_PatternIdx >= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternType][0] && t_PatternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternType][1])
			return patternType;
	}
	throw logic_error("getPatternType");
}


pair<int, int> RRM_TDM_DRA::getOccupiedSubCarrierRange(MessageType t_MessageType, int t_PatternIdx) {

	pair<int, int> res;

	int offset = 0;
	for (int i = 0; i < t_MessageType; i++) {
		offset += s_RB_NUM_PER_PATTERN_TYPE[i] * s_PATTERN_NUM_PER_PATTERN_TYPE[i];
		t_PatternIdx -= s_PATTERN_NUM_PER_PATTERN_TYPE[i];
	}
	res.first = offset + s_RB_NUM_PER_PATTERN_TYPE[t_MessageType] * t_PatternIdx;
	res.second = offset + s_RB_NUM_PER_PATTERN_TYPE[t_MessageType] * (t_PatternIdx + 1) - 1;

	res.first *= 12;
	res.second = (res.second + 1) * 12 - 1;
	return res;
}
