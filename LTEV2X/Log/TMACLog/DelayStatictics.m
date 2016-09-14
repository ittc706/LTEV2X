clear all;
close all;
clc;


separate=0;




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




if(max(periodQueuingDelay)-min(periodQueuingDelay)<5)
    [numberPeriod,centerPeriod]=hist(periodQueuingDelay,min(periodQueuingDelay)-1:min(periodQueuingDelay)+5);
else
    [numberPeriod,centerPeriod]=hist(periodQueuingDelay,min(periodQueuingDelay)-1:max(periodQueuingDelay)+1);
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(max(emergencyQueuingDelay)-min(emergencyQueuingDelay)<5)
    [numberEmergency,centerEmergency]=hist(emergencyQueuingDelay,min(emergencyQueuingDelay)-1:min(emergencyQueuingDelay)+5);
else
    [numberEmergency,centerEmergency]=hist(emergencyQueuingDelay,min(emergencyQueuingDelay)-1:max(emergencyQueuingDelay)+1);
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(max(dataQueuingDelay)-min(dataQueuingDelay)<5)
    [numberData,centerData]=hist(dataQueuingDelay,min(dataQueuingDelay)-1:min(dataQueuingDelay)+5);
else
    [numberData,centerData]=hist(dataQueuingDelay,min(dataQueuingDelay)-1:max(dataQueuingDelay)+1);
end
numberData=numberData./sum(numberData);

%% 等待时延
if(separate==1)
    figure(1)
    bar(centerPeriod,numberPeriod);
    title('周期事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(2)
    bar(centerEmergency,numberEmergency);
    title('紧急事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(3)
    bar(centerData,numberData);
    title('数据业务事件等待时延统计','LineWidth',2);
    xlabel('等待时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
else
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
end


%% 传输时延
if(max(periodSendDelay)-min(periodSendDelay)<5)
    [numberPeriod,centerPeriod]=hist(periodSendDelay,min(periodSendDelay)-1:min(periodSendDelay)+5);
else
    [numberPeriod,centerPeriod]=hist(periodSendDelay,min(periodSendDelay)-1:max(periodSendDelay)+1);
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(max(emergencySendDelay)-min(emergencySendDelay)<5)
    [numberEmergency,centerEmergency]=hist(emergencySendDelay,min(emergencySendDelay)-1:min(emergencySendDelay)+5);
else
    [numberEmergency,centerEmergency]=hist(emergencySendDelay,min(emergencySendDelay)-1:max(emergencySendDelay)+1);
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(max(dataSendDelay)-min(dataSendDelay)<5)
    [numberData,centerData]=hist(dataSendDelay,min(dataSendDelay)-1:min(dataSendDelay)+5);
else
    [numberData,centerData]=hist(dataSendDelay,min(dataSendDelay)-1:max(dataSendDelay)+1);
end
numberData=numberData./sum(numberData);


if(separate==1)
    figure(4)
    bar(centerPeriod,numberPeriod);
    title('周期事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(5)
    bar(centerEmergency,numberEmergency);
    title('紧急事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(6)
    bar(centerData,numberData);
    title('数据业务事件传输时延统计','LineWidth',2);
    xlabel('传输时延(TTI)','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
else
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
end



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



if(max(periodConflictNum)-min(periodConflictNum)<5)
    [numberPeriod,centerPeriod]=hist(periodConflictNum,min(periodConflictNum)-1:min(periodConflictNum)+5);
else
    [numberPeriod,centerPeriod]=hist(periodConflictNum,min(periodConflictNum)-1:max(periodConflictNum)+1);
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(max(emergencyConflictNum)-min(emergencyConflictNum)<5)
    [numberEmergency,centerEmergency]=hist(emergencyConflictNum,min(emergencyConflictNum)-1:min(emergencyConflictNum)+5);
else
    [numberEmergency,centerEmergency]=hist(emergencyConflictNum,min(emergencyConflictNum)-1:max(emergencyConflictNum)+1);
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(max(dataConflictNum)-min(dataConflictNum)<5)
    [numberData,centerData]=hist(dataConflictNum,min(dataConflictNum)-1:min(dataConflictNum)+5);
else
    [numberData,centerData]=hist(dataConflictNum,min(dataConflictNum)-1:max(dataConflictNum)+1);
end
numberData=numberData./sum(numberData);

if(separate==1)
    figure(7)
    bar(centerPeriod,numberPeriod);
    title('周期事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(8)
    bar(centerEmergency,numberEmergency);
    title('紧急事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;

    figure(9)
    bar(centerData,numberData);
    title('数据业务事件冲突统计','LineWidth',2);
    xlabel('冲突次数','LineWidth',2);
    ylabel('概率','LineWidth',2);
    grid on;
else
    figure(3)
    set(3,'position',[0,0,1800,600]);
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
end




%% 吞吐率
load ./TTIThroughput.txt
load ./RSUThroughput.txt
load ./VeUENumPerRSULogInfo.txt

TTIThroughput=TTIThroughput/1000;
accumulatedTTIThroughput=zeros(1,length(TTIThroughput));
accumulatedTTIThroughput(1,1)=TTIThroughput(1,1);
for iter=2:length(TTIThroughput)
    accumulatedTTIThroughput(1,iter)=accumulatedTTIThroughput(1,iter-1)+TTIThroughput(1,iter);
end

figure(10)
plot(TTIThroughput);
title('TTI吞吐量','LineWidth',2);
xlabel('TTI','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;

figure(11)
plot(accumulatedTTIThroughput);
title('累计吞吐量','LineWidth',2);
xlabel('TTI','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


RSUThroughput=RSUThroughput/1000;
figure(12)
bar(RSUThroughput);
title('吞吐量统计','LineWidth',2);
xlabel('RSUId','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


VeUENumPerRSULogInfo=mean(VeUENumPerRSULogInfo,1);
figure(13)
bar(VeUENumPerRSULogInfo)
title('车辆分布统计图','LineWidth',2);
xlabel('车辆数目','LineWidth',2);
ylabel('RSUId','LineWidth',2);
grid on;



%% 紧急事件以及数据业务事件泊松分布验证
load ./EmergencyPossion.txt;
load ./DataPossion.txt;

if(unique(EmergencyPossion)==0)
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,10);
else
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,unique(EmergencyPossion));
end


figure(14)
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

figure(15)
bar(centerData,numberData);
title('数据业务事件泊松分布图','LineWidth',2);
xlabel('在仿真时间内数据业务事件生成次数','LineWidth',2);
ylabel('车辆数目','LineWidth',2);
grid on;


