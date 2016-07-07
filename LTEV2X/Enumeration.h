#pragma once



enum eLinkType {//链路方向
	DOWNLINK,    //下行链路
	UPLINK      //上行链路
};

enum eTransMode {//传输模式
	INVALID_TRANSMODE,  //无效
	TM_1,        //???
	TM_2,        //???
	CODEBOOK,    //基于码本
	BEAMFORMING  //基于Beamforming
};

enum eScheduleMode {

};


enum eMessageType {
	PERIOD,  //周期性信息（包括车辆的基本信息，车速，位置，方向等等）
	EMERGENCY,   //紧急信息（比如车祸信息，优先级较高）
	DATA     //数据业务(文本，视频，声音等等）
};


enum eRSUType {
	INTERSECTION,       //处于十字路口中央，城镇特有
	VERTICAL,           //处于竖直的道路中
	HORIZONTAL          //处于水平的道路中    
};

enum eDRAMode {
	//P1:  Collision avoidance based on sensing
	//P2:  Enhanced random resource selection
	//P3:  Location-based resource selection
	P13,               //Combination of P1 and P3
	P23,               //Combination of P2 and P3
	P123               //Combination of P1 and P2 and P3
};


//****************物理层变量类型****************//

enum eModulationType {//调制方式
	BPSK = 1,
	QPSK = 2,
	_16QAM = 4,
	_64QAM = 6
};










struct sMCS {//调制编码方式等级
	eModulationType modulationType;//调制方式
	float rate;//码率
};


struct sMacPacket {
	int size;//MAC包的大小
	//double size_no_rate_loss;
	//double size_non_fullbuffer;//~~~
	int createTime;//MAC包的产生时间
	int receiveTime;//MAC包成功接收时间
	int reTransmitionNum;//MAC包重传次数
};