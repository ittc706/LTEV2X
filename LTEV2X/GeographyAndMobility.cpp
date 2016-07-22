#include <tuple>
#include "GeographyAndMobility.h"
#include "System.h"
#include "Enumeration.h"

using namespace std;

void cSystem::channelGeneration(){
	//RSU.m_VeUEIdList是在freshLoc函数内生成的，因此需要在更新位置前清空这个列表
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_VeUEIdList.clear();
		for (int clusterIdx = 0;clusterIdx < _RSU.m_DRAClusterNum;clusterIdx++) {
			_RSU.m_DRAClusterVeUEIdList[clusterIdx].clear();
		}
	}
	//同时也清除eNB.m_VeUEIdList
	for (int eNBId = 0;eNBId < m_Config.eNBNum;eNBId++)
		m_eNBAry[eNBId].m_VeUEIdList.clear();
	
	//运动模型
	freshLoc();

	//将更新后的RSU.m_VeUEIdList压入对应的簇中
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		for (int VeUEId : _RSU.m_VeUEIdList) {
			int clusterIdx = m_VeUEAry[VeUEId].m_ClusterIdx;
			_RSU.m_DRAClusterVeUEIdList[clusterIdx].push_back(VeUEId);
		}
	}
	
	//记录并更新每辆车的位置日志
	for(int VeUEId=0;VeUEId<m_Config.VeUENum;VeUEId++)
		m_VeUEAry[VeUEId].m_LocationUpdateLogInfoList.push_back(tuple<int, int>(m_VeUEAry[VeUEId].m_RSUId, m_VeUEAry[VeUEId].m_ClusterIdx));

	//UNDONE
	//更新基站的VeUE容器
	for (int eNBId = 0;eNBId < m_Config.eNBNum;eNBId++) {
		ceNB &_eNB = m_eNBAry[eNBId];
		for (int RSUId : _eNB.m_RSUIdList) {
			for (int VeUEId : m_RSUAry[RSUId].m_VeUEIdList) {
				_eNB.m_VeUEIdList.push_back(VeUEId);
			}
		}
	}
	//UNDONE
}