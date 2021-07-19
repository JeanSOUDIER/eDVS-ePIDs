clc;
clear all;
close all;

Power = table2array(readtable(strcat('PFE/files/scope_8.csv')));

plot(Power(:,1),Power(:,2),'-or');

P = trapz(Power(:,1),Power(:,2));
fprintf('%f W\n',P);
