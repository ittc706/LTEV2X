#pragma once


enum eLocationType { //λ������ 
	Los, 
	Nlos, 
	None 
}; 


enum eScheduleMode {
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

enum eDRAMode {
	//P1:  Collision avoidance based on sensing
	//P2:  Enhanced random resource selection
	//P3:  Location-based resource selection
	P13,               //Combination of P1 and P3
	P23,               //Combination of P2 and P3
	P123               //Combination of P1 and P2 and P3
};


