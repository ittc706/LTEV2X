#include <tuple>
#include "GeographyAndMobility.h"
#include "System.h"
#include "Enumeration.h"

using namespace std;

void cSystem::channelGeneration(){
	//RSU.m_VeUEIdList����freshLoc���������ɵģ������Ҫ�ڸ���λ��ǰ�������б�
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		_RSU.m_VeUEIdList.clear();
		for (int clusterIdx = 0;clusterIdx < _RSU.m_DRAClusterNum;clusterIdx++) {
			_RSU.m_DRAClusterVeUEIdList[clusterIdx].clear();
		}
	}
	//ͬʱҲ���eNB.m_VeUEIdList
	for (int eNBId = 0;eNBId < m_Config.eNBNum;eNBId++)
		m_eNBAry[eNBId].m_VeUEIdList.clear();
	
	//�˶�ģ��
	freshLoc();

	//�����º��RSU.m_VeUEIdListѹ���Ӧ�Ĵ���
	for (int RSUId = 0;RSUId < m_Config.RSUNum;RSUId++) {
		cRSU &_RSU = m_RSUAry[RSUId];
		for (int VeUEId : _RSU.m_VeUEIdList) {
			int clusterIdx = m_VeUEAry[VeUEId].m_ClusterIdx;
			_RSU.m_DRAClusterVeUEIdList[clusterIdx].push_back(VeUEId);
		}
	}
	
	//��¼������ÿ������λ����־
	for(int VeUEId=0;VeUEId<m_Config.VeUENum;VeUEId++)
		m_VeUEAry[VeUEId].m_LocationUpdateLogInfoList.push_back(tuple<int, int>(m_VeUEAry[VeUEId].m_RSUId, m_VeUEAry[VeUEId].m_ClusterIdx));

	//UNDONE
	//���»�վ��VeUE����
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