clear all;
close all;
clc;

Nfile = '6';

Ttime = table2array(readtable(strcat('files/Time_',Nfile,'.csv')));
Data = table2array(readtable(strcat('files/ePID_points_',Nfile,'.csv')));
Tsensor = table2array(readtable(strcat('files/DVS_timing_',Nfile,'.csv')));
Tcontroler = table2array(readtable(strcat('files/ePID_timing_',Nfile,'.csv')));
Dsensor = table2array(readtable(strcat('files/DVS_points_',Nfile,'.csv')));
Dtracker = table2array(readtable(strcat('files/Cluster_points_',Nfile,'.csv')));
grapher = 4;
if isfile(strcat('files/Read_timing_',Nfile,'.csv')) ~= 0
    Tread = table2array(readtable(strcat('files/Read_timing_',Nfile,'.csv')));
    grapher = grapher+1;
    Yread = zeros(1,Ttime(:,2)-Ttime(:,1));
    TdiffRead = Tread(:,2)-Tread(:,1);
    for i = 1:length(Tread)
        Yread(:,Tread(i,1)-Ttime(:,1):Tread(i,1)-Ttime(:,1)+TdiffRead(i,1)) = ones(1,TdiffRead(i,1)+1);
    end
end
if isfile(strcat('files/hard_timing_',Nfile,'.csv')) ~= 0
    Thard = table2array(readtable(strcat('files/hard_timing_',Nfile,'.csv')));
    grapher = grapher+1;
    Yhard = zeros(1,Ttime(:,2)-Ttime(:,1));
    TdiffHard = Thard(:,2)-Thard(:,1);
    for i = 1:length(Thard)
        Yhard(:,Thard(i,1)-Ttime(:,1):Thard(i,1)-Ttime(:,1)+TdiffHard(i,1)) = ones(1,TdiffHard(i,1)+1);
    end
end

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

Dused = [];
Dunused = [];
for i = 1:length(Dsensor)
    if Dsensor(i,3) == 1
        Dused = [Dused; Dsensor(i,:)];
    else
        Dunused = [Dunused; Dsensor(i,:)];
    end
end

axi = Ttime(:,2)-Ttime(:,1);

figure(1);
tiledlayout(grapher,1);
nexttile;
hold on;
title('Responses');
xlabel('time');
ylabel('position [mm]');
stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,1),'-or');
xlim([0 axi]);
ylim([-70 70]);
stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,2),'-og');
xlim([0 axi]);
ylim([-70 70]);
legend({'Y','Ysp'},'Location','northeast');
hold off;
nexttile;
stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,3),'-ob');
legend({'U'},'Location','northeast');

if isfile(strcat('files/Read_timing_',Nfile,'.csv')) ~= 0
    nexttile;
    plot(Yread);
    title('CPU load form reading USB');
    xlabel('time');
    ylabel('CPU used [bool]');
    axis([0 axi -0.1 1.1]);
end

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

if isfile(strcat('files/hard_timing_',Nfile,'.csv')) ~= 0
    nexttile;
    plot(Yhard);
    title('CPU load form appling command');
    xlabel('time');
    ylabel('CPU used [bool]');
    axis([0 axi -0.1 1.1]);
end

figure(2);
hold on;
title('Points cloud');
xlabel('x piexls');
ylabel('y piexls');
zlabel('time [us]');
plot3(Dtracker(:,1),Dtracker(:,2),Dtracker(:,4)-Ttime(:,1)*ones(length(Dtracker),1),'or');
list = "Cluster tracking";
if length(Dunused) ~= 0
    plot3(Dunused(:,1),Dunused(:,2),Dunused(:,4)-Ttime(:,1)*ones(length(Dunused),1),'ok');
    list = [list "Received events"];
end
if length(Dused) ~= 0
    plot3(Dused(:,1),Dused(:,2),Dused(:,4)-Ttime(:,1)*ones(length(Dused),1),'og');
    list = [list "Used events"];
end
%legend({'Cluster tracking','Received events','Used events'},'Location','northeast');
legend(list,'Location','northeast');
view(3);
hold off;

loadTracker = sum(Ysensor)/(Dsensor(length(Dsensor),4)-Dsensor(1,4));
loadController = sum(Ycontroler)/(Dtracker(length(Dtracker),4)-Dtracker(1,4));

fprintf('Tracker load %f %%\n',loadTracker);
fprintf('Controller load %f %%\n',loadController);

MeanTimeTracker = sum(TdiffSensor(1:length(TdiffSensor)-1))/(length(TdiffSensor)-1);
MaxEventRateTracker = 1/(MeanTimeTracker*1e-3);
MeanTimeController = sum(Tdiffconstroler(1:length(Tdiffconstroler)-1))/(length(Tdiffconstroler)-1);
MaxEventRateController = 1/(MeanTimeController*1e-3);

fprintf('Mean time tracker %f us, %f Kevts/s\n',MeanTimeTracker,MaxEventRateTracker);
fprintf('Mean time controller %f us, %f Kevts/s\n',MeanTimeController,MaxEventRateController);

if isfile(strcat('files/Read_timing_',Nfile,'.csv')) ~= 0
    MeanTimeRead = sum(TdiffRead(1:length(TdiffRead)-1))/(length(TdiffRead)-1);
    MaxEventRateRead = 1/(MeanTimeRead*1e-3);
    fprintf('Mean time reading %f us, %f Kevts/s\n',MeanTimeRead,MaxEventRateRead);
end

if isfile(strcat('files/hard_timing_',Nfile,'.csv')) ~= 0
    MeanTimeHard = sum(TdiffHard(1:length(TdiffHard)-1))/(length(TdiffHard)-1);
    MaxEventRateHard = 1/(MeanTimeHard*1e-3);
    fprintf('Mean time command apply %f us, %f Kevts/s\n',MeanTimeHard,MaxEventRateHard);
end
