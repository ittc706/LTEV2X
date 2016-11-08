#pragma once


enum LocationType { //λ������ 
	Los, 
	Nlos, 
	None 
}; 

enum GTTMode {
	URBAN,      //����
	HIGHSPEED   //����
};

enum RRMMode {
	RR,       //Round-Robin,��ѯ
	TDM_DRA,        //Distributed Resource Allocation,�ֲ�ʽ
	ICC_DRA
};


enum MessageType {
	EMERGENCY = 0,   //������Ϣ�����糵����Ϣ�����ȼ��ϸߣ�
	PERIOD = 1,  //��������Ϣ������VeUE�Ļ�����Ϣ�����٣�λ�ã�����ȵȣ�
	DATA = 2     //����ҵ��(�ı�����Ƶ�������ȵȣ�
};


enum RSUType {
	INTERSECTION,       //����ʮ��·�����룬��������
	VERTICAL,           //������ֱ�ĵ�·��
	HORIZONTAL          //����ˮƽ�ĵ�·��    
};



enum EventLogType {
	SUCCEED = 0,
	CONFLICT=1,
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
	ALL_BUSY=12,
};


enum ModulationType {
	QPSK = 2,
	_16QAM = 4,
	_64QAM = 6,
};
