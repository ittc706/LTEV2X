clear all;
close all;
clc;

%% 解析标签数据
fid=fopen('StatisticsDescription.txt','r');
Description=fscanf(fid,'%s');

[start,endd,extents,match,tokens,names]=regexp(Description,'<([^<>]*)>([^<>]*)</([^<>]*)>')

figId=1;%图的Id

%% 读取时延统计信息
PeriodDelayStatistics=load('PeriodDelayStatistics.txt');

PeriodQueuingDelay=PeriodDelayStatistics(1,:);
PeriodSendDelay=PeriodDelayStatistics(2,:);


[numberPeriod,centerPeriod]=hist(PeriodQueuingDelay,min(PeriodQueuingDelay)-1:max(PeriodQueuingDelay)+1);
numberPeriod=numberPeriod./sum(numberPeriod);


%% 等待时延

figure(figId)
figId=figId+1;
bar(centerPeriod,numberPeriod);
title('周期事件等待时延统计','LineWidth',2);
xlabel('等待时延(TTI)','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;
    

%% 传输时延
[numberPeriod,centerPeriod]=hist(PeriodSendDelay,min(PeriodSendDelay)-1:max(PeriodSendDelay)+1);
numberPeriod=numberPeriod./sum(numberPeriod);

figure(figId)
figId=figId+1;
bar(centerPeriod,numberPeriod);
title('周期事件传输时延统计','LineWidth',2);
xlabel('传输时延(TTI)','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;



%% 读取冲突统计信息
PeriodConflictNum=load('PeriodConflictNum.txt');

[numberPeriod,centerPeriod]=hist(PeriodConflictNum,min(PeriodConflictNum)-1:max(PeriodConflictNum)+1);
numberPeriod=numberPeriod./sum(numberPeriod);

figure(figId)
figId=figId+1;
bar(centerPeriod,numberPeriod);
title('周期事件冲突统计','LineWidth',2);
xlabel('冲突次数','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;


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





%% PRR
PackageLossDistance=load('PackageLoss.txt');
[numPackageLossDistance,centerPackageLossDistance]=hist(PackageLossDistance',0:10:500);

numPackageLossDistance=numPackageLossDistance/6307;

figure(figId)
figId=figId+1;
plot(centerPackageLossDistance,numPackageLossDistance,'bo-','LineWidth',2);
title('PDR','LineWidth',2);
xlabel('Drop Rate','LineWidth',2);
ylabel('Distance(m)','LineWidth',2);
grid on;
