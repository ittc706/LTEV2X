clear all;
close all;
clc;


separate=0;%控制作图合并还是分开
figId=1;%图的Id

%% 读取时延统计信息

EmergencyDelayStatistics=load('EmergencyDelayStatistics.txt');
PeriodDelayStatistics=load('PeriodDelayStatistics.txt');
DataDelayStatistics=load('DataDelayStatistics.txt');

EmergencyQueuingDelay=EmergencyDelayStatistics(1,:);
EmergencySendDelay=EmergencyDelayStatistics(2,:);

PeriodQueuingDelay=PeriodDelayStatistics(1,:);
PeriodSendDelay=PeriodDelayStatistics(2,:);

DataQueuingDelay=DataDelayStatistics(1,:);
DataSendDelay=DataDelayStatistics(2,:);

[numberEmergency,centerEmergency]=hist(EmergencyQueuingDelay,min(EmergencyQueuingDelay)-1:max(EmergencyQueuingDelay)+1);
numberEmergency=numberEmergency./sum(numberEmergency);

[numberPeriod,centerPeriod]=hist(PeriodQueuingDelay,min(PeriodQueuingDelay)-1:max(PeriodQueuingDelay)+1);
numberPeriod=numberPeriod./sum(numberPeriod);

[numberData,centerData]=hist(DataQueuingDelay,min(DataQueuingDelay)-1:max(DataQueuingDelay)+1);
numberData=numberData./sum(numberData);

%% 等待时延
if(separate==1)
    figure(figId)
    figId=figId+1;
    bar(centerEmergency,numberEmergency);
    title('紧急事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerPeriod,numberPeriod);
    title('周期事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
    
    figure(figId)
    figId=figId+1;
    bar(centerData,numberData);
    title('数据业务事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
else
    figure(figId)
    figId=figId+1;
    set(1,'position',[0,0,1800,600]);
    
    subplot(1,3,1);
    bar(centerEmergency,numberEmergency);
    title('紧急事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
    
    subplot(1,3,2);
    bar(centerPeriod,numberPeriod);
    title('周期事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    subplot(1,3,3);
    bar(centerData,numberData);
    title('数据业务事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
end


%% 传输时延
[numberEmergency,centerEmergency]=hist(EmergencySendDelay,min(EmergencySendDelay)-1:max(EmergencySendDelay)+1);
numberEmergency=numberEmergency./sum(numberEmergency);

[numberPeriod,centerPeriod]=hist(PeriodSendDelay,min(PeriodSendDelay)-1:max(PeriodSendDelay)+1);
numberPeriod=numberPeriod./sum(numberPeriod);

[numberData,centerData]=hist(DataSendDelay,min(DataSendDelay)-1:max(DataSendDelay)+1);
numberData=numberData./sum(numberData);


if(separate==1)
    figure(figId)
    figId=figId+1;
    bar(centerEmergency,numberEmergency);
    title('紧急事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
    
    figure(figId)
    figId=figId+1;
    bar(centerPeriod,numberPeriod);
    title('周期事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerData,numberData);
    title('数据业务事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
else
    figure(figId)
    figId=figId+1;
    set(2,'position',[0,0,1800,600]);
    
    subplot(1,3,1);
    bar(centerEmergency,numberEmergency);
    title('紧急事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
    
    subplot(1,3,2);
    bar(centerPeriod,numberPeriod);
    title('周期事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    subplot(1,3,3);
    bar(centerData,numberData);
    title('数据业务事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
end



%% 读取冲突统计信息
EmergencyConflictNum=load('EmergencyConflictNum.txt');
PeriodConflictNum=load('PeriodConflictNum.txt');
DataConflictNum=load('DataConflictNum.txt');



[numberEmergency,centerEmergency]=hist(EmergencyConflictNum,min(EmergencyConflictNum)-1:max(EmergencyConflictNum)+1);
numberEmergency=numberEmergency./sum(numberEmergency);

[numberPeriod,centerPeriod]=hist(PeriodConflictNum,min(PeriodConflictNum)-1:max(PeriodConflictNum)+1);
numberPeriod=numberPeriod./sum(numberPeriod);

[numberData,centerData]=hist(DataConflictNum,min(DataConflictNum)-1:max(DataConflictNum)+1);
numberData=numberData./sum(numberData);

if(separate==1)
    figure(figId)
    figId=figId+1;
    bar(centerEmergency,numberEmergency);
    title('紧急事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
    
    figure(figId)
    figId=figId+1;
    bar(centerPeriod,numberPeriod);
    title('周期事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerData,numberData);
    title('数据业务事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
else
    figure(figId)
    figId=figId+1;
    set(3,'position',[0,0,1800,600]);

    subplot(1,3,1);
    bar(centerEmergency,numberEmergency);
    title('紧急事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
    
    subplot(1,3,2);
    bar(centerPeriod,numberPeriod);
    title('周期事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;


    subplot(1,3,3);
    bar(centerData,numberData);
    title('数据业务事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
end




%% 吞吐率
TTIThroughput=load('./TTIThroughput.txt');
RSUThroughput=load('./RSUThroughput.txt');
VeUENumPerRSULogInfo=load('../GTTLog/VeUENumPerRSULogInfo.txt');

TTIThroughput=TTIThroughput/1000;
accumulatedTTIThroughput=zeros(1,length(TTIThroughput));
accumulatedTTIThroughput(1,1)=TTIThroughput(1,1);
for iter=2:length(TTIThroughput)
    accumulatedTTIThroughput(1,iter)=accumulatedTTIThroughput(1,iter-1)+TTIThroughput(1,iter);
end

figure(figId)
figId=figId+1;
plot(TTIThroughput);
title('TTI吞吐量','LineWidth',2);
xlabel('TTI','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;

figure(figId)
figId=figId+1;
plot(accumulatedTTIThroughput);
title('累计吞吐量','LineWidth',2);
xlabel('TTI','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


RSUThroughput=RSUThroughput/1000;
figure(figId)
figId=figId+1;
bar(RSUThroughput);
title('吞吐量统计','LineWidth',2);
xlabel('RSUId','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


VeUENumPerRSULogInfo=mean(VeUENumPerRSULogInfo,1);
figure(figId)
figId=figId+1;
bar(VeUENumPerRSULogInfo)
title('车辆分布统计图','LineWidth',2);
xlabel('车辆数目','LineWidth',2);
ylabel('RSUId','LineWidth',2);
grid on;


%TTI吞吐量CDF图
[numberTTIThroughput,centerTTIThroughput]=hist(TTIThroughput,100);
Tem=zeros(length(centerTTIThroughput),2);
Tem(:,1)=centerTTIThroughput.';
Tem(:,2)=numberTTIThroughput.';
sortrows(Tem,1);%依据中心值进行主值排序
centerTTIThroughput=Tem(:,1).';
numberTTIThroughput=Tem(:,2).';
TotalTTI=length(TTIThroughput);
accumulateNumberTTIThroughput=cumsum(numberTTIThroughput);%计算累数量
accumulatePTTIThroughput=accumulateNumberTTIThroughput./TotalTTI;%计算累概率

figure(figId)
figId=figId+1;
plot(centerTTIThroughput,accumulatePTTIThroughput,'bo-','LineWidth',2);
title('TTI吞吐率CDF图','LineWidth',2);
xlabel('吞吐量(Kbits/TTI)','LineWidth',2);
ylabel('CDF','LineWidth',2);
grid on;


%RSU吞吐量CDF图
RSUThroughput=RSUThroughput/length(TTIThroughput);
[numberRSUThroughput,centerRSUThroughput]=hist(RSUThroughput,length(RSUThroughput));
Tem=zeros(length(centerRSUThroughput),2);
Tem(:,1)=centerRSUThroughput.';
Tem(:,2)=numberRSUThroughput.';
sortrows(Tem,1);%依据中心值进行主值排序
centerRSUThroughput=Tem(:,1).';
numberRSUThroughput=Tem(:,2).';
TotalRSU=length(RSUThroughput);
accumulateNumberRSUThroughput=cumsum(numberRSUThroughput);%计算累数量
accumulatePRSUThroughput=accumulateNumberRSUThroughput./TotalRSU;%计算累概率

figure(figId)
figId=figId+1;
plot(centerRSUThroughput,accumulatePRSUThroughput,'bo-','LineWidth',2);
title('RSU吞吐率CDF图','LineWidth',2);
xlabel('吞吐量(Kbits/TTI/RSU)','LineWidth',2);
ylabel('CDF','LineWidth',2);
grid on;



%% 紧急事件以及数据业务事件泊松分布验证
EmergencyPossion=load('./EmergencyPossion.txt');
DataPossion=load('./DataPossion.txt');

if(unique(EmergencyPossion)==0)
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,10);
else
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,unique(EmergencyPossion));
end


figure(figId)
figId=figId+1;
bar(centerEmergency,numberEmergency);
title('紧急事件泊松分布图','LineWidth',2);
xlabel('在仿真时间内紧急事件生成次数','LineWidth',2);
ylabel('车辆数目','LineWidth',2);
grid on;

if(unique(DataPossion)==0)
    [numberData,centerData]=hist(DataPossion,10);
else
    [numberData,centerData]=hist(DataPossion,unique(DataPossion));
end

figure(figId)
figId=figId+1;
bar(centerData,numberData);
title('数据业务事件泊松分布图','LineWidth',2);
xlabel('在仿真时间内数据业务事件生成次数','LineWidth',2);
ylabel('车辆数目','LineWidth',2);
grid on;


