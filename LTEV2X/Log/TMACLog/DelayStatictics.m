clear all;
close all;
clc;


separate=0;%������ͼ�ϲ����Ƿֿ�
figId=1;%ͼ��Id

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

%% �ȴ�ʱ��
if(separate==1)
    figure(figId)
    figId=figId+1;
    bar(centerPeriod,numberPeriod);
    title('�����¼��ȴ�ʱ��ͳ��','LineWidth',2);
    xlabel('�ȴ�ʱ��(TTI)','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerEmergency,numberEmergency);
    title('�����¼��ȴ�ʱ��ͳ��','LineWidth',2);
    xlabel('�ȴ�ʱ��(TTI)','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerData,numberData);
    title('����ҵ���¼��ȴ�ʱ��ͳ��','LineWidth',2);
    xlabel('�ȴ�ʱ��(TTI)','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;
else
    figure(figId)
    figId=figId+1;
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
end


%% ����ʱ��
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
    figure(figId)
    figId=figId+1;
    bar(centerPeriod,numberPeriod);
    title('�����¼�����ʱ��ͳ��','LineWidth',2);
    xlabel('����ʱ��(TTI)','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerEmergency,numberEmergency);
    title('�����¼�����ʱ��ͳ��','LineWidth',2);
    xlabel('����ʱ��(TTI)','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerData,numberData);
    title('����ҵ���¼�����ʱ��ͳ��','LineWidth',2);
    xlabel('����ʱ��(TTI)','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;
else
    figure(figId)
    figId=figId+1;
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
end



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
    figure(figId)
    figId=figId+1;
    bar(centerPeriod,numberPeriod);
    title('�����¼���ͻͳ��','LineWidth',2);
    xlabel('��ͻ����','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerEmergency,numberEmergency);
    title('�����¼���ͻͳ��','LineWidth',2);
    xlabel('��ͻ����','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;

    figure(figId)
    figId=figId+1;
    bar(centerData,numberData);
    title('����ҵ���¼���ͻͳ��','LineWidth',2);
    xlabel('��ͻ����','LineWidth',2);
    ylabel('����','LineWidth',2);
    grid on;
else
    figure(figId)
    figId=figId+1;
    set(3,'position',[0,0,1800,600]);
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
end




%% ������
load ./TTIThroughput.txt
load ./RSUThroughput.txt
load ./VeUENumPerRSULogInfo.txt

TTIThroughput=TTIThroughput/1000;
accumulatedTTIThroughput=zeros(1,length(TTIThroughput));
accumulatedTTIThroughput(1,1)=TTIThroughput(1,1);
for iter=2:length(TTIThroughput)
    accumulatedTTIThroughput(1,iter)=accumulatedTTIThroughput(1,iter-1)+TTIThroughput(1,iter);
end

figure(figId)
figId=figId+1;
plot(TTIThroughput);
title('TTI������','LineWidth',2);
xlabel('TTI','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;

figure(figId)
figId=figId+1;
plot(accumulatedTTIThroughput);
title('�ۼ�������','LineWidth',2);
xlabel('TTI','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


RSUThroughput=RSUThroughput/1000;
figure(figId)
figId=figId+1;
bar(RSUThroughput);
title('������ͳ��','LineWidth',2);
xlabel('RSUId','LineWidth',2);
ylabel('K bit','LineWidth',2);
grid on;


VeUENumPerRSULogInfo=mean(VeUENumPerRSULogInfo,1);
figure(figId)
figId=figId+1;
bar(VeUENumPerRSULogInfo)
title('�����ֲ�ͳ��ͼ','LineWidth',2);
xlabel('������Ŀ','LineWidth',2);
ylabel('RSUId','LineWidth',2);
grid on;


%TTI������CDFͼ
[numberTTIThroughput,centerTTIThroughput]=hist(TTIThroughput,100);
Tem=zeros(length(centerTTIThroughput),2);
Tem(:,1)=centerTTIThroughput.';
Tem(:,2)=numberTTIThroughput.';
sortrows(Tem,1);%��������ֵ������ֵ����
centerTTIThroughput=Tem(:,1).';
numberTTIThroughput=Tem(:,2).';
TotalTTI=length(TTIThroughput);
accumulateNumberTTIThroughput=cumsum(numberTTIThroughput);%����������
accumulatePTTIThroughput=accumulateNumberTTIThroughput./TotalTTI;%�����۸���

figure(figId)
figId=figId+1;
plot(centerTTIThroughput,accumulatePTTIThroughput,'bo-','LineWidth',2);
title('TTI������CDFͼ','LineWidth',2);
xlabel('������(Kbits/TTI)','LineWidth',2);
ylabel('CDF','LineWidth',2);
grid on;


%RSU������CDFͼ
RSUThroughput=RSUThroughput/length(TTIThroughput);
[numberRSUThroughput,centerRSUThroughput]=hist(RSUThroughput,length(RSUThroughput));
Tem=zeros(length(centerRSUThroughput),2);
Tem(:,1)=centerRSUThroughput.';
Tem(:,2)=numberRSUThroughput.';
sortrows(Tem,1);%��������ֵ������ֵ����
centerRSUThroughput=Tem(:,1).';
numberRSUThroughput=Tem(:,2).';
TotalRSU=length(RSUThroughput);
accumulateNumberRSUThroughput=cumsum(numberRSUThroughput);%����������
accumulatePRSUThroughput=accumulateNumberRSUThroughput./TotalRSU;%�����۸���

figure(figId)
figId=figId+1;
plot(centerRSUThroughput,accumulatePRSUThroughput,'bo-','LineWidth',2);
title('RSU������CDFͼ','LineWidth',2);
xlabel('������(Kbits/TTI/RSU)','LineWidth',2);
ylabel('CDF','LineWidth',2);
grid on;



%% �����¼��Լ�����ҵ���¼����ɷֲ���֤
load ./EmergencyPossion.txt;
load ./DataPossion.txt;

if(unique(EmergencyPossion)==0)
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,10);
else
    [numberEmergency,centerEmergency]=hist(EmergencyPossion,unique(EmergencyPossion));
end


figure(figId)
figId=figId+1;
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

figure(figId)
figId=figId+1;
bar(centerData,numberData);
title('����ҵ���¼����ɷֲ�ͼ','LineWidth',2);
xlabel('�ڷ���ʱ��������ҵ���¼����ɴ���','LineWidth',2);
ylabel('������Ŀ','LineWidth',2);
grid on;


