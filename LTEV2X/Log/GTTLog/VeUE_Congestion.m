clear all;
close all;
clc;

%% 解析标签数据
fid=fopen('StatisticsDescription.txt','r');
Description=fscanf(fid,'%s');

[tokens]=regexp(Description,'<([^<>]*)>([^<>]*)</([^<>]*)>', 'tokens');
DescriptionNum=length(tokens);
DescriptionMap=containers.Map();
for iter=1:length(tokens)
    DescriptionMap(cell2mat(tokens{iter}(1)))=str2double(cell2mat(tokens{iter}(2)));
end


%% 读取数据
VeUECongestionInfo=load('VeUECongestionInfo.txt');

[row,col]=size(VeUECongestionInfo);

% 车辆的数目
VeUENum=DescriptionMap('VeUENum');

% 拥塞等级数目
CongestionLevelNum=DescriptionMap('CongestionLevelNum');

% 运动次数
frecNum=row/VeUENum;

% 三维数组，第三维度是运动次数
VeUECongestionPerFrec=zeros(VeUENum,col,frecNum);

figure(1);
RandomColor = rand(CongestionLevelNum,3);
for iterFrec=1:frecNum
    VeUECongestionPerFrec(:,:,iterFrec)=VeUECongestionInfo((iterFrec-1)*VeUENum+1:iterFrec*VeUENum,:);
    for iterVeUE=1:VeUENum
        plot(VeUECongestionPerFrec(iterVeUE,1,iterFrec),VeUECongestionPerFrec(iterVeUE,2,iterFrec),'X','Color',RandomColor(VeUECongestionPerFrec(iterVeUE,3,iterFrec)+1,:));
        hold on;
    end
    input('按任意键查看下一次运动的拥塞分布图','s')
end



            