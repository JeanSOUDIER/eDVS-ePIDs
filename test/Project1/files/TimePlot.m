clear all;
close all;

Nfile = '0';

Ttime = table2array(readtable(strcat('Time_',Nfile,'.csv')));
Tsensor = table2array(readtable(strcat('DVS_timing_',Nfile,'.csv')));
Tcontroler = table2array(readtable(strcat('ePID_timing_',Nfile,'.csv')));

Ysensor = zeros(1,Ttime(:,2)-Ttime(:,1));
TdiffSensor = Tsensor(:,2)-Tsensor(:,1);
for i = 1:length(Tsensor)
    Ysensor(:,Tsensor(i,1)-Ttime(:,1):Tsensor(i,1)-Ttime(:,1)+TdiffSensor(i,1)) = ones(1,TdiffSensor(i,1)+1);
end

Ycontroler = zeros(1,Ttime(:,2)-Ttime(:,1));
Tdiffconstroler = Tcontroler(:,2)-Tcontroler(:,1);
for i = 1:length(Tcontroler)
    Ycontroler(:,Tcontroler(i,1)-Ttime(:,1):Tcontroler(i,1)-Ttime(:,1)+Tdiffconstroler(i,1)) = ones(1,Tdiffconstroler(i,1)+1);
end

hold on;
plot(Ysensor);
axis([0 length(Ysensor) -0.1 1.1]);
plot(Ycontroler);
axis([0 length(Ycontroler) -0.1 1.1]);
hold off;
