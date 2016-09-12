#pragma once


enum eLocationType { //λ������ 
	Los, 
	Nlos, 
	None 
}; 

enum eGTATMode {
	URBAN,      //����
	HIGHSPEED   //����
};

enum eRRMMode {
	PF=1,       //���ڱ�����ƽ�ĵ���
	RR=2,       //Round-Robin,��ѯ
	DRA         //Distributed Resource Allocation,�ֲ�ʽ
};


enum eMessageType {
	EMERGENCY = -1,   //������Ϣ�����糵����Ϣ�����ȼ��ϸߣ�
	PERIOD = 0,  //��������Ϣ������VeUE�Ļ�����Ϣ�����٣�λ�ã�����ȵȣ�
	DATA = 1     //����ҵ��(�ı�����Ƶ�������ȵȣ�
};


enum eRSUType {
	INTERSECTION,       //����ʮ��·�����룬��������
	VERTICAL,           //������ֱ�ĵ�·��
	HORIZONTAL          //����ˮƽ�ĵ�·��    
};



enum eEventLogType {
	SUCCEED = 0,
	EVENT_TO_WAIT = 2,
	SCHEDULETABLE_TO_SWITCH = 3,
	SCHEDULETABLE_TO_WAIT = 4,
	WAIT_TO_SWITCH = 5,
	WAIT_TO_ADMIT = 6,
	SWITCH_TO_WAIT = 8,
	TRANSIMIT_TO_WAIT = 9,
	IS_TRANSIMITTING=10,
	ADMIT_TO_WAIT = 11,
};


