figure(1)
[id,~,~,absX,absY,~] = textread('VeUEMessage.txt','%n%n%n%n%n%n');
vueNum = length(id);
RandomColor = linspecer(1);
for cnt = 1:1:vueNum
    plot(absX(cnt,1),absY(cnt,1),'X','Color',RandomColor);hold on
end
l1=legend('VUE',1);
title('高速公路车辆撒点分布图','fontweight','bold')
xlabel('x/m','fontweight','bold');
ylabel('y/m','fontweight','bold');
axis([-2200,2200,-15,15]);
set(l1,'Fontname', 'Times New Roman','FontWeight','bold')


figure(2)
[id,~,~,absX,absY,zoneId] = textread('VeUEMessage.txt','%n%n%n%n%n%n');
vueNum = length(id);
Zonenummber = 5;
RandomColor = linspecer(Zonenummber*5);
for cnt = 1:1:vueNum
    ZoneId = zoneId(cnt,1)+1;
    plot(absX(cnt,1),absY(cnt,1),'X','Color',RandomColor(ZoneId*5,:));hold on
end
l2=legend('Zone1','Zone2','Zone3','Zone4','Zone5',1);
title('高速公路不同拥塞等级车辆分布图','fontweight','bold')
xlabel('x/m','fontweight','bold');
ylabel('y/m','fontweight','bold');
axis([-2200,2200,-15,15]);
set(l2,'Fontname', 'Times New Roman','FontWeight','bold')


