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
	/*------------------内部类------------------*/
public:
	class ScheduleInfo {
		/*--------域--------*/
	public:
		/*
		* 事件编号
		*/
		int eventId;

		/*
		* 车辆编号
		*/
		int VeUEId;

		/*
		* RSU编号
		*/
		int RSUId;

		/*
		* 簇编号
		*/
		int clusterIdx;

		/*
		* 频域块编号
		*/
		int patternIdx;

		/*
		* 当前传输的数据包的编号
		* 直接从消息类读取
		*/
		int currentPackageIdx = -1;

		/*
		* 剩余待传bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
		* 直接从消息类读取
		*/
		int remainBitNum = -1;

		/*
		* 当前传输的bit数量(并非实际传输的bit数量，而是等效的真实数据的传输数量，需要除去信道编码的冗余bit)
		* 直接从消息类读取
		*/
		int transimitBitNum = -1;

		/*-------方法-------*/
		/*
		* 构造函数
		*/
		ScheduleInfo() {}

		/*
		* 构造函数
		*/
		ScheduleInfo(int t_EventId, int t_VeUEId, int t_RSUId, int t_ClusterIdx, int t_PatternIdx) {
			this->eventId = t_EventId;
			this->VeUEId = t_VeUEId;
			this->RSUId = t_RSUId;
			this->clusterIdx = t_ClusterIdx;
			this->patternIdx = t_PatternIdx;
		}

		/*
		* 生成格式化字符串
		*/
		std::string toLogString();

		/*
		* 生成表示调度信息的string对象
		* 包括事件的Id，车辆的Id，以及要传输该事件所占用的TTI区间
		*/
		std::string toScheduleString(int t_NumTab);
	};

	/*------------------域------------------*/
private:
	/*
	* 指向用于不同单元RSU数据交互的系统级RSU对象
	*/
	RSU* m_This;

	/*------------------方法------------------*/
public:
	/*
	* 初始化
	* 部分成员需要等到GTT模块初始化完毕后，有了簇的数量才能进行本单元RSU的初始化
	*/
	virtual void initialize() = 0;

	/*
	* 用于取得指向实际类型的指针
	* 由于静态类型为RRM_RSU
	*/
	virtual RRM_TDM_DRA_RSU *const getTDM_DRAPoint() = 0;
	virtual RRM_ICC_DRA_RSU *const getICC_DRAPoint() = 0;
	virtual RRM_RR_RSU *const getRRPoint() = 0;

	/*
	* 取得系统级System的RSU的指针
	*/
	RSU* getSystemPoint() { return m_This; }

	/*
	* 设置系统级System的RSU的指针
	*/
	void setSystemPoint(RSU* t_Point) { m_This = t_Point; }
};



class RRM_ICC_DRA_RSU :public RRM_RSU {
	/*------------------域------------------*/
public:
	/*
	* RSU级别的待接入列表
	* 外层下标为簇编号
	* 内层list存放的是对应簇的VeUEId
	*/
	std::vector<std::list<int>> m_AccessEventIdList;

	/*
	* RSU级别的等待列表
	* 外层下标为簇编号
	* 内层list存放的是对应簇的VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发
	*		3、VeUE在传输消息后，发生冲突，解决冲突后，转入等待链表
	*/
	std::vector<std::list<int>> m_WaitEventIdList;

	/*
	* Pattern块释是否可用的标记
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	* 若"m_PatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
	*/
	std::vector<std::vector<bool>> m_PatternIsAvailable;

	/*
	* 存放调度调度信息
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	*/
	std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;

	/*
	* 当前时刻当前RSU内处于传输状态的调度信息链表
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	* 最内层用list用于处理冲突
	*/
	std::vector<std::vector<std::list<ScheduleInfo*>>> m_TransimitScheduleInfoList;

	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	RRM_ICC_DRA_RSU();

	/*
	* 初始化
	* 部分成员需要等到GTT模块初始化完毕后，有了簇的数量才能进行本单元RSU的初始化
	*/
	void initialize() override;

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 将AccessVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToAccessEventIdList(int t_ClusterIdx, int t_EventId);

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToWaitEventIdList(int t_ClusterIdx, int t_EventId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

	/*
	* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToTransmitScheduleInfoList(ScheduleInfo* t_Info);

	/*
	* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToScheduleInfoTable(ScheduleInfo* t_Info);

	/*
	* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
	*/
	void pullFromScheduleInfoTable(int t_TTI);

	/*
	* 用于取得指向实际类型的指针
	*/
	RRM_TDM_DRA_RSU *const getTDM_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_ICC_DRA_RSU *const getICC_DRAPoint() override { return this; }
	RRM_RR_RSU *const getRRPoint() override { throw std::logic_error("RuntimeException"); }
};


class RRM_RR_RSU :public RRM_RSU {
	/*------------------域------------------*/
public:
	/*
	* RSU级别的接入列表
	* 外层下标为簇编号
	* 内层list存放的是处于等待接入状态的VeUEId
	*/
	std::vector<std::list<int>> m_AccessEventIdList;

	/*
	* RSU级别的等待列表
	* 外层下标为簇编号
	* 内层list存放的是处于等待接入状态的VeUEId
	* 其来源有：
	*		1、分簇后，由System级的切换链表转入该RSU级别的等待链表
	*		2、事件链表中当前的事件触发，转入等待链表
	*/
	std::vector<std::list<int>> m_WaitEventIdList;

	/*
	* 存放调度调度信息，本次进行传输
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	*/
	std::vector<std::vector<ScheduleInfo*>> m_TransimitScheduleInfoTable;

	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	RRM_RR_RSU();

	/*
	* 初始化
	* 部分成员需要等到GTT模块初始化完毕后，有了簇的数量才能进行本单元RSU的初始化
	*/
	void initialize() override;

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 将AccessVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToAccessEventIdList(int t_ClusterIdx, int t_EventId);

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToWaitEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToSwitchEventIdList(int t_EventId, std::list<int>& t_SwitchVeUEIdList);

	/*
	* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToTransimitScheduleInfoTable(ScheduleInfo* t_Info);

	/*
	* 用于取得指向实际类型的指针
	*/
	RRM_TDM_DRA_RSU *const getTDM_DRAPoint()override { throw std::logic_error("RuntimeException"); }
	RRM_ICC_DRA_RSU *const getICC_DRAPoint() override { throw std::logic_error("RuntimeException"); }
	RRM_RR_RSU *const getRRPoint() override { return this; }
};


class RRM_TDM_DRA_RSU :public RRM_RSU {
	/*------------------域------------------*/
public:
	/*
	* TDR:Time Domain Resource
	* 下标代表簇编号
	* tuple存储的变量的含义依次为：存储每个簇所分配时间数量区间的左端点，右端点以及区间长度
	*/
	std::vector<std::tuple<int, int, int>> m_ClusterTDRInfo;

	/*
	* Pattern块释是否可用的标记
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	* 若"m_PatternIsAvailable[i][j]==true"代表簇i的Pattern块j可用
	*/
	std::vector<std::vector<bool>> m_PatternIsAvailable;

	/*
	* 接入列表
	* 存放的是VeUEId
	* 外层下标代表簇编号
	* 内层first代表Emergency，second代表非Emergency
	*/
	std::vector<std::pair<std::list<int>, std::list<int>>> m_AccessEventIdList;

	/*
	* 等待列表
	* 存放的是VeUEId
	* 外层下标代表簇编号
	* 内层first代表Emergency，second代表非Emergency
	*/
	std::vector<std::pair<std::list<int>, std::list<int>>> m_WaitEventIdList;

	/*
	* 存放调度调度信息(已经成功接入，但尚未传输完毕，在其传输完毕之前会一直占用该资源块)
	* 外层下标代表簇编号
	* 内层下标代表Pattern编号
	*/
	std::vector<std::vector<ScheduleInfo*>> m_ScheduleInfoTable;

	/*
	* 当前时刻当前RSU内处于传输状态的调度信息链表
	* 外层下标为簇编号
	* 中层下标代表Pattern编号
	* 内层用list用于处理冲突，即对应簇对应Pattern下的当前进行传输的事件的调度信息
	* 对于紧急事件，所有簇都有效
	* 对于非紧急事件，仅当前时刻对应的簇有效
	*/
	std::vector<std::vector<std::list<ScheduleInfo*>>>  m_TransimitScheduleInfoList;

	/*------------------方法------------------*/
public:
	/*
	* 构造函数
	*/
	RRM_TDM_DRA_RSU();

	/*
	* 初始化
	* 部分成员需要等到GTT模块初始化完毕后，有了簇的数量才能进行本单元RSU的初始化
	*/
	void initialize() override;

	/*
	* 生成格式化字符串
	*/
	std::string toString(int t_NumTab);

	/*
	* 返回当前TTI可以进行事件传输的簇编号
	*/
	int getClusterIdx(int t_TTI);

	/*
	* 将AccessVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToAccessEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId);

	/*
	* 将WaitVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToWaitEventIdList(bool t_IsEmergency, int t_ClusterIdx, int t_EventId);

	/*
	* 将SwitchVeUEIdList的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToSwitchEventIdList(std::list<int>& t_SwitchVeUEIdList, int t_EventId);

	/*
	* 将TransimitScheduleInfo的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToTransimitScheduleInfoList(ScheduleInfo* t_Info);

	/*
	* 将ScheduleInfoTable的添加封装起来，便于查看哪里调用，利于调试
	*/
	void pushToScheduleInfoTable(ScheduleInfo* t_Info);

	/*
	* 将RSU级别的ScheduleInfoTable的弹出封装起来，便于查看哪里调用，利于调试
	*/
	void pullFromScheduleInfoTable(int t_TTI);

	/*
	* 用于取得指向实际类型的指针
	*/
	RRM_TDM_DRA_RSU *const getTDM_DRAPoint()override { return this; }
	RRM_ICC_DRA_RSU *const getICC_DRAPoint() override { throw std::logic_error("RuntimeException"); }
	RRM_RR_RSU *const getRRPoint() override { throw std::logic_error("RuntimeException"); }
};
