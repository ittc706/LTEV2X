clear all;
close all;
clc;

%% ��ȡʱ��ͳ����Ϣ

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




if(length(unique(periodQueuingDelay))<10)
    [numberPeriod,centerPeriod]=hist(periodQueuingDelay,10);
else
    [numberPeriod,centerPeriod]=hist(periodQueuingDelay,unique(periodQueuingDelay));
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(length(unique(emergencyQueuingDelay))<10)
    [numberEmergency,centerEmergency]=hist(emergencyQueuingDelay,10);
else
    [numberEmergency,centerEmergency]=hist(emergencyQueuingDelay,unique(emergencyQueuingDelay));
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(length(unique(dataQueuingDelay))<10)
    [numberData,centerData]=hist(dataQueuingDelay,10);
else
    [numberData,centerData]=hist(dataQueuingDelay,unique(dataQueuingDelay));
end
numberData=numberData./sum(numberData);

%% �ȴ�ʱ��
figure(1)
set(1,'position',[0,0,1800,600]);
subplot(1,3,1);
bar(centerPeriod,numberPeriod);
title('�����¼��ȴ�ʱ��ͳ��','LineWidth',2);
xlabel('�ȴ�ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

subplot(1,3,2);
bar(centerEmergency,numberEmergency);
title('�����¼��ȴ�ʱ��ͳ��','LineWidth',2);
xlabel('�ȴ�ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

subplot(1,3,3);
bar(centerData,numberData);
title('����ҵ���¼��ȴ�ʱ��ͳ��','LineWidth',2);
xlabel('�ȴ�ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

%% ����ʱ��
if(length(unique(periodSendDelay))<10)
    [numberPeriod,centerPeriod]=hist(periodSendDelay,10);
else
    [numberPeriod,centerPeriod]=hist(periodSendDelay,unique(periodSendDelay));
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(length(unique(emergencySendDelay))<10)
    [numberEmergency,centerEmergency]=hist(emergencySendDelay,10);
else
    [numberEmergency,centerEmergency]=hist(emergencySendDelay,unique(emergencySendDelay));
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(length(unique(dataSendDelay))<10)
    [numberData,centerData]=hist(dataSendDelay,10);
else
    [numberData,centerData]=hist(dataSendDelay,unique(dataSendDelay));
end
numberData=numberData./sum(numberData);

figure(2)
set(2,'position',[0,0,1800,600]);
subplot(1,3,1);
bar(centerPeriod,numberPeriod);
title('�����¼�����ʱ��ͳ��','LineWidth',2);
xlabel('����ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

subplot(1,3,2);
bar(centerEmergency,numberEmergency);
title('�����¼�����ʱ��ͳ��','LineWidth',2);
xlabel('����ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

subplot(1,3,3);
bar(centerData,numberData);
title('����ҵ���¼�����ʱ��ͳ��','LineWidth',2);
xlabel('����ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;


%% �����¼��Լ�����ҵ���¼����ɷֲ���֤
load ./EmergencyPossion.txt;
load ./DataPossion.txt;

if(unique(EmergencyPossion)==0)
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,10);
else
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,unique(EmergencyPossion));
end


figure(3)
bar(centerEmergency,numberEmergency);
title('�����¼����ɷֲ�ͼ','LineWidth',2);
xlabel('�ڷ���ʱ���ڽ����¼����ɴ���','LineWidth',2);
ylabel('������Ŀ','LineWidth',2);
grid on;

if(unique(DataPossion)==0)
    [numberData,centerData]=hist(DataPossion,10);
else
    [numberData,centerData]=hist(DataPossion,unique(DataPossion));
end

figure(4)
bar(centerData,numberData);
title('����ҵ���¼����ɷֲ�ͼ','LineWidth',2);
xlabel('�ڷ���ʱ��������ҵ���¼����ɴ���','LineWidth',2);
ylabel('������Ŀ','LineWidth',2);
grid on;



%% ��ȡ��ͻͳ����Ϣ
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



if(length(unique(periodConflictNum))<10)
    [numberPeriod,centerPeriod]=hist(periodConflictNum,10);
else
    [numberPeriod,centerPeriod]=hist(periodConflictNum,unique(periodConflictNum));
end
numberPeriod=numberPeriod./sum(numberPeriod);

if(length(unique(emergencyConflictNum))<10)
    [numberEmergency,centerEmergency]=hist(emergencyConflictNum,10);
else
    [numberEmergency,centerEmergency]=hist(emergencyConflictNum,unique(emergencyConflictNum));
end
numberEmergency=numberEmergency./sum(numberEmergency);

if(length(unique(dataConflictNum))<10)
    [numberData,centerData]=hist(dataConflictNum,10);
else
    [numberData,centerData]=hist(dataConflictNum,unique(dataConflictNum));
end
numberData=numberData./sum(numberData);


figure(5)
set(5,'position',[0,0,1800,600]);
subplot(1,3,1);
bar(centerPeriod,numberPeriod);
title('�����¼���ͻͳ��','LineWidth',2);
xlabel('��ͻ����','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

subplot(1,3,2);
bar(centerEmergency,numberEmergency);
title('�����¼���ͻͳ��','LineWidth',2);
xlabel('��ͻ����','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

subplot(1,3,3);
bar(centerData,numberData);
title('����ҵ���¼���ͻͳ��','LineWidth',2);
xlabel('��ͻ����','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;



%% ������
load ./TTIThroughput.txt

TTIThroughput=TTIThroughput/1000;
accumulatedTTIThroughput=zeros(1,length(TTIThroughput));
accumulatedTTIThroughput(1,1)=TTIThroughput(1,1);
for iter=2:length(TTIThroughput)
    accumulatedTTIThroughput(1,iter)=accumulatedTTIThroughput(1,iter-1)+TTIThroughput(1,iter);
end

figure(6)
plot(TTIThroughput);
title('TTI������','LineWidth',2);
xlabel('TTI(10ms)','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;

figure(7)
plot(accumulatedTTIThroughput);
title('�ۼ�������','LineWidth',2);
xlabel('TTI','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


load ./RSUThroughput.txt

RSUThroughput=RSUThroughput/1000;

figure(8)
bar(RSUThroughput);
title('������ͳ��','LineWidth',2);
xlabel('RSUId(10ms)','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


