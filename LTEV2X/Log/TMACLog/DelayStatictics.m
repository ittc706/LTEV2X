clear all;
close all;
clc;

load ./EmergencyPossion.txt;
load ./DataPossion.txt;

[number3,center3]=hist(EmergencyPossion,unique(EmergencyPossion));
max3=max(center3);
y3=zeros(1,max3);%等待时间包括0，因此加1
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
set(gca,'XTickLabel',0:1:max3);
xlabel('在仿真时间内紧急事件生成次数','LineWidth',2);
ylabel('车辆数目','LineWidth',2);
grid on;


[number4,center4]=hist(DataPossion,unique(DataPossion));
max4=max(center4);
y4=zeros(1,max4);%等待时间包括0，因此加1
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
set(gca,'XTickLabel',0:1:max4);
xlabel('在仿真时间内紧急事件生成次数','LineWidth',2);
ylabel('车辆数目','LineWidth',2);
grid on;