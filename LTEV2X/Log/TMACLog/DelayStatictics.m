clear all;
close all;
clc;

load ./DelayStatistics.txt;


queuingDelay=DelayStatistics(1,:);
sendDelay=DelayStatistics(2,:);


if(unique(queuingDelay)==0)
    [number1,center1]=hist(queuingDelay,10);
else
    [number1,center1]=hist(queuingDelay,unique(queuingDelay));
end
[number2,center2]=hist(sendDelay,unique(sendDelay));


%% �ȴ�ʱ��
figure(1)
bar(center1,number1);
title('�ȴ�ʱ��ͳ��','LineWidth',2);
xlabel('�ȴ�ʱ��','LineWidth',2);
ylabel('�¼�����','LineWidth',2);
grid on;


%% ����ʱ��
figure(2)
bar(center2,number2);
title('����ʱ��ͳ��','LineWidth',2);
xlabel('����ʱ��(TTI)','LineWidth',2);
ylabel('�¼�����','LineWidth',2);
grid on;


%% �����¼��Լ�����ҵ���¼����ɷֲ���֤
load ./EmergencyPossion.txt;
load ./DataPossion.txt;

if(unique(EmergencyPossion)==0)
    [number3,center3]=hist(EmergencyPossion,10);
else
    [number3,center3]=hist(EmergencyPossion,unique(EmergencyPossion));
end


figure(3)
bar(center3,number3);
title('�����¼����ɷֲ�ͼ','LineWidth',2);
xlabel('�ڷ���ʱ���ڽ����¼����ɴ���','LineWidth',2);
ylabel('������Ŀ','LineWidth',2);
grid on;

if(unique(DataPossion)==0)
    [number4,center4]=hist(DataPossion,10);
else
    [number4,center4]=hist(DataPossion,unique(DataPossion));
end

figure(4)
bar(center4,number4);
title('����ҵ���¼����ɷֲ�ͼ','LineWidth',2);
xlabel('�ڷ���ʱ��������ҵ���¼����ɴ���','LineWidth',2);
ylabel('������Ŀ','LineWidth',2);
grid on;



%% ��ͻͳ��
load ./ConflictNum.txt
if(unique(ConflictNum)==0)
    [number5,center5]=hist(ConflictNum,10);
else
    [number5,center5]=hist(ConflictNum,unique(ConflictNum));
end


figure(5)
bar(center5,number5);
title('��ͻͳ��','LineWidth',2);
xlabel('��ͻ����','LineWidth',2);
ylabel('�¼���Ŀ','LineWidth',2);
grid on;



%% ������
load ./TTIThroughput.txt

TTIThroughput=TTIThroughput/1000;

figure(6)
plot(TTIThroughput);
title('������ͳ��','LineWidth',2);
xlabel('TTI(10ms)','LineWidth',2);
ylabel('K bit','LineWidth',2);

load ./RSUThroughput.txt

RSUThroughput=RSUThroughput/1000;

figure(7)
bar(RSUThroughput);
title('������ͳ��','LineWidth',2);
xlabel('RSUId(10ms)','LineWidth',2);
ylabel('K bit','LineWidth',2);


