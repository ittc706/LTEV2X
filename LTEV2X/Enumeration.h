#pragma once


enum LocationType { //位置类型 
	Los, 
	Nlos, 
	None 
}; 

enum GTATMode {
	URBAN,      //城市
	HIGHSPEED   //高速
};

enum RRMMode {
	PF=1,       //基于比例公平的调度
	RR=2,       //Round-Robin,轮询
	TDM_DRA         //Distributed Resource Allocation,分布式
};


enum MessageType {
	EMERGENCY = 0,   //紧急信息（比如车祸信息，优先级较高）
	PERIOD = 1,  //周期性信息（包括VeUE的基本信息，车速，位置，方向等等）
	DATA = 2     //数据业务(文本，视频，声音等等）
};


enum RSUType {
	INTERSECTION,       //处于十字路口中央，城镇特有
	VERTICAL,           //处于竖直的道路中
	HORIZONTAL          //处于水平的道路中    
};



enum EventLogType {
	SUCCEED = 0,
	EVENT_TO_WAIT = 2,
	SCHEDULETABLE_TO_SWITCH = 3,
	SCHEDULETABLE_TO_WAIT = 4,
	WAIT_TO_SWITCH = 5,
	WAIT_TO_ADMIT = 6,
	WAIT_TO_WAIT = 7,
	SWITCH_TO_WAIT = 8,
	TRANSIMIT_TO_WAIT = 9,
	IS_TRANSIMITTING=10,
	ADMIT_TO_WAIT = 11,
};


enum ModulationType {
	QPSK = 2,
	_16QAM = 4,
	_64QAM = 6,
};
