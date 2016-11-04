clear all;
close all;
clc;


figId=1;%ͼ��Id

%% ��ȡʱ��ͳ����Ϣ

DelayStatistics=importdata('DelayStatistics.txt');

periodBound=find(isnan(DelayStatistics(1,:)),1,'first');



if(isempty(periodBound))
    periodQueuingDelay=DelayStatistics(1,:);
    periodSendDelay=DelayStatistics(2,:);
else
    periodQueuingDelay=DelayStatistics(1,1:periodBound-1);
    periodSendDelay=DelayStatistics(2,1:periodBound-1);
end




if(max(periodQueuingDelay)-min(periodQueuingDelay)<5)
    [numberPeriod,centerPeriod]=hist(periodQueuingDelay,min(periodQueuingDelay)-1:min(periodQueuingDelay)+5);
else
    [numberPeriod,centerPeriod]=hist(periodQueuingDelay,min(periodQueuingDelay)-1:max(periodQueuingDelay)+1);
end
numberPeriod=numberPeriod./sum(numberPeriod);


%% �ȴ�ʱ��

figure(figId)
figId=figId+1;
bar(centerPeriod,numberPeriod);
title('�����¼��ȴ�ʱ��ͳ��','LineWidth',2);
xlabel('�ȴ�ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

   


%% ����ʱ��
if(max(periodSendDelay)-min(periodSendDelay)<5)
    [numberPeriod,centerPeriod]=hist(periodSendDelay,min(periodSendDelay)-1:min(periodSendDelay)+5);
else
    [numberPeriod,centerPeriod]=hist(periodSendDelay,min(periodSendDelay)-1:max(periodSendDelay)+1);
end
numberPeriod=numberPeriod./sum(numberPeriod);




figure(figId)
figId=figId+1;
bar(centerPeriod,numberPeriod);
title('�����¼�����ʱ��ͳ��','LineWidth',2);
xlabel('����ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

    



%% ��ȡ��ͻͳ����Ϣ
ConflictNum=importdata('ConflictNum.txt');

periodBound=find(isnan(ConflictNum(1,:)),1,'first');


if(isempty(periodBound))
    periodConflictNum=ConflictNum(1,:);
else
    periodConflictNum=ConflictNum(1,1:periodBound-1);
end



if(max(periodConflictNum)-min(periodConflictNum)<5)
    [numberPeriod,centerPeriod]=hist(periodConflictNum,min(periodConflictNum)-1:min(periodConflictNum)+5);
else
    [numberPeriod,centerPeriod]=hist(periodConflictNum,min(periodConflictNum)-1:max(periodConflictNum)+1);
end
numberPeriod=numberPeriod./sum(numberPeriod);




figure(figId)
figId=figId+1;
bar(centerPeriod,numberPeriod);
title('�����¼���ͻͳ��','LineWidth',2);
xlabel('��ͻ����','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;

    




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




