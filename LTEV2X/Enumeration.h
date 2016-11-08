#pragma once


enum LocationType { //位置类型 
	Los, 
	Nlos, 
	None 
}; 

enum GTTMode {
	URBAN,      //城市
	HIGHSPEED   //高速
};

enum RRMMode {
	RR,       //Round-Robin,轮询
	TDM_DRA,        //Distributed Resource Allocation,分布式
	ICC_DRA
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
	TRANSIMITTING,
	SUCCEED,
	EVENT_TO_WAIT,
	SCHEDULETABLE_TO_SWITCH,
	SCHEDULETABLE_TO_WAIT,
	WAIT_TO_SWITCH,
	WAIT_TO_WAIT,
	SWITCH_TO_WAIT,
	TRANSIMIT_TO_WAIT,
};


enum ModulationType {
	QPSK = 2,
	_16QAM = 4,
	_64QAM = 6,
};
