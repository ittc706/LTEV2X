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


%% 等待时延
figure(1)
bar(center1,number1);
xlabel('等待时延','LineWidth',2);
ylabel('事件数量','LineWidth',2);
grid on;


%% 传输时延
figure(2)
bar(center2,number2);
xlabel('传输时延(TTI)','LineWidth',2);
ylabel('事件数量','LineWidth',2);
grid on;


%% 紧急事件以及数据业务事件泊松分布验证
load ./EmergencyPossion.txt;
load ./DataPossion.txt;

if(unique(EmergencyPossion)==0)
    [number3,center3]=hist(EmergencyPossion,10);
else
    [number3,center3]=hist(EmergencyPossion,unique(EmergencyPossion));
end


figure(3)
bar(center3,number3);
xlabel('在仿真时间内紧急事件生成次数','LineWidth',2);
ylabel('车辆数目','LineWidth',2);
grid on;

if(unique(DataPossion)==0)
    [number4,center4]=hist(DataPossion,10);
else
    [number4,center4]=hist(DataPossion,unique(DataPossion));
end

figure(4)
bar(center4,number4);
xlabel('在仿真时间内数据业务事件生成次数','LineWidth',2);
ylabel('车辆数目','LineWidth',2);
grid on;



%% 冲突统计
load ./ConflictNum.txt
if(unique(ConflictNum)==0)
    [number5,center5]=hist(ConflictNum,10);
else
    [number5,center5]=hist(ConflictNum,unique(ConflictNum));
end


figure(5)
bar(center5,number5);
xlabel('冲突次数','LineWidth',2);
ylabel('事件数目','LineWidth',2);
grid on;



%% 吞吐率
load ./TTIThroughput.txt

TTIThroughput=TTIThroughput/1000;

figure(6)
plot(TTIThroughput);
xlabel('TTI(10ms)','LineWidth',2);
ylabel('K bit','LineWidth',2);

load ./RSUThroughput.txt

RSUThroughput=RSUThroughput/1000;

figure(6)
bar(RSUThroughput);
xlabel('RSUId(10ms)','LineWidth',2);
ylabel('K bit','LineWidth',2);


