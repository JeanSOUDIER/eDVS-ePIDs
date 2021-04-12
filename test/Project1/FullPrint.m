clear all;
close all;
clc;

Nfile = '1';

Ttime = table2array(readtable(strcat('files/Time_',Nfile,'.csv')));
Data = table2array(readtable(strcat('files/ePID_points_',Nfile,'.csv')));
Tsensor = table2array(readtable(strcat('files/DVS_timing_',Nfile,'.csv')));
Tcontroler = table2array(readtable(strcat('files/ePID_timing_',Nfile,'.csv')));
Dsensor = table2array(readtable(strcat('files/DVS_points_',Nfile,'.csv')));
Dtracker = table2array(readtable(strcat('files/Cluster_points_',Nfile,'.csv')));

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

axi = Ttime(:,2)-Ttime(:,1);

figure(1);
tiledlayout(3,1);
nexttile;
hold on;
title('Responses');
xlabel('time');
ylabel('position [mm]');
plot(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,1),'-or');
xlim([0 axi]);
plot(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,2),'-og');
xlim([0 axi]);
plot(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,3),'-ob');
xlim([0 axi]);
legend({'Y','Ysp','U'},'Location','northeast');
hold off;

nexttile;
plot(Ysensor);
title('CPU load form object tracker');
xlabel('time');
ylabel('CPU used [bool]');
axis([0 axi -0.1 1.1]);

nexttile;
plot(Ycontroler);
title('CPU load form controller');
xlabel('time');
ylabel('CPU used [bool]');
axis([0 axi -0.1 1.1]);

figure(2);
hold on;
title('Points cloud');
xlabel('x piexls');
ylabel('y piexls');
zlabel('time [us]');
plot3(Dtracker(:,1),Dtracker(:,2),Dtracker(:,4)-Ttime(:,1)*ones(length(Dtracker),1),'or');
plot3(Dsensor(:,1),Dsensor(:,2),Dsensor(:,4)-Ttime(:,1)*ones(length(Dsensor),1),'ok');
legend({'Cluster tracking','Received events'},'Location','northeast');
view(3);
hold off;

loadTracker = sum(Ysensor)/(Dsensor(length(Dsensor),4)-Dsensor(1,4));
loadController = sum(Ycontroler)/(Dtracker(length(Dtracker),4)-Dtracker(1,4));

fprintf('Tracker load %f %%\n',loadTracker);
fprintf('Controller load %f %%\n',loadController);

MeanTimeTracker = sum(TdiffSensor)/length(TdiffSensor);
MaxEventRateTracker = 1/(MeanTimeTracker*1e-3);
MeanTimeController = sum(Tdiffconstroler)/length(Tdiffconstroler);
MaxEventRateController = 1/(MeanTimeController*1e-3);

fprintf('Mean time tracker %f us, %f Kevts/s\n',MeanTimeTracker,MaxEventRateTracker);
fprintf('Mean time controller %f us, %f Kevts/s\n',MeanTimeController,MaxEventRateController);
