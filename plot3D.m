clear all;
close all;

Nfile = '0';

Ttime = table2array(readtable(strcat('files/Time_',Nfile,'.csv')));
Dsensor = table2array(readtable(strcat('files/DVS_points_',Nfile,'.csv')));
Dtracker = table2array(readtable(strcat('files/Cluster_points_',Nfile,'.csv')));

hold on;
plot3(Dtracker(:,1),Dtracker(:,2),Dtracker(:,4)-Ttime(:,1)*ones(length(Dtracker),1),'ro');
plot3(Dsensor(:,1),Dsensor(:,2),Dsensor(:,4)-Ttime(:,1)*ones(length(Dsensor),1),'ko');
view(3);
hold off;
