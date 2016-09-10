clear all;
close all;
clc;

%% 读取时延统计信息

DelayStatistics=importdata('DelayStatistics.txt');

periodBound=find(isnan(DelayStatistics(1,:)),1,'first');
emergencyBound=find(isnan(DelayStatistics(2,:)),1,'first');
dataBound=find(isnan(DelayStatistics(3,:)),1,'first');


if(isempty(periodBound))
    periodQueuingDelay=DelayStatistics(1,:);
    periodSendDelay=DelayStatistics(4,:);
else
    periodQueuingDelay=DelayStatistics(1,1:periodBound-1);
    periodSendDelay=DelayStatistics(4,1:periodBound-1);
end

if(isempty(emergencyBound))
    emergencyQueuingDelay=DelayStatistics(2,:);
    emergencySendDelay=DelayStatistics(5,:);
else
    emergencyQueuingDelay=DelayStatistics(2,1:emergencyBound-1);
    emergencySendDelay=DelayStatistics(5,1:emergencyBound-1);
end

if(isempty(dataBound))
    dataQueuingDelay=DelayStatistics(3,:);
    dataSendDelay=DelayStatistics(6,:);
else
    dataQueuingDelay=DelayStatistics(3,1:dataBound-1);
    dataSendDelay=DelayStatistics(6,1:dataBound-1);
end




if(sum(unique(periodQueuingDelay))<3)
    [numberPeriod,centerPeriod]=hist(periodQueuingDelay,10);
else
    [numberPeriod,centerPeriod]=hist(periodQueuingDelay,unique(periodQueuingDelay));
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(sum(unique(emergencyQueuingDelay))<3)
    [numberEmergency,centerEmergency]=hist(emergencyQueuingDelay,10);
else
    [numberEmergency,centerEmergency]=hist(emergencyQueuingDelay,unique(emergencyQueuingDelay));
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(sum(unique(dataQueuingDelay))<3)
    [numberData,centerData]=hist(dataQueuingDelay,10);
else
    [numberData,centerData]=hist(dataQueuingDelay,unique(dataQueuingDelay));
end
numberData=numberData./sum(numberData);

%% 等待时延
figure(1)
set(1,'position',[0,0,1800,600]);
subplot(1,3,1);
bar(centerPeriod,numberPeriod);
title('周期事件等待时延统计','LineWidth',2);
xlabel('等待时延(TTI)','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;

subplot(1,3,2);
bar(centerEmergency,numberEmergency);
title('紧急事件等待时延统计','LineWidth',2);
xlabel('等待时延(TTI)','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;

subplot(1,3,3);
bar(centerData,numberData);
title('数据业务事件等待时延统计','LineWidth',2);
xlabel('等待时延(TTI)','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;

%% 传输时延
if(sum(unique(periodSendDelay))<3)
    [numberPeriod,centerPeriod]=hist(periodSendDelay,10);
else
    [numberPeriod,centerPeriod]=hist(periodSendDelay,unique(periodSendDelay));
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(sum(unique(emergencySendDelay))<3)
    [numberEmergency,centerEmergency]=hist(emergencySendDelay,10);
else
    [numberEmergency,centerEmergency]=hist(emergencySendDelay,unique(emergencySendDelay));
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(sum(unique(dataSendDelay))<3)
    [numberData,centerData]=hist(dataSendDelay,10);
else
    [numberData,centerData]=hist(dataSendDelay,unique(dataSendDelay));
end
numberData=numberData./sum(numberData);

figure(2)
set(2,'position',[0,0,1800,600]);
subplot(1,3,1);
bar(centerPeriod,numberPeriod);
title('周期事件传输时延统计','LineWidth',2);
xlabel('传输时延(TTI)','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;

subplot(1,3,2);
bar(centerEmergency,numberEmergency);
title('紧急事件传输时延统计','LineWidth',2);
xlabel('传输时延(TTI)','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;

subplot(1,3,3);
bar(centerData,numberData);
title('数据业务事件传输时延统计','LineWidth',2);
xlabel('传输时延(TTI)','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;


%% 紧急事件以及数据业务事件泊松分布验证
load ./EmergencyPossion.txt;
load ./DataPossion.txt;

if(unique(EmergencyPossion)==0)
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,10);
else
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,unique(EmergencyPossion));
end


figure(3)
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

figure(4)
bar(centerData,numberData);
title('数据业务事件泊松分布图','LineWidth',2);
xlabel('在仿真时间内数据业务事件生成次数','LineWidth',2);
ylabel('车辆数目','LineWidth',2);
grid on;



%% 读取冲突统计信息
ConflictNum=importdata('ConflictNum.txt');

periodBound=find(isnan(ConflictNum(1,:)),1,'first');
emergencyBound=find(isnan(ConflictNum(2,:)),1,'first');
dataBound=find(isnan(ConflictNum(3,:)),1,'first');


if(isempty(periodBound))
    periodConflictNum=ConflictNum(1,:);
else
    periodConflictNum=ConflictNum(1,1:periodBound-1);
end

if(isempty(emergencyBound))
    emergencyConflictNum=ConflictNum(2,:);
else
    emergencyConflictNum=ConflictNum(2,1:emergencyBound-1);
end

if(isempty(dataBound))
    dataConflictNum=ConflictNum(3,:);
else
    dataConflictNum=ConflictNum(3,1:dataBound-1);
end



if(sum(unique(periodConflictNum))<3)
    [numberPeriod,centerPeriod]=hist(periodConflictNum,10);
else
    [numberPeriod,centerPeriod]=hist(periodConflictNum,unique(periodConflictNum));
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(sum(unique(emergencyConflictNum))<3)
    [numberEmergency,centerEmergency]=hist(emergencyConflictNum,10);
else
    [numberEmergency,centerEmergency]=hist(emergencyConflictNum,unique(emergencyConflictNum));
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(sum(unique(dataConflictNum))<3)
    [numberData,centerData]=hist(dataConflictNum,10);
else
    [numberData,centerData]=hist(dataConflictNum,unique(dataConflictNum));
end
numberData=numberData./sum(numberData);


figure(5)
set(5,'position',[0,0,1800,600]);
subplot(1,3,1);
bar(centerPeriod,numberPeriod);
title('周期事件冲突统计','LineWidth',2);
xlabel('冲突次数','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;

subplot(1,3,2);
bar(centerEmergency,numberEmergency);
title('紧急事件冲突统计','LineWidth',2);
xlabel('冲突次数','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;

subplot(1,3,3);
bar(centerData,numberData);
title('数据业务事件冲突统计','LineWidth',2);
xlabel('冲突次数','LineWidth',2);
ylabel('概率','LineWidth',2);
grid on;



%% 吞吐率
load ./TTIThroughput.txt

TTIThroughput=TTIThroughput/1000;
accumulatedTTIThroughput=zeros(1,length(TTIThroughput));
accumulatedTTIThroughput(1,1)=TTIThroughput(1,1);
for iter=2:length(TTIThroughput)
    accumulatedTTIThroughput(1,iter)=accumulatedTTIThroughput(1,iter-1)+TTIThroughput(1,iter);
end

figure(6)
plot(TTIThroughput);
title('TTI吞吐量','LineWidth',2);
xlabel('TTI(10ms)','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;

figure(7)
plot(accumulatedTTIThroughput);
title('累计吞吐量','LineWidth',2);
xlabel('TTI','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


load ./RSUThroughput.txt

RSUThroughput=RSUThroughput/1000;

figure(8)
bar(RSUThroughput);
title('吞吐率统计','LineWidth',2);
xlabel('RSUId(10ms)','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


