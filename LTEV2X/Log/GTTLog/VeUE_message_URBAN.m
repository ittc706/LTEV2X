%城镇道路场景

% % 显示某个RSU下不同簇下的车辆拓扑
[data1,data2,data3,data4,data5] = textread('VeUEMessage.txt','%n%n%n%n%n');
dim = length(data1);
ClusterNumber = 5;
RandomColor = rand(ClusterNumber,3);
 for cnt = 1:1:dim
    if(data2(cnt,1)==8)%选择查看的RSU编号，从0到23
    ClusterId = data3(cnt,1)+1;
    plot(data4(cnt,1),data5(cnt,1),'X','Color',RandomColor(ClusterId,:));hold on
    end
 end
 
% 显示不同RSU下车辆的拓扑
% [data1,data2,data3,data4,data5] = textread('VeUEMessage.txt','%n%n%n%n%n');
% dim = length(data1);
% RSUNumber = 24;
% RandomColor = rand(RSUNumber,3);
%  for cnt = 1:1:dim
%     RSUId = data2(cnt,1)+1;
%     plot(data4(cnt,1),data5(cnt,1),'X','Color',RandomColor(RSUId,:));hold on
%  end
% 
%     
    
            