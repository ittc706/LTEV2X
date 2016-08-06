clear all;
close all;
clc;

load ./DelayStatistics.txt;


queuingDelay=DelayStatistics(1,:);
sendDelay=DelayStatistics(2,:);


[number1,center1]=hist(queuingDelay,unique(queuingDelay));
[number2,center2]=hist(sendDelay,unique(sendDelay));
max1=max(center1);
y1=zeros(1,max1);%�ȴ�ʱ�����0����˼�1
iter=1;
for m=1:1:length(y1)
    while(iter<=length(center1) && center1(iter)<m)
        iter=iter+1;
    end
    if(iter<=length(center1)&&center1(iter)==m)
        y1(m)=number1(iter);
    else
        y1(m)=0;
    end    
end

figure(1)
bar(y1,0.5);
xlabel('�ȴ�ʱ��','LineWidth',2);
ylabel('�¼�����','LineWidth',2);
grid on;



max2=max(center2);
y2=zeros(1,max2);%�ȴ�ʱ�����0����˼�1
iter=1;
for m=1:1:length(y2)
    while(iter<=length(center2) && center2(iter)<m)
        iter=iter+1;
    end
    if(iter<=length(center2)&&center2(iter)==m)
        y2(m)=number2(iter);
    else
        y2(m)=0;
    end    
end

figure(2)
bar(y2);
xlabel('����ʱ��(TTI)','LineWidth',2);
ylabel('�¼�����','LineWidth',2);
grid on;


load ./EmergencyPossion.txt;

[number3,center3]=hist(EmergencyPossion,unique(EmergencyPossion));
max3=max(center3);
y3=zeros(1,max3);%�ȴ�ʱ�����0����˼�1
iter=1;
for m=0:1:length(y3)
    while(iter<=length(center3) && center3(iter)<m)
        iter=iter+1;
    end
    if(iter<=length(center3)&&center3(iter)==m)
        y3(m+1)=number3(iter);
    else
        y3(m+1)=0;
    end    
end


figure(3)
bar(y3);
set(gca,'XTickLabel',[0:1:max3]);
xlabel('�ڷ���ʱ���ڽ����¼����ɴ���','LineWidth',2);
ylabel('������Ŀ','LineWidth',2);
grid on;



load ./ConflictNum.txt


[number4,center4]=hist(ConflictNum,unique(ConflictNum));
mean4=mean(ConflictNum)
max4=max(center4);
y4=zeros(1,max4);%�ȴ�ʱ�����0����˼�1
iter=1;
for m=0:1:length(y4)
    while(iter<=length(center4) && center4(iter)<m)
        iter=iter+1;
    end
    if(iter<=length(center4)&&center4(iter)==m)
        y4(m+1)=number4(iter);
    else
        y4(m+1)=0;
    end    
end


figure(4)
bar(y4);
set(gca,'XTickLabel');
xlabel('��ͻ����','LineWidth',2);
ylabel('�¼���Ŀ','LineWidth',2);
grid on;


