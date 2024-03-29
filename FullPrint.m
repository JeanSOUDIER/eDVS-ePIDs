try
    parpool;
catch e
end

clear all;
close all;
clc;

%serie of files to read
Nfile = '0';
Mode = "DVS";        % DVS ; Cam
Mode2 = "ePID";      % PID ; ePID
Mode3 = "ePID";      % PID ; ePID
Mode4 = "sensor";    % sensor
Mode5 = "DVS";       % DVS ; (Cam) => GT sensor for IAE and IAU (only Mode=="Cam")

% possible configurations
%DVS ; ePID ; ePID ; sensor
%DVS ;  PID ;  PID ; sensor
%Cam ; ePID ; ePID ; none
%Cam ;  PID ;  PID ; none

%reading files
Ttime = ReadCSVfiles('Time', Nfile);
if(Mode == "DVS")
    Tsensor = ReadCSVfiles('DVS_timing', Nfile);
    Dsensor = ReadCSVfiles('DVS_points', Nfile);
    Dtracker = ReadCSVfiles('Cluster_points', Nfile);
elseif(Mode == "Cam")
    Tsensor = ReadCSVfiles('Cam_timing', Nfile);
    Dsensor = ReadCSVfiles('Cam_points', Nfile);
end
if(Mode2 == "ePID")
    Data = ReadCSVfiles('ePID_points0', Nfile);
    Tcontroler = ReadCSVfiles('ePID_timing0', Nfile);
    if(Mode3 == "ePID")
        Data2 = ReadCSVfiles('ePID_points1', Nfile);
        Tcontroler2 = ReadCSVfiles('ePID_timing1', Nfile);
        SubMode = 3;
    elseif(Mode3 == "PID")
        Data2 = ReadCSVfiles('PID_points1', Nfile);
        Tcontroler2 = ReadCSVfiles('PID_timing1', Nfile);
        SubMode = 3;
    else
        SubMode = 2;
    end
    if(Mode == "Cam")
        if(Mode5 == "DVS")
            Ttime2 = ReadCSVfiles('Time2', Nfile);
            Dtracker = ReadCSVfiles('Cluster_points', Nfile);
        else
            Dtracker = Data;
            Dtracker(:,3) = Dtracker(:,2);
        end
    end
elseif(Mode2 == "PID")
    Data = ReadCSVfiles('PID_points0', Nfile);
    Tcontroler = ReadCSVfiles('PID_timing0', Nfile);
    if(Mode3 == "ePID")
        Data2 = ReadCSVfiles('ePID_points1', Nfile);
        Tcontroler2 = ReadCSVfiles('ePID_timing1', Nfile);
        SubMode = 3;
    elseif(Mode3 == "PID")
        Data2 = ReadCSVfiles('PID_points1', Nfile);
        Tcontroler2 = ReadCSVfiles('PID_timing1', Nfile);
        SubMode = 3;
    else
        SubMode = 2;
    end
    if(Mode == "Cam")
        if(Mode5 == "DVS")
            Ttime2 = ReadCSVfiles('Time2', Nfile);
            Dtracker = ReadCSVfiles('Cluster_points', Nfile);
        else
            Dtracker = Data;
            Dtracker(:,3) = Dtracker(:,2);
        end
    end
else
    if(Mode3 == "ePID")
        Data = ReadCSVfiles('ePID_points1', Nfile);
        Tcontroler = ReadCSVfiles('ePID_timing1', Nfile);
        SubMode = 1;
    elseif(Mode3 == "PID")
        Data = ReadCSVfiles('PID_points1', Nfile);
        Tcontroler = ReadCSVfiles('PID_timing1', Nfile);
        SubMode = 1;
    end
end

if(Mode4 == "sensor")
    axi = Ttime(:,2)-Ttime(:,1);
    PotData = ReadCSVfiles('Pot_points', Nfile);
    figure(5);
    hold on;
    stairs(PotData(:,4)-Ttime(:,1)*ones(length(PotData),1),PotData(:,1),'-or');
    stairs(PotData(:,4)-Ttime(:,1)*ones(length(PotData),1),PotData(:,2),'-og');
    xlim([0 axi]);
    ProperYaxisMulti(PotData(:,1), PotData(:,2));
    legend({'Y','Ysp'},'Location','northeast');
    title('Sensor motor');
    xlabel('time [us]');
    ylabel('position [rad]');
    hold off;
    set(gcf, 'PaperSize', [20 20]);
    print(gcf, 'fig/SensorPot.pdf', '-dpdf', '-fillpage');
    
end

%CPU usage graph size
grapher = 0;
if(Mode ~= "NONE")
    grapher = grapher+1;
    [Tread, Yread, TdiffRead, grapher] = ReadExistingCSVfiles('Read_timing', Nfile, Ttime, grapher);
    [Ysensor, TdiffSensor] = ComputeTimes(Tsensor, Ttime);

    %Compute the cloud used for clustering and not
    Dused = [];
    Dunused = [];
    for i = 1:length(Dsensor)
        if Dsensor(i,3) == 1
            Dused = [Dused; Dsensor(i,:)];
        else
            Dunused = [Dunused; Dsensor(i,:)];
        end
    end
end
DataGT = ReadExistingCSVfilesNoTime('GTsensor', Nfile, Ttime);
if(Mode2 ~= "NONE")
    grapher = grapher+3;
    [Thard, Yhard, TdiffHard, grapher] = ReadExistingCSVfiles('hard_timing', Nfile, Ttime, grapher);
    [Ycontroler, Tdiffconstroler] = ComputeTimes(Tcontroler, Ttime);
    if(SubMode > 2)
        grapher = grapher+3;
        [Ycontroler2, Tdiffconstroler2] = ComputeTimes(Tcontroler2, Ttime);
    end
elseif(Mode3 ~= "NONE")
    grapher = grapher+3;
    [Ycontroler, Tdiffconstroler] = ComputeTimes(Tcontroler, Ttime);
end

if(Mode4 == "sensor")
    figure(6);
    hold on;
    stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,2),'-or');
    if(Mode == "DVS")
        stairs(Dtracker(:,4)-Ttime(:,1)*ones(length(Dtracker),1),-151/150*Dtracker(:,1)+64,'-og');
    else
        if(Mode5 == "DVS")
            stairs(Dtracker(:,4)-Ttime2(:,1)*ones(length(Dtracker),1),-151/150*Dtracker(:,1)+64,'-og');
        else
            stairs(Dtracker(:,4)-Ttime(:,1)*ones(length(Dtracker),1),Dtracker(:,1),'-og');
        end
    end
    xlim([0 axi]);
    if(Mode5 == "DVS")
        ProperYaxisMulti(Data(:,2), -151/150*Dtracker(:,1)+64);
    else
        ProperYaxisMulti(Data(:,2), Dtracker(:,1));
    end
    legend({'Ysp','Y'},'Location','northeast');
    title('Sensor ball');
    xlabel('time [us]');
    ylabel('position [mm]');
    hold off;
    set(gcf, 'PaperSize', [20 20]);
    print(gcf, 'fig/SensorDVS.pdf', '-dpdf', '-fillpage');
end


    %first plot
    axi = Ttime(:,2)-Ttime(:,1);
    if(Mode2 ~= "NONE")
        figure(3);
        hold on;
        stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,1),'-or');
        stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,2),'-og');
        %stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,3),'-ob');
        if(DataGT ~= 0)
            stairs(DataGT(:,2)-Ttime(:,1)*ones(length(DataGT),1),140*ones(length(DataGT),1)-DataGT(:,1),'-om');
        end
        xlim([0 axi]);
        ProperYaxisMulti(Data(:,1), Data(:,2));
        legend({'Y','Ysp'},'Location','northeast');
        hold off;
        title('Controller ball');
        xlabel('time [us]');
        ylabel('position [mm]');
        set(gcf, 'PaperSize', [20 20]);
        print(gcf, 'fig/PIDbille.pdf', '-dpdf', '-fillpage');
        
        figure(4);
        hold on;
        stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,1),'-or');
        stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,2),'-og');
        %stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,3),'-ob');
        xlim([0 axi]);
        ProperYaxisMulti(Data2(:,1), Data2(:,2));
        legend({'Y','Ysp'},'Location','northeast');
        hold off;
        title('Controller motor');
        xlabel('time [us]');
        ylabel('position [rad]');
        set(gcf, 'PaperSize', [20 20]);
        print(gcf, 'fig/PIDmot.pdf', '-dpdf', '-fillpage');
    end
    
    figure(1);
    tiledlayout(grapher,1);
    nexttile;
    title('response');
    xlabel('time');
    ylabel('position [mm]');

    if(Mode2 ~= "NONE")
        %plot response
        
        hold on;
        stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,1),'-or');
        xlim([0 axi]);
        stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,2),'-og');
        xlim([0 axi]);
        ProperYaxisMulti(Data(:,1), Data(:,2));
        legend({'Y','Ysp'},'Location','northeast');
        hold off;

        %plot command
        nexttile;
        stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,3),'-ob');
        xlim([0 axi]);
        ProperYaxis(Data(:,3));
        legend({'U'},'Location','northeast');

        if SubMode > 2
            %plot response
            nexttile;
            hold on;
            stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,1),'-or');
            xlim([0 axi]);
            stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,2),'-og');
            xlim([0 axi]);
            ProperYaxisMulti(Data2(:,1), Data2(:,2));
            legend({'Y','Ysp'},'Location','northeast');
            hold off;

            %plot command
            nexttile;
            stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,3),'-ob');
            xlim([0 axi]);
            ProperYaxis(Data2(:,3));
            legend({'U'},'Location','northeast');
        end
    end
    
    %plot usages
    if(Mode ~= "NONE")
        if isfile(strcat('PFE/files/Read_timing_',Nfile,'.csv')) ~= 0
            plotUsage('CPU load form reading USB', Yread, axi);
        end
        plotUsage('CPU load form object tracker', Ysensor, axi);
    end
    if(Mode2 ~= "NONE")
        plotUsage('CPU load form controller', Ycontroler, axi);
        if SubMode > 2
            plotUsage('CPU load form controller2', Ycontroler2, axi);
        end
        if isfile(strcat('PFE/files/hard_timing_',Nfile,'.csv')) ~= 0
            plotUsage('CPU load form appling command', Yhard, axi);
        end
    end

    if(Mode == "DVS")
        %second plot
        figure(2);
        hold on;
        plot3(Dtracker(:,1),Dtracker(:,2),Dtracker(:,4)-Ttime(:,1)*ones(length(Dtracker),1),'or');
        list = "Cluster tracking";
        if length(Dunused) ~= 0 %not empty
            plot3(Dunused(:,1),Dunused(:,2),Dunused(:,4)-Ttime(:,1)*ones(length(Dunused),1),'ok');
            list = [list "Received events"];
        end
        if length(Dused) ~= 0
            plot3(Dused(:,1),Dused(:,2),Dused(:,4)-Ttime(:,1)*ones(length(Dused),1),'og');
            list = [list "Used events"];
        end
        legend(list,'Location','northeast');
        view(3);
        hold off;
        title('Points cloud');
        xlabel('x piexls');
        ylabel('y piexls');
        zlabel('time [us]');
        set(gcf, 'PaperSize', [20 20]);
        print(gcf, 'fig/Points_cloud.pdf', '-dpdf', '-fillpage');
    end

if(Mode == "DVS")
    PrintsUsage(Ysensor, Dsensor, TdiffSensor, 'Tracker');
end
if(Mode ~= "NONE")
    PrintExistingUsage('Read_timing', Nfile, TdiffRead, 'reading');
end
if(Mode2 ~= "NONE")
    PrintsUsage(Ycontroler, Data, Tdiffconstroler, 'Controller');
    fprintf('%d evts\n',length(Data));
    [mini maxi means] = ComputeTimesSpace(Tcontroler);
    fprintf('min time %f ms\n',mini/1000);
    fprintf('max time %f ms\n',maxi/1000);
    fprintf('mean time %f ms\n',means/1000);
    
    if(Mode == "DVS")
        error = Dtracker(:,3)-(-151/150*Dtracker(:,1)+64);
        IAE = trapz(Dtracker(:,4)-Ttime(:,1)*ones(length(Dtracker),1),abs(error));
    else
        if(Mode5 == "DVS")
            error = Dtracker(:,3)-(-151/150*Dtracker(:,1)+64);
            IAE = trapz(Dtracker(:,4)-Ttime2(:,1)*ones(length(Dtracker),1),abs(error));
        else
            error = Data(:,2)-Data(:,1);
            IAE = trapz(Data(:,4)-Ttime(:,1)*ones(length(Data),1),abs(error));
        end
    end
    fprintf('IAE ball %e\n',IAE);
    IAU = trapz(Data(:,4)-Ttime(:,1)*ones(length(Data),1),abs(Data(:,3)));
    fprintf('IAU ball %e\n',IAU);
    
    if SubMode > 2
        PrintsUsage(Ycontroler2, Data2, Tdiffconstroler2, 'Controller2');
        fprintf('%d evts\n',length(Data2));
        [mini2 maxi2 means2] = ComputeTimesSpace(Tcontroler2);
        fprintf('2 min time %f ms\n',mini2/1000);
        fprintf('2 max time %f ms\n',maxi2/1000);
        fprintf('2 mean time %f ms\n',means2/1000);
        
        error2 = PotData(:,2)-PotData(:,1);
        IAE2 = trapz(PotData(:,4)-Ttime(:,1)*ones(length(PotData),1),abs(error2));
        fprintf('IAE motor %e\n',IAE2);
        IAU2 = trapz(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),abs(Data2(:,3)));
        fprintf('IAU motor %e\n',IAU2);
    end
    PrintExistingUsage('hard_timing', Nfile, TdiffHard, 'command apply');
end

WriteEvts('events',[length(Dsensor) length(Dused) length(Data) length(Data2)]);

P = CurrentScope(Nfile);

if(SubMode > 2)
    WriteInfos('infos',mini, maxi, means, mini2, maxi2, means2, P, IAE, IAE2, IAU, IAU2);
end
