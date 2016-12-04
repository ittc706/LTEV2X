#pragma once
#include<string>
#include<vector>
#include<list>
#include<tuple>

class RSU;
class RRM_TDM_DRA_RSU;
class RRM_ICC_DRA_RSU;
class RRM_RR_RSU;

class RRM_RSU {
	/*------------------�ڲ���------------------*/
public:
	class ScheduleInfo {
		/*--------��--------*/
	public:
		/*
		* �¼����
		*/
		int eventId;

		/*
		* �������
		*/
		int VeUEId;

		/*
		* RSU���
		*/
		int RSUId;

		/*
		* �ر��
		*/
		int clusterIdx;

		/*
		* Ƶ�����
		*/
		int patternIdx;

		/*
		* ��ǰ��������ݰ��ı��
		* ֱ�Ӵ���Ϣ���ȡ
		*/
		int currentPackageIdx = -1;

		/*
		* ʣ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
		* ֱ�Ӵ���Ϣ���ȡ
		*/
		int remainBitNum = -1;

		/*
		* ��ǰ�����bit����(����ʵ�ʴ����bit���������ǵ�Ч����ʵ���ݵĴ�����������Ҫ��ȥ�ŵ����������bit)
		* ֱ�Ӵ���Ϣ���ȡ
		*/
		int transimitBitNum = -1;

		/*-------����-------*/
		/*
		* ���캯��
		*/
		ScheduleInfo() {}

		/*
		* ���캯��
		*/
		ScheduleInfo(int t_EventId, int t_VeUEId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx) {
			this->eventId = t_EventId;
			this->VeUEId = t_VeUEId;
			this->RSUId = t_RSUId;
			this->clusterIdx = t_ClusterIdx;
			this->patternIdx = t_PatternIdx;
		}

		/*
		* ���ɸ�ʽ���ַ���
		*/
		std::string toLogString();

		/*
		* ���ɱ�ʾ������Ϣ��string����
		* �����¼���Id��������Id���Լ�Ҫ������¼���ռ�õ�TTI����
		*/
		std::string toScheduleString(int t_NumTab);
	};

	/*------------------��------------------*/
private:
	/*
	* ָ�����ڲ�ͬ��ԪRSU���ݽ�����ϵͳ��RSU����
	*/
	RSU* m_This;

	/*------------------����------------------*/
public:
	/*
	* ��ʼ��
	* ���ֳ�Ա��Ҫ�ȵ�GTTģ���ʼ����Ϻ����˴ص��������ܽ��б���ԪRSU�ĳ�ʼ��
	*/
	virtual void initialize() = 0;

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	* ���ھ�̬����ΪRRM_RSU
	*/
	virtual RRM_TDM_DRA_RSU *const getTDM_DRAPoint() = 0;
	virtual RRM_ICC_DRA_RSU *const getICC_DRAPoint() = 0;
	virtual RRM_RR_RSU *const getRRPoint() = 0;

	/*
	* ȡ��ϵͳ��System��RSU��ָ��
	*/
	RSU* getSystemPoint() { return m_This; }

	/*
	* ����ϵͳ��System��RSU��ָ��
	*/
	void setSystemPoint(RSU* t_Point) { m_This = t_Point; }
};



class RRM_ICC_DRA_RSU :public RRM_RSU {
	/*------------------��------------------*/
public:
	/*
	* RSU����Ĵ������б�
	* ����±�Ϊ�ر��
	* �ڲ�list��ŵ��Ƕ�Ӧ�ص�VeUEId
	*/
	std::vector<std::list<int>> m_AccessEventIdList;

	/*
	* RSU����ĵȴ��б�
	* ����±�Ϊ�ر��
	* �ڲ�list��ŵ��Ƕ�Ӧ�ص�VeUEId
	* ����Դ�У�
	*		1���ִغ���System�����л�����ת���RSU����ĵȴ�����
	*		2���¼������е�ǰ���¼�����
	*		3��VeUE�ڴ�����Ϣ�󣬷�����ͻ�������ͻ��ת��ȴ�����
	*/
	std::vector<std::list<int>> m_WaitEventIdList;

	/*
	* Pattern�����Ƿ���õı��
	* ����±����ر��
	* �ڲ��±����Pattern���
	* ��"m_PatternIsAvailable[i][j]==true"�����i��Pattern��j����
	*/
	std::vector<std::vector<bool>> m_PatternIsAvailable;

	/*
	* ��ŵ��ȵ�����Ϣ
	* ����±����ر��
	* �ڲ��±����Pattern���
	*/
	std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;

	/*
	* ��ǰʱ�̵�ǰRSU�ڴ��ڴ���״̬�ĵ�����Ϣ����
	* ����±����ر��
	* �ڲ��±����Pattern���
	* ���ڲ���list���ڴ����ͻ
	*/
	std::vector<std::vector<std::list<ScheduleInfo*>>> m_TransimitScheduleInfoList;

	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	RRM_ICC_DRA_RSU();

	/*
	* ��ʼ��
	* ���ֳ�Ա��Ҫ�ȵ�GTTģ���ʼ����Ϻ����˴ص��������ܽ��б���ԪRSU�ĳ�ʼ��
	*/
	void initialize() override;

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ��AccessVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToAccessEventIdList(int t_ClusterIdx, int t_EventId);

	/*
	* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToWaitEventIdList(int t_ClusterIdx, int t_EventId);

	/*
	* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

	/*
	* ��TransimitScheduleInfo����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToTransmitScheduleInfoList(ScheduleInfo* t_Info);

	/*
	* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToScheduleInfoTable(ScheduleInfo* t_Info);

	/*
	* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
	*/
	void pullFromScheduleInfoTable(int t_TTI);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	RRM_TDM_DRA_RSU *const getTDM_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_ICC_DRA_RSU *const getICC_DRAPoint() override { return this; }
	RRM_RR_RSU *const getRRPoint() override { throw std::logic_error("RuntimeException"); }
};


class RRM_RR_RSU :public RRM_RSU {
	/*------------------��------------------*/
public:
	/*
	* RSU����Ľ����б�
	* ����±�Ϊ�ر��
	* �ڲ�list��ŵ��Ǵ��ڵȴ�����״̬��VeUEId
	*/
	std::vector<std::list<int>> m_AccessEventIdList;

	/*
	* RSU����ĵȴ��б�
	* ����±�Ϊ�ر��
	* �ڲ�list��ŵ��Ǵ��ڵȴ�����״̬��VeUEId
	* ����Դ�У�
	*		1���ִغ���System�����л�����ת���RSU����ĵȴ�����
	*		2���¼������е�ǰ���¼�������ת��ȴ�����
	*/
	std::vector<std::list<int>> m_WaitEventIdList;

	/*
	* ��ŵ��ȵ�����Ϣ�����ν��д���
	* ����±����ر��
	* �ڲ��±����Pattern���
	*/
	std::vector<std::vector<ScheduleInfo*>> m_TransimitScheduleInfoTable;

	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	RRM_RR_RSU();

	/*
	* ��ʼ��
	* ���ֳ�Ա��Ҫ�ȵ�GTTģ���ʼ����Ϻ����˴ص��������ܽ��б���ԪRSU�ĳ�ʼ��
	*/
	void initialize() override;

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ��AccessVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToAccessEventIdList(int t_ClusterIdx, int t_EventId);

	/*
	* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToWaitEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId);

	/*
	* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList);

	/*
	* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToTransimitScheduleInfoTable(ScheduleInfo* t_Info);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	RRM_TDM_DRA_RSU *const getTDM_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_ICC_DRA_RSU *const getICC_DRAPoint() override { throw std::logic_error("RuntimeException"); }
	RRM_RR_RSU *const getRRPoint() override { return this; }
};


class RRM_TDM_DRA_RSU :public RRM_RSU {
	/*------------------��------------------*/
public:
	/*
	* TDR:Time Domain Resource
	* �±����ر��
	* tuple�洢�ı����ĺ�������Ϊ���洢ÿ����������ʱ�������������˵㣬�Ҷ˵��Լ����䳤��
	*/
	std::vector<std::tuple<int, int, int>> m_ClusterTDRInfo;

	/*
	* Pattern�����Ƿ���õı��
	* ����±����ر��
	* �ڲ��±����Pattern���
	* ��"m_PatternIsAvailable[i][j]==true"�����i��Pattern��j����
	*/
	std::vector<std::vector<bool>> m_PatternIsAvailable;

	/*
	* �����б�
	* ��ŵ���VeUEId
	* ����±����ر��
	* �ڲ�first����Emergency��second�����Emergency
	*/
	std::vector<std::pair<std::list<int>, std::list<int>>> m_AccessEventIdList;

	/*
	* �ȴ��б�
	* ��ŵ���VeUEId
	* ����±����ر��
	* �ڲ�first����Emergency��second�����Emergency
	*/
	std::vector<std::pair<std::list<int>, std::list<int>>> m_WaitEventIdList;

	/*
	* ��ŵ��ȵ�����Ϣ(�Ѿ��ɹ����룬����δ������ϣ����䴫�����֮ǰ��һֱռ�ø���Դ��)
	* ����±����ر��
	* �ڲ��±����Pattern���
	*/
	std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;

	/*
	* ��ǰʱ�̵�ǰRSU�ڴ��ڴ���״̬�ĵ�����Ϣ����
	* ����±�Ϊ�ر��
	* �в��±����Pattern���
	* �ڲ���list���ڴ����ͻ������Ӧ�ض�ӦPattern�µĵ�ǰ���д�����¼��ĵ�����Ϣ
	* ���ڽ����¼������дض���Ч
	* ���ڷǽ����¼�������ǰʱ�̶�Ӧ�Ĵ���Ч
	*/
	std::vector<std::vector<std::list<ScheduleInfo*>>>  m_TransimitScheduleInfoList;

	/*------------------����------------------*/
public:
	/*
	* ���캯��
	*/
	RRM_TDM_DRA_RSU();

	/*
	* ��ʼ��
	* ���ֳ�Ա��Ҫ�ȵ�GTTģ���ʼ����Ϻ����˴ص��������ܽ��б���ԪRSU�ĳ�ʼ��
	*/
	void initialize() override;

	/*
	* ���ɸ�ʽ���ַ���
	*/
	std::string toString(int t_NumTab);

	/*
	* ���ص�ǰTTI���Խ����¼�����Ĵر��
	*/
	int getClusterIdx(int t_TTI);

	/*
	* ��AccessVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToAccessEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId);

	/*
	* ��WaitVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToWaitEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId);

	/*
	* ��SwitchVeUEIdList����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

	/*
	* ��TransimitScheduleInfo����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToTransimitScheduleInfoList(ScheduleInfo* t_Info);

	/*
	* ��ScheduleInfoTable����ӷ�װ���������ڲ鿴������ã����ڵ���
	*/
	void pushToScheduleInfoTable(ScheduleInfo* t_Info);

	/*
	* ��RSU�����ScheduleInfoTable�ĵ�����װ���������ڲ鿴������ã����ڵ���
	*/
	void pullFromScheduleInfoTable(int t_TTI);

	/*
	* ����ȡ��ָ��ʵ�����͵�ָ��
	*/
	RRM_TDM_DRA_RSU *const getTDM_DRAPoint()override { return this; }
	RRM_ICC_DRA_RSU *const getICC_DRAPoint() override { throw std::logic_error("RuntimeException"); }
	RRM_RR_RSU *const getRRPoint() override { throw std::logic_error("RuntimeException"); }
};
