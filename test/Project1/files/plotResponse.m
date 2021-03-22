clear all;
close all;

Nfile = '0';

Ttime = table2array(readtable(strcat('Time_',Nfile,'.csv')));
Data = table2array(readtable(strcat('ePID_points_',Nfile,'.csv')));

hold on;
plot(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,1),'-r');
plot(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,2),'-g');
plot(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,3),'-b');
hold off;