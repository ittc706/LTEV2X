clear all;
close all;
clc;

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