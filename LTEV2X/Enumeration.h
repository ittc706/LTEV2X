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

enum ScheduleMode {

};




//***************地理拓扑类变量设置*****************//







//****************MAC层变量类型******************//






//****************物理层变量类型****************//

enum ModulationType {//调制方式
	BPSK = 1,
	QPSK = 2,
	_16QAM = 4,
	_64QAM = 6
};










struct sMCS {//调制编码方式等级
	ModulationType modulationType;//调制方式
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