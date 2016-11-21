clear all;
close all;
clc;

%% ������ǩ����
fid=fopen('StatisticsDescription.txt','r');
Description=fscanf(fid,'%s');

[start,endd,extents,match,tokens,names]=regexp(Description,'<([^<>]*)>([^<>]*)</([^<>]*)>')

figId=1;%ͼ��Id

%% ��ȡʱ��ͳ����Ϣ
PeriodDelayStatistics=load('PeriodDelayStatistics.txt');

PeriodQueuingDelay=PeriodDelayStatistics(1,:);
PeriodSendDelay=PeriodDelayStatistics(2,:);


[numberPeriod,centerPeriod]=hist(PeriodQueuingDelay,min(PeriodQueuingDelay)-1:max(PeriodQueuingDelay)+1);
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
[numberPeriod,centerPeriod]=hist(PeriodSendDelay,min(PeriodSendDelay)-1:max(PeriodSendDelay)+1);
numberPeriod=numberPeriod./sum(numberPeriod);

figure(figId)
figId=figId+1;
bar(centerPeriod,numberPeriod);
title('�����¼�����ʱ��ͳ��','LineWidth',2);
xlabel('����ʱ��(TTI)','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;



%% ��ȡ��ͻͳ����Ϣ
PeriodConflictNum=load('PeriodConflictNum.txt');

[numberPeriod,centerPeriod]=hist(PeriodConflictNum,min(PeriodConflictNum)-1:max(PeriodConflictNum)+1);
numberPeriod=numberPeriod./sum(numberPeriod);

figure(figId)
figId=figId+1;
bar(centerPeriod,numberPeriod);
title('�����¼���ͻͳ��','LineWidth',2);
xlabel('��ͻ����','LineWidth',2);
ylabel('����','LineWidth',2);
grid on;


%% ������
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
