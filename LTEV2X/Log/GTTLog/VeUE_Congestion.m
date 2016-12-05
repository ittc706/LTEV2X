clear all;
close all;
clc;

%% ������ǩ����
fid=fopen('StatisticsDescription.txt','r');
Description=fscanf(fid,'%s');

[tokens]=regexp(Description,'<([^<>]*)>([^<>]*)</([^<>]*)>', 'tokens');
DescriptionNum=length(tokens);
DescriptionMap=containers.Map();
for iter=1:length(tokens)
    DescriptionMap(cell2mat(tokens{iter}(1)))=str2double(cell2mat(tokens{iter}(2)));
end


%% ��ȡ����
VeUECongestionInfo=load('VeUECongestionInfo.txt');

[row,col]=size(VeUECongestionInfo);

% ��������Ŀ
VeUENum=DescriptionMap('VeUENum');

% ӵ���ȼ���Ŀ
CongestionLevelNum=DescriptionMap('CongestionLevelNum');

% �˶�����
frecNum=row/VeUENum;

% ��ά���飬����ά�����˶�����
VeUECongestionPerFrec=zeros(VeUENum,col,frecNum);

figure(1);
RandomColor = rand(CongestionLevelNum,3);
for iterFrec=1:frecNum
    VeUECongestionPerFrec(:,:,iterFrec)=VeUECongestionInfo((iterFrec-1)*VeUENum+1:iterFrec*VeUENum,:);
    for iterVeUE=1:VeUENum
        plot(VeUECongestionPerFrec(iterVeUE,1,iterFrec),VeUECongestionPerFrec(iterVeUE,2,iterFrec),'X','Color',RandomColor(VeUECongestionPerFrec(iterVeUE,3,iterFrec)+1,:));
        hold on;
    end
    input('��������鿴��һ���˶���ӵ���ֲ�ͼ','s')
end



            