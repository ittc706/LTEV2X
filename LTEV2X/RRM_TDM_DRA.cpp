/*
* =====================================================================================
*
*       Filename:  RRM_TDM_DRA.cpp
*
*    Description:  RRMģ���ʱ�ַ�������ͬ�Ĵ�ռ�ò�ͬ��ʱ�䣬�������ϵ�
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
#include"ConfigLoader.h"

using namespace std;

int RRM_TDM_DRA::s_NTTI;

int RRM_TDM_DRA::s_PATTERN_TYPE_NUM;

vector<int> RRM_TDM_DRA::s_RB_NUM_PER_PATTERN_TYPE;

vector<int> RRM_TDM_DRA::s_PATTERN_NUM_PER_PATTERN_TYPE;

vector<pair<int, int>> RRM_TDM_DRA::s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL;

int RRM_TDM_DRA::s_TOTAL_PATTERN_NUM;

void RRM_TDM_DRA::loadConfig(Platform t_Platform) {
	ConfigLoader configLoader;
	if (t_Platform == Windows) {
		configLoader.resolvConfigPath("Config\\RRM_TDM_DRAConfig.xml");
	}
	else if (t_Platform == Linux) {
		configLoader.resolvConfigPath("Config/RRM_TDM_DRAConfig.xml");
	}
	else {
		throw logic_error("Platform Config Error!");
	}

	stringstream ss;

	const string nullString("");
	string temp;

	if ((temp = configLoader.getParam("NTTI")) != nullString) {
		ss << temp;
		ss >> s_NTTI;
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("PatternTypeNum")) != nullString) {
		ss << temp;
		ss >> s_PATTERN_TYPE_NUM;
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("RBNumPerPatternType")) != nullString) {
		s_RB_NUM_PER_PATTERN_TYPE.clear();
		ss << temp;
		string temp2;
		while (ss >> temp2) {
			s_RB_NUM_PER_PATTERN_TYPE.push_back(ConfigLoader::stringToInt(temp2));
		}
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	if ((temp = configLoader.getParam("PatternNumPerPatternType")) != nullString) {
		s_PATTERN_NUM_PER_PATTERN_TYPE.clear();
		ss << temp;
		string temp2;
		while (ss >> temp2) {
			s_PATTERN_NUM_PER_PATTERN_TYPE.push_back(ConfigLoader::stringToInt(temp2));
		}
		ss.clear();//�����־λ
		ss.str("");
	}
	else
		throw logic_error("ConfigLoaderError");

	s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL.assign(s_PATTERN_TYPE_NUM, pair<int, int>(0, 0));
	for (int patternTypeIdx = 0; patternTypeIdx < s_PATTERN_TYPE_NUM; patternTypeIdx++) {
		s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternTypeIdx].first = (patternTypeIdx == 0 ? 0 : s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternTypeIdx - 1].second + 1);
		s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternTypeIdx].second = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternTypeIdx].first + s_PATTERN_NUM_PER_PATTERN_TYPE[patternTypeIdx] - 1;
	}

	s_TOTAL_PATTERN_NUM = [&]() {
		int res = 0;
		for (int num : s_PATTERN_NUM_PER_PATTERN_TYPE)
			res += num;
		return res;
	}();

	/*cout << "NTTI: " << s_NTTI << endl;
	cout << "PatternTypeNum: " << s_PATTERN_TYPE_NUM << endl;
	cout << "RBNumPerPatternType: " << endl;
	Print::printVectorDim1(s_RB_NUM_PER_PATTERN_TYPE);
	cout << "PatternNumPerPatternType: " << endl;
	Print::printVectorDim1(s_PATTERN_NUM_PER_PATTERN_TYPE);
	cout << "PatternTypePatternIndexInterval: " << endl;
	for (pair<int, int>& p : s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL)
		cout << "[" << p.first << "," << p.second << "]" << endl;
	cout << "TotalPatternNum: " << s_TOTAL_PATTERN_NUM << endl;
	system("pause");*/
}

RRM_TDM_DRA::RRM_TDM_DRA(System* t_Context) :
	RRM(t_Context) {
	m_ThreadNum = t_Context->m_Config.threadNum;

	m_InterferenceVec = vector<vector<list<int>>>(GTT::s_VeUE_NUM, vector<list<int>>(s_TOTAL_PATTERN_NUM));
	m_ThreadsRSUIdRange = vector<pair<int, int>>(m_ThreadNum);

	int num = GTT::s_RSU_NUM / m_ThreadNum;
	for (int threadIdx = 0; threadIdx < m_ThreadNum; threadIdx++) {
		m_ThreadsRSUIdRange[threadIdx] = pair<int, int>(threadIdx*num, (threadIdx + 1)*num - 1);
	}
	m_ThreadsRSUIdRange[m_ThreadNum - 1].second = GTT::s_RSU_NUM - 1;//�������һ���߽�
}


void RRM_TDM_DRA::initialize() {
	//��ʼ��VeUE�ĸ�ģ���������
	m_VeUEAry = new RRM_VeUE*[GTT::s_VeUE_NUM];
	for (int VeUEId = 0; VeUEId <GTT::s_VeUE_NUM; VeUEId++) {
		m_VeUEAry[VeUEId] = new RRM_TDM_DRA_VeUE();
	}

	//��ʼ��RSU�ĸ�ģ���������
	m_RSUAry = new RRM_RSU*[GTT::s_RSU_NUM];
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		m_RSUAry[RSUId] = new RRM_TDM_DRA_RSU();
	}
}


void RRM_TDM_DRA::cleanWhenLocationUpdate() {
	for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++) {
		for (vector<int>& preInterferenceVeUEIdVec : m_VeUEAry[VeUEId]->m_PreInterferenceVeUEIdVec)
			preInterferenceVeUEIdVec.clear();

		m_VeUEAry[VeUEId]->m_PreSINR.assign(s_TOTAL_PATTERN_NUM, (numeric_limits<double>::min)());
	}
}


void RRM_TDM_DRA::schedule() {
	bool clusterFlag = getContext()->m_TTI  %getContext()->m_Config.locationUpdateNTTI == 0;

	//��Դ������Ϣ���:����ÿ��RSU�ڵĽ��������
	informationClean();

	//���ݴش�С����ʱ����Դ�Ļ���
	//groupSizeBasedTDM(clusterFlag);
	uniformTDM(clusterFlag);

	//���µȴ�����
	updateAccessEventIdList(clusterFlag);

	//��Դѡ��
	selectRBBasedOnP123();

	//ͳ��ʱ����Ϣ
	delaystatistics();

	//֡����ͻ
	conflictListener();

	//����������˵�Ԫ���������Ӧ����
	transimitPreparation();

	//ģ�⴫�俪ʼ�����µ�����Ϣ
	transimitStart();

	//д�������Ϣ
	writeScheduleInfo();

	//ģ�⴫�������ͳ��������
	transimitEnd();
}


void RRM_TDM_DRA::informationClean() {
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			_RSU->getTDM_DRAPoint()->m_AccessEventIdList[clusterIdx].first.clear();
			_RSU->getTDM_DRAPoint()->m_AccessEventIdList[clusterIdx].second.clear();
		}
	}
}


void RRM_TDM_DRA::groupSizeBasedTDM(bool t_ClusterFlag) {
	if (!t_ClusterFlag)return;
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		//�������������RSU����һ����ʱ
		if (_RSU->getSystemPoint()->getGTTPoint()->m_VeUEIdList.size() == 0) {
			/*-----------------------ATTENTION-----------------------
			* ����ֵΪ(0,-1,0)�ᵼ�»�ȡ��ǰ�ر��ʧ�ܣ����������ط���Ҫ����
			* ���ֱ�Ӹ�ÿ���ض���ֵΪ�������䣬����Ҳû���κ����ã����������������
			*------------------------ATTENTION-----------------------*/
			int tempNTTI = s_NTTI;//��֪��Ϊɶ��ֱ���������ʽ������s_NTTI����g++/gcc��������ӻ����s_NTTIδ��������
			_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum, tuple<int, int, int>(0, tempNTTI - 1, tempNTTI));
			continue;
		}

		//��ʼ��
		_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		//����ÿ��TTIʱ϶��Ӧ��VeUE��Ŀ(double)��!!!������!!��
		double VeUESizePerTTI = static_cast<double>(_RSU->getSystemPoint()->getGTTPoint()->m_VeUEIdList.size()) / static_cast<double>(s_NTTI);

		//clusterSize�洢ÿ���ص�VeUE��Ŀ(double)��!!!������!!��
		vector<double> clusterSize(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum, 0);

		//��ʼ��clusterSize
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++)
			clusterSize[clusterIdx] = static_cast<double>(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx].size());

		//���ȸ�������һ�����Ĵط���һ��TTI
		int basicNTTI = 0;
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			//����ô���������һ��VeUE��ֱ�ӷ����һ����λ��ʱ����Դ
			if (_RSU->getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx].size() != 0) {
				get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) = 1;
				clusterSize[clusterIdx] -= VeUESizePerTTI;
				basicNTTI++;
			}
		}

		//���˸���Ϊ�յĴط����һ��TTI�⣬ʣ��ɷ����TTI����
		int remainNTTI = s_NTTI - basicNTTI;

		//����ʣ�µ���Դ�飬ѭ������һʱ϶�������ǰ������ߵĴأ�����֮�󣬸��Ķ�Ӧ�ı�������ȥ��ʱ϶��Ӧ��VeUE����
		while (remainNTTI > 0) {
			int dex = getMaxIndex(clusterSize);
			if (dex == -1) throw logic_error("������û�з����ʱ����Դ������ÿ�����ڵ�VeUE�Ѿ�Ϊ����");
			get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[dex])++;
			remainNTTI--;
			clusterSize[dex] -= VeUESizePerTTI;
		}

		//��ʼ�������䷶Χ��������
		get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[0]) = 0;
		get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[0]) = get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[0]) + get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[0]) - 1;
		for (int clusterIdx = 1; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) = get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx - 1]) + 1;
			get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) = get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) + get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]) - 1;
		}


		//����������д���RSU�ڵ�ÿһ������
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU->getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId]->getTDM_DRAPoint()->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]), get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]));
		}
	}
	writeClusterPerformInfo();
}


void RRM_TDM_DRA::uniformTDM(bool t_ClusterFlag) {
	if (!t_ClusterFlag)return;
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		//��ʼ��
		_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo = vector<tuple<int, int, int>>(_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum, tuple<int, int, int>(0, 0, 0));

		int equalTimeLength = s_NTTI / _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum;

		int lastClusterLength = s_NTTI - equalTimeLength*_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum + equalTimeLength;

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx] = tuple<int, int, int>(equalTimeLength*clusterIdx, equalTimeLength*(clusterIdx + 1) - 1, equalTimeLength);
		}

		//�޸����һ���ص�ʱ�򳤶ȣ���Ϊƽ���س����ܱ����������ˣ���ˣ�ƽ���س���*�������������ܵ���ʱ�䣬��˽������������һ����
		get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum - 1]) = s_NTTI - 1;
		get<2>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[_RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum - 1]) = lastClusterLength;



		//����������д���RSU�ڵ�ÿһ������
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; ++clusterIdx) {
			for (int VeUEId : _RSU->getSystemPoint()->getGTTPoint()->m_ClusterVeUEIdList[clusterIdx])
				m_VeUEAry[VeUEId]->getTDM_DRAPoint()->m_ScheduleInterval = tuple<int, int>(get<0>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]), get<1>(_RSU->getTDM_DRAPoint()->m_ClusterTDRInfo[clusterIdx]));
		}
	}
	writeClusterPerformInfo();
}


void RRM_TDM_DRA::updateAccessEventIdList(bool t_ClusterFlag) {
	//���ȣ�����System������¼���������
	processEventList();
	//��Σ������ǰTTI������λ�ø��£���Ҫ������ȱ�
	if (t_ClusterFlag) {
		//����RSU����ĵ�������
		processScheduleInfoTableWhenLocationUpdate();

		//����RSU����ĵȴ�����
		processWaitEventIdListWhenLocationUpdate();

		//����System������л�����
		processSwitchListWhenLocationUpdate();
	}
	//����ɵȴ������ɽ����
	processWaitEventIdList();
}


void RRM_TDM_DRA::processEventList() {
	for (int eventId : getContext()->m_TMCPoint->m_EventTTIList[getContext()->m_TTI]) {
		Event &event = getContext()->m_TMCPoint->m_EventVec[eventId];
		int VeUEId = event.getVeUEId();
		int RSUId = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId;
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		int clusterIdx = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx;

		//�����¼�ѹ������¼��ȴ�����
		bool isEmergency = event.getMessageType() == EMERGENCY;
		_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//������־
		getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, EVENT_TO_WAIT, -1, -1, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Trigger");
		writeTTILogInfo(getContext()->m_TTI, EVENT_TO_WAIT, eventId, -1, -1, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Trigger");
	}
}


void RRM_TDM_DRA::processScheduleInfoTableWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		//��ʼ���� m_ScheduleInfoTable
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {
				if (_RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[clusterIdx][patternIdx] == nullptr) continue;

				int eventId = _RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[clusterIdx][patternIdx]->eventId;
				int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();
				//��VeUE���ڵ�ǰRSU�У�Ӧ����ѹ��System������л�����
				if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId != _RSU->getSystemPoint()->getGTTPoint()->m_RSUId) {
					//ѹ��Switch����
					_RSU->getTDM_DRAPoint()->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//��ʣ�����bit����
					getContext()->m_TMCPoint->m_EventVec[eventId].reset();

					//���ͷŸõ�����Ϣ����Դ
					Delete::safeDelete(_RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[clusterIdx][patternIdx]);

					//�ͷ�Pattern��Դ
					_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					//������־
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, SCHEDULETABLE_TO_SWITCH, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, SCHEDULETABLE_TO_SWITCH, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "LocationUpdate");
				}
				else {
					//RSU�ڲ������˴��л�������ӵ��ȱ���ȡ����ѹ��ȴ�����
					if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx != clusterIdx) {
						//ѹ���RSU�ĵȴ�����
						bool isEmergency = getContext()->m_TMCPoint->m_EventVec[eventId].getMessageType() == EMERGENCY;
						_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(isEmergency, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, eventId);

						//���ͷŸõ�����Ϣ����Դ
						Delete::safeDelete(_RSU->getTDM_DRAPoint()->m_ScheduleInfoTable[clusterIdx][patternIdx]);

						//�ͷ�Pattern��Դ
						_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

						//������־
						getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, SCHEDULETABLE_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
						writeTTILogInfo(getContext()->m_TTI, SCHEDULETABLE_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
					}
				}
			}
		}
	}
}


void RRM_TDM_DRA::processWaitEventIdListWhenLocationUpdate() {
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		//��ʼ���� m_WaitEventIdList

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			/*  EMERGENCY  */
			list<int>::iterator it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.begin();
			while (it != _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.end()) {
				int eventId = *it;
				int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();
				//��VeUE�Ѿ����ڸ�RSU��Χ��
				if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId != _RSU->getSystemPoint()->getGTTPoint()->m_RSUId) {

					//������ӵ�System�����RSU�л�������
					_RSU->getTDM_DRAPoint()->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//����ӵȴ�������ɾ��
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.erase(it);

					//��ʣ�����bit����
					getContext()->m_TMCPoint->m_EventVec[eventId].reset();

					//������־
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_SWITCH, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_SWITCH, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				//��Ȼ���ڵ�ǰRSU��Χ�ڣ���λ�ڲ�ͬ�Ĵ�
				else if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx != clusterIdx) {

					//������ӵ����ڴصĵȴ�����
					_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(true, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, eventId);

					//����ӵȴ������еĵ�ǰ��ɾ��
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.erase(it);

					//������־
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
				}
				else {
					it++;
					continue; //�������ڵ�ǰRSU�ĵȴ�����
				}
			}
			/*  EMERGENCY  */

			it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.begin();
			while (it != _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.end()) {
				int eventId = *it;
				int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();
				//��VeUE�Ѿ����ڸ�RSU��Χ��
				if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_RSUId != _RSU->getSystemPoint()->getGTTPoint()->m_RSUId) {

					//������ӵ�System�����RSU�л�������
					_RSU->getTDM_DRAPoint()->pushToSwitchEventIdList(m_SwitchEventIdList, eventId);

					//����ӵȴ�������ɾ��
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.erase(it);

					//��ʣ�����bit����
					getContext()->m_TMCPoint->m_EventVec[eventId].reset();

					//������־
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_SWITCH, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_SWITCH, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "LocationUpdate");
				}
				//��Ȼ���ڵ�ǰRSU��Χ�ڣ���λ�ڲ�ͬ�Ĵ�
				else if (m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx != clusterIdx) {

					//������ӵ����ڴصĵȴ�����
					_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(false, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, eventId);

					//����ӵȴ������еĵ�ǰ��ɾ��
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.erase(it);

					//������־
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_ClusterIdx, -1, "LocationUpdate");
				}
				else {
					it++;
					continue; //�������ڵ�ǰRSU�ĵȴ�����
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

		//ת��ȴ�����
		bool isEmergency = getContext()->m_TMCPoint->m_EventVec[eventId].getMessageType() == EMERGENCY;
		_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(isEmergency, clusterIdx, eventId);

		//��Switch���н���ɾ��
		it = m_SwitchEventIdList.erase(it);

		//������־
		getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, SWITCH_TO_WAIT, -1, -1, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "LocationUpdate");
		writeTTILogInfo(getContext()->m_TTI, SWITCH_TO_WAIT, eventId, -1, -1, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "LocationUpdate");
	}
}


void RRM_TDM_DRA::processWaitEventIdList() {
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];
		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			list<int>::iterator it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.begin();
			while (it != _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.end()) {
				int eventId = *it;
				//������¼�����Ҫ�˱ܣ���ת������
				if (getContext()->m_TMCPoint->m_EventVec[eventId].tryAcccess()) {

					//ѹ���������
					_RSU->getTDM_DRAPoint()->pushToAccessEventIdList(true, clusterIdx, eventId);

					//������־
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_ACCESS, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");
					writeTTILogInfo(getContext()->m_TTI, WAIT_TO_ACCESS, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");

					//�����¼��ӵȴ�����ɾ��
					it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first.erase(it);
				}
				else {
					//������־
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
			//������¼�����Ҫ�˱ܣ���ת������
			if (getContext()->m_TMCPoint->m_EventVec[eventId].tryAcccess()) {

				//ѹ���������
				_RSU->getTDM_DRAPoint()->pushToAccessEventIdList(false, clusterIdx, eventId);

				//������־
				getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WAIT_TO_ACCESS, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");
				writeTTILogInfo(getContext()->m_TTI, WAIT_TO_ACCESS, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "CanAccess");

				//�����¼��ӵȴ�����ɾ��
				it = _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second.erase(it);
			}
			else {
				//������־
				getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, WITHDRAWING, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "Withdraw");
				writeTTILogInfo(getContext()->m_TTI, WITHDRAWING, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, -1, -1, -1, "Withdraw");

				it++;
			}
		}
	}
}


void RRM_TDM_DRA::selectRBBasedOnP123() {
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			vector<int> curAvaliableEmergencyPatternIdx;//��ǰ�ؿ��õ�EmergencyPattern���

			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
				if (_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliableEmergencyPatternIdx.push_back(patternIdx);
				}
			}

			for (int eventId : _RSU->getTDM_DRAPoint()->m_AccessEventIdList[clusterIdx].first) {
				int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();

				//Ϊ��ǰ�û��ڿ��õ�EmergencyPattern�������ѡ��һ����ÿ���û��������ѡ�����EmergencyPattern��
				int patternIdx = m_VeUEAry[VeUEId]->getTDM_DRAPoint()->selectRBBasedOnP2(curAvaliableEmergencyPatternIdx);

				//�޶�ӦPattern���͵�pattern��Դ����
				if (patternIdx == -1) {
					//������־
					getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, ACCESS_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");
					writeTTILogInfo(getContext()->m_TTI, ACCESS_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");

					continue;
				}
				//����Դ���Ϊռ��
				_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

				//��������Ϣѹ��m_EmergencyTransimitEventIdList��
				_RSU->getTDM_DRAPoint()->pushToTransimitScheduleInfoList(new RRM_RSU::ScheduleInfo(eventId, VeUEId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx));

			}

		}
		/*  EMERGENCY  */

		//��ʼ�����Emergency���¼�
		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);

		/*
		* ��ǰTTI���õ�Pattern����
		* �±�����Pattern����ı�ţ�����Emergency��һ���ǿյģ����Ǳ�����������Ų���ƫ��
		* ÿ���ڲ�vector���������Pattern���õ�Pattern���(����Pattern���)
		*/
		vector<vector<int>> curAvaliablePatternIdx(s_PATTERN_TYPE_NUM);

		for (int patternTypeIdx = 1; patternTypeIdx < s_PATTERN_TYPE_NUM; patternTypeIdx++) {
			for (int patternIdx = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternTypeIdx].first; patternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternTypeIdx].second; patternIdx++) {
				if (_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx]) {
					curAvaliablePatternIdx[patternTypeIdx].push_back(patternIdx);
				}
			}
		}

		for (int eventId : _RSU->getTDM_DRAPoint()->m_AccessEventIdList[clusterIdx].second) {
			int VeUEId = getContext()->m_TMCPoint->m_EventVec[eventId].getVeUEId();

			//Ϊ��ǰ�û��ڿ��õĶ�Ӧ���¼����͵�Pattern�������ѡ��һ����ÿ���û��������ѡ�����Pattern��
			MessageType messageType = getContext()->m_TMCPoint->m_EventVec[eventId].getMessageType();
			int patternIdx = m_VeUEAry[VeUEId]->getTDM_DRAPoint()->selectRBBasedOnP2(curAvaliablePatternIdx[messageType]);

			//���û��������Ϣ����û��patternʣ����
			if (patternIdx == -1) {
				//������־
				getContext()->m_TMCPoint->m_EventVec[eventId].addEventLog(getContext()->m_TTI, ACCESS_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");
				writeTTILogInfo(getContext()->m_TTI, ACCESS_TO_WAIT, eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "AllBusy");

				continue;
			}

			//����Դ���Ϊռ��
			_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = false;

			//��������Ϣѹ��m_TransimitEventIdList��
			_RSU->getTDM_DRAPoint()->pushToTransimitScheduleInfoList(new RRM_RSU::ScheduleInfo(eventId, VeUEId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx));

		}
		//�����ȱ��е�ǰ���Լ���������û�ѹ�봫��������(���������ͷǽ���)
		_RSU->getTDM_DRAPoint()->pullFromScheduleInfoTable(getContext()->m_TTI);
	}
}


void RRM_TDM_DRA::delaystatistics() {
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			//����ȴ�����
			for (int eventId : _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].first)
				getContext()->m_TMCPoint->m_EventVec[eventId].increaseQueueDelay();
			//����˿����ڽ�Ҫ����Ĵ�������
			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++)
				for (RRM_RSU::ScheduleInfo* &p : _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
					getContext()->m_TMCPoint->m_EventVec[p->eventId].increaseSendDelay();
		}
		/*  EMERGENCY  */


		//����ȴ�����
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int eventId : _RSU->getTDM_DRAPoint()->m_WaitEventIdList[clusterIdx].second)
				getContext()->m_TMCPoint->m_EventVec[eventId].increaseQueueDelay();
		}

		//����˿����ڽ�Ҫ����Ĵ�������
		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);
		for (int patternIdx = s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {
			for (RRM_RSU::ScheduleInfo* &p : _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx])
				getContext()->m_TMCPoint->m_EventVec[p->eventId].increaseSendDelay();
		}

		//����˿�λ�ڵ��ȱ��У��������ڵ�ǰ�ص��¼�
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
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
				list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
					for (RRM_RSU::ScheduleInfo* &info : lst) {
						//���Ƚ��¼�ѹ��ȴ��б�
						_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(true, clusterIdx, info->eventId);

						//��ͻ������¼���״̬
						getContext()->m_TMCPoint->m_EventVec[info->eventId].conflict();

						//������־
						getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Conflict");
						writeTTILogInfo(getContext()->m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Conflict");

						//�ͷŵ�����Ϣ������ڴ���Դ
						Delete::safeDelete(info);
					}

					//�ͷ�Pattern��Դ
					_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

					lst.clear();
				}
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);
		for (int patternIdx = s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {

			list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
			if (lst.size() > 1) {//����һ��VeUE�ڵ�ǰTTI����Pattern�ϴ��䣬�������˳�ͻ��������ӵ��ȴ��б�
				for (RRM_RSU::ScheduleInfo* &info : lst) {
					//���Ƚ��¼�ѹ��ȴ��б�
					_RSU->getTDM_DRAPoint()->pushToWaitEventIdList(false, clusterIdx, info->eventId);

					//��ͻ������¼���״̬
					getContext()->m_TMCPoint->m_EventVec[info->eventId].conflict();

					//������־
					getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMIT_TO_WAIT, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Conflict");
					writeTTILogInfo(getContext()->m_TTI, TRANSIMIT_TO_WAIT, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, -1, "Conflict");

					//�ͷŵ�����Ϣ������ڴ���Դ
					Delete::safeDelete(info);
				}
				//�ͷ�Pattern��Դ
				_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;

				lst.clear();
			}
		}
	}
}


void RRM_TDM_DRA::transimitPreparation() {
	//���������һ�θ�����Ϣ
	for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++)
		for (int patternIdx = 0; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++)
			m_InterferenceVec[VeUEId][patternIdx].clear();

	//ͳ�Ʊ��εĸ�����Ϣ
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {
				list<RRM_RSU::ScheduleInfo*> &curList = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (curList.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
					RRM_RSU::ScheduleInfo *curInfo = *curList.begin();
					int curVeUEId = curInfo->VeUEId;
					for (int otherClusterIdx = 0; otherClusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; otherClusterIdx++) {
						if (otherClusterIdx == clusterIdx)continue;
						list<RRM_RSU::ScheduleInfo*> &otherList = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[otherClusterIdx][patternIdx];
						if (otherList.size() == 1) {//�������и�pattern���г����ڴ��䣬��ô���ó�����Ϊ���ų���
							RRM_RSU::ScheduleInfo *otherInfo = *otherList.begin();
							int otherVeUEId = otherInfo->VeUEId;
							m_InterferenceVec[curVeUEId][patternIdx].push_back(otherVeUEId);
						}
					}
				}
			}
		}
	}

	//����ÿ�����ĸ��ų����б�	
	for (int patternIdx = 0; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {
		for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++) {
			list<int>& interList = m_InterferenceVec[VeUEId][patternIdx];

			m_VeUEAry[VeUEId]->m_InterferenceVeUENum[patternIdx] = (int)interList.size();//д�������Ŀ

			m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx].assign(interList.begin(), interList.end());//д����ų���ID

			/*if (m_VeUEAry[VeUEId]->m_InterferenceVeUENum[patternIdx]>0) {
				g_FileTemp << "VeUEId: " << VeUEId << " [";
				for (auto c : m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx])
					g_FileTemp << c << ", ";
				g_FileTemp << " ]" << endl;
			}*/
		}
	}

	//����������˵�Ԫ���������Ӧ����
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
	WT* copyWTPoint = getContext()->m_WTPoint->getCopy();//����ÿ���̵߳ĸ�ģ����в�ͬ��״̬���޷����������������ģ�����ڱ��μ���
	for (int RSUId = t_FromRSUId; RSUId <= t_ToRSUId; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
				list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {
					RRM_RSU::ScheduleInfo *info = *lst.begin();
					int VeUEId = info->VeUEId;

					//����SINR����ȡ���Ʊ��뷽ʽ
					pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(getContext()->m_TMCPoint->m_EventVec[info->eventId].getMessageType(), patternIdx);
					//g_FileTemp << "Emergency PatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << endl;

					double factor = m_VeUEAry[VeUEId]->m_ModulationType * m_VeUEAry[VeUEId]->m_CodeRate;

					//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
					int maxEquivalentBitNum = (int)((double)(s_RB_NUM_PER_PATTERN_TYPE[EMERGENCY] * s_BIT_NUM_PER_RB)* factor);

					//����SINR
					double curSINR = 0;
					if (m_VeUEAry[VeUEId]->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId]->isAlreadyCalculateSINR(patternIdx)) {//���Ʊ��뷽ʽ��Ҫ����ʱ
						curSINR = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
						m_VeUEAry[VeUEId]->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx];
						m_VeUEAry[VeUEId]->m_PreSINR[patternIdx] = curSINR;
					}
					else
						curSINR = m_VeUEAry[VeUEId]->m_PreSINR[patternIdx];

					//��¼������Ϣ
					double tmpDistance = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_Distance[RSUId];
					if (curSINR < s_DROP_SINR_BOUNDARY) {
						//��¼����
						getContext()->m_TMCPoint->m_EventVec[info->eventId].packetLoss(tmpDistance);
					}
					info->transimitBitNum = maxEquivalentBitNum;
					info->currentPackageIdx = getContext()->m_TMCPoint->m_EventVec[info->eventId].getCurrentPackageIdx();
					info->remainBitNum = getContext()->m_TMCPoint->m_EventVec[info->eventId].getRemainBitNum();

					//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit��������������Ϣ״̬
					int realEquivalentBitNum = getContext()->m_TMCPoint->m_EventVec[info->eventId].transimit(maxEquivalentBitNum, tmpDistance);

					//�ۼ�������
					getContext()->m_TMCPoint->m_TTIRSUThroughput[getContext()->m_TTI][_RSU->getSystemPoint()->getGTTPoint()->m_RSUId] += realEquivalentBitNum;

					//������־
					getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMITTING, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
					writeTTILogInfo(getContext()->m_TTI, TRANSIMITTING, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
				}
			}
		}
		/*  EMERGENCY  */


		int clusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);
		for (int patternIdx = s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx < s_TOTAL_PATTERN_NUM; patternIdx++) {

			list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RRM_RSU::ScheduleInfo *info = *lst.begin();
				int VeUEId = info->VeUEId;

				int patternType = getPatternType(patternIdx);

				//����SINR����ȡ���Ʊ��뷽ʽ
				pair<int, int> subCarrierIdxRange = getOccupiedSubCarrierRange(getContext()->m_TMCPoint->m_EventVec[info->eventId].getMessageType(), patternIdx);
				//g_FileTemp << "NonEmergencyPatternIdx = " << patternIdx << "  [" << subCarrierIdxRange.first << " , " << subCarrierIdxRange.second << " ]  " << ((patternType == 0) ? "Emergency" : (patternType == 1 ? "Period" : "Data")) << endl;

				double factor = m_VeUEAry[VeUEId]->m_ModulationType * m_VeUEAry[VeUEId]->m_CodeRate;

				//�ñ��뷽ʽ�£���Pattern��һ��TTI���ɴ������Ч��Ϣbit����
				int maxEquivalentBitNum = (int)((double)(s_RB_NUM_PER_PATTERN_TYPE[patternType] * s_BIT_NUM_PER_RB)* factor);

				//����SINR
				double curSINR = 0;
				if (m_VeUEAry[VeUEId]->isNeedRecalculateSINR(patternIdx) || !m_VeUEAry[VeUEId]->isAlreadyCalculateSINR(patternIdx)) {//���Ʊ��뷽ʽ��Ҫ����ʱ
					curSINR = copyWTPoint->SINRCalculate(info->VeUEId, subCarrierIdxRange.first, subCarrierIdxRange.second, patternIdx);
					m_VeUEAry[VeUEId]->m_PreInterferenceVeUEIdVec[patternIdx] = m_VeUEAry[VeUEId]->m_InterferenceVeUEIdVec[patternIdx];
					m_VeUEAry[VeUEId]->m_PreSINR[patternIdx] = curSINR;
				}
				else
					curSINR = m_VeUEAry[VeUEId]->m_PreSINR[patternIdx];

				//��¼������Ϣ
				double tmpDistance = m_VeUEAry[VeUEId]->getSystemPoint()->getGTTPoint()->m_Distance[RSUId];
				if (curSINR < s_DROP_SINR_BOUNDARY) {
					//��¼����
					getContext()->m_TMCPoint->m_EventVec[info->eventId].packetLoss(tmpDistance);
				}
				info->transimitBitNum = maxEquivalentBitNum;
				info->currentPackageIdx = getContext()->m_TMCPoint->m_EventVec[info->eventId].getCurrentPackageIdx();
				info->remainBitNum = getContext()->m_TMCPoint->m_EventVec[info->eventId].getRemainBitNum();

				//�ñ��뷽ʽ�£���Pattern��һ��TTI�����ʵ�ʵ���Ч��Ϣbit��������������Ϣ״̬
				int realEquivalentBitNum = getContext()->m_TMCPoint->m_EventVec[info->eventId].transimit(maxEquivalentBitNum, tmpDistance);

				//�ۼ�������
				getContext()->m_TMCPoint->m_TTIRSUThroughput[getContext()->m_TTI][_RSU->getSystemPoint()->getGTTPoint()->m_RSUId] += realEquivalentBitNum;

				//������־
				getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, TRANSIMITTING, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
				writeTTILogInfo(getContext()->m_TTI, TRANSIMITTING, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Transimit");
			}
		}
	}
	copyWTPoint->freeCopy();//getCopy��ͨ��new�����ģ���������ͷ���Դ
}

void RRM_TDM_DRA::writeScheduleInfo() {
	if (!getContext()->m_Config.scheduleLogIsOn)return;
	m_FileScheduleInfo << "[ TTI = " << left << setw(3) << getContext()->m_TTI << "]" << endl;
	m_FileScheduleInfo << "{" << endl;
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		int NonEmergencyClusterIdx = _RSU->getTDM_DRAPoint()->getClusterIdx(getContext()->m_TTI);

		m_FileScheduleInfo << "    RSU[" << _RSU->getSystemPoint()->getGTTPoint()->m_RSUId << "][TTI = " << getContext()->m_TTI << "]" << endl;
		m_FileScheduleInfo << "    {" << endl;
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			m_FileScheduleInfo << "        Cluster[" << clusterIdx << "]" << endl;
			m_FileScheduleInfo << "        {" << endl;

			m_FileScheduleInfo << "            EMERGENCY:" << endl;
			for (int patternIdx = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[EMERGENCY].first; patternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[EMERGENCY].second; patternIdx++) {
				bool isAvaliable = _RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx];
				m_FileScheduleInfo << "                Pattern[" << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
				if (!isAvaliable) {
					RRM_RSU::ScheduleInfo *info = *(_RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					m_FileScheduleInfo << info->toScheduleString(5) << endl;
				}
			}
			if (clusterIdx != NonEmergencyClusterIdx)continue;
			m_FileScheduleInfo << "            PERIOD:" << endl;
			for (int patternIdx = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[PERIOD].first; patternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[PERIOD].second; patternIdx++) {
				bool isAvaliable = _RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx];
				m_FileScheduleInfo << "                Pattern[" << left << setw(3) << patternIdx << "] : " << (isAvaliable ? "Available" : "Unavailable") << endl;
				if (!isAvaliable) {
					RRM_RSU::ScheduleInfo *info = *(_RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx].begin());
					m_FileScheduleInfo << info->toScheduleString(5) << endl;
				}
			}
			m_FileScheduleInfo << "            DATA:" << endl;
			for (int patternIdx = s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[DATA].first; patternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[DATA].second; patternIdx++) {
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
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
		RRM_RSU *_RSU = m_RSUAry[RSUId];

		/*  EMERGENCY  */
		for (int clusterIdx = 0; clusterIdx < _RSU->getSystemPoint()->getGTTPoint()->m_ClusterNum; clusterIdx++) {
			for (int patternIdx = 0; patternIdx < s_PATTERN_NUM_PER_PATTERN_TYPE[EMERGENCY]; patternIdx++) {
				list<RRM_RSU::ScheduleInfo*> &lst = _RSU->getTDM_DRAPoint()->m_TransimitScheduleInfoList[clusterIdx][patternIdx];
				if (lst.size() == 1) {
					RRM_RSU::ScheduleInfo* &info = *lst.begin();
					//�Ѿ�������ϣ�����Դ�ͷ�
					if (getContext()->m_TMCPoint->m_EventVec[info->eventId].isFinished()) {

						//������־
						getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, SUCCEED, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
						writeTTILogInfo(getContext()->m_TTI, SUCCEED, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

						//�ͷŵ�����Ϣ������ڴ���Դ
						Delete::safeDelete(info);

						//�ͷ�Pattern��Դ
						_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;
					}
					else {//��δ�������
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
			if (lst.size() == 1) {//ֻ��һ���û��ڴ��䣬���û�����ȷ�Ĵ����������ݣ����뿪��֮ǰ��
				RRM_RSU::ScheduleInfo* &info = *lst.begin();
				//˵���������Ѿ��������
				if (getContext()->m_TMCPoint->m_EventVec[info->eventId].isFinished()) {

					//������־								
					getContext()->m_TMCPoint->m_EventVec[info->eventId].addEventLog(getContext()->m_TTI, SUCCEED, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");
					writeTTILogInfo(getContext()->m_TTI, SUCCEED, info->eventId, _RSU->getSystemPoint()->getGTTPoint()->m_RSUId, clusterIdx, patternIdx, -1, -1, -1, "Succeed");

					//�ͷŵ�����Ϣ������ڴ���Դ
					Delete::safeDelete(info);

					//�ͷ�Pattern��Դ
					_RSU->getTDM_DRAPoint()->m_PatternIsAvailable[clusterIdx][patternIdx] = true;
				}
				else {//��������δ���꣬����ѹ��m_ScheduleInfoTable
					_RSU->getTDM_DRAPoint()->pushToScheduleInfoTable(info);
					info = nullptr;
				}
			}
			//������󣬽���pattern�ϵ�������գ���ʱҪ��������ǿգ�Ҫ������nullptrָ�룩
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

	//��ӡVeUE��Ϣ
	m_FileClasterPerformInfo << "    VUE Info: " << endl;
	m_FileClasterPerformInfo << "    {" << endl;
	for (int VeUEId = 0; VeUEId < GTT::s_VeUE_NUM; VeUEId++) {
		RRM_VeUE &_VeUE = *(m_VeUEAry[VeUEId]);
		m_FileClasterPerformInfo << _VeUE.getTDM_DRAPoint()->toString(2) << endl;
	}
	m_FileClasterPerformInfo << "    }\n" << endl;

	////��ӡ��վ��Ϣ
	//out << "    eNB Info: " << endl;
	//out << "    {" << endl;
	//for (int eNBId = 0; eNBId < m_Config.eNBNum; eNBId++) {
	//	ceNB &_eNB = m_eNBAry[eNBId];
	//	out << _eNB.toString(2) << endl;
	//}
	//out << "    }\n" << endl;

	//��ӡRSU��Ϣ
	m_FileClasterPerformInfo << "    RSU Info: " << endl;
	m_FileClasterPerformInfo << "    {" << endl;
	for (int RSUId = 0; RSUId < GTT::s_RSU_NUM; RSUId++) {
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
	//patternIdxָ�����¼����͵�Pattern�ľ�����ţ���0��ʼ��ţ�����Emergency
	for (int patternType = 0; patternType < s_PATTERN_TYPE_NUM; patternType++) {
		if (t_PatternIdx >= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternType].first && t_PatternIdx <= s_PATTERN_TYPE_PATTERN_INDEX_INTERVAL[patternType].second)
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
