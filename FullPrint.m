clear all;
close all;
clc;

%serie of files to read
Nfile = '1';
Mode = "DVS";
Mode2 = "ePID";
SubMode = 0;

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
    switch SubMode
        case 0
            Data = ReadCSVfiles('ePID_points', Nfile);
            Tcontroler = ReadCSVfiles('ePID_timing', Nfile);
        case 1
            Data = ReadCSVfiles('ePID_points1', Nfile);
            Tcontroler = ReadCSVfiles('ePID_timing1', Nfile);
        case 2
            Data = ReadCSVfiles('ePID_points2', Nfile);
            Tcontroler = ReadCSVfiles('ePID_timing2', Nfile);
        otherwise
            Data = ReadCSVfiles('ePID_points1', Nfile);
            Tcontroler = ReadCSVfiles('ePID_timing1', Nfile);
            Data2 = ReadCSVfiles('ePID_points2', Nfile);
            Tcontroler2 = ReadCSVfiles('ePID_timing2', Nfile);
    end
elseif(Mode2 == "PID")
    switch SubMode
        case 0
            Data = ReadCSVfiles('PID_points', Nfile);
            Tcontroler = ReadCSVfiles('PID_timing', Nfile);
        case 1
            Data = ReadCSVfiles('PID_points1', Nfile);
            Tcontroler = ReadCSVfiles('PID_timing1', Nfile);
        case 2
            Data = ReadCSVfiles('PID_points2', Nfile);
            Tcontroler = ReadCSVfiles('PID_timing2', Nfile);
        otherwise
            Data = ReadCSVfiles('PID_points1', Nfile);
            Tcontroler = ReadCSVfiles('PID_timing1', Nfile);
            Data2 = ReadCSVfiles('PID_points2', Nfile);
            Tcontroler2 = ReadCSVfiles('PID_timing2', Nfile);
    end
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
if(Mode2 ~= "NONE")
    grapher = grapher+3;
    [Thard, Yhard, TdiffHard, grapher] = ReadExistingCSVfiles('hard_timing', Nfile, Ttime, grapher);
    [Ycontroler, Tdiffconstroler] = ComputeTimes(Tcontroler, Ttime);
    if(SubMode > 2)
        grapher = grapher+3;
        [Ycontroler2, Tdiffconstroler2] = ComputeTimes(Tcontroler2, Ttime);
    end
end



    %first plot
    figure(1);
    tiledlayout(grapher,1);
    nexttile;
    title('response');
    xlabel('time');
    ylabel('position [mm]');
    axi = Ttime(:,2)-Ttime(:,1);

    if(Mode2 ~= "NONE")
        %plot response
        hold on;
        stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,1),'-or');
        xlim([0 axi]);
        ylim([-70 70]);
        stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,2),'-og');
        xlim([0 axi]);
        ylim([-70 70]);
        legend({'Y','Ysp'},'Location','northeast');
        hold off;

        %plot command
        nexttile;
        stairs(Data(:,4)-Ttime(:,1)*ones(length(Data),1),Data(:,3),'-ob');
        legend({'U'},'Location','northeast');

        if SubMode > 2
            %plot response
            hold on;
            stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,1),'-or');
            xlim([0 axi]);
            ylim([-70 70]);
            stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,2),'-og');
            xlim([0 axi]);
            ylim([-70 70]);
            legend({'Y','Ysp'},'Location','northeast');
            hold off;

            %plot command
            nexttile;
            stairs(Data2(:,4)-Ttime(:,1)*ones(length(Data2),1),Data2(:,3),'-ob');
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
            plotUsage('CPU load form appling command', Ysensor, axi);
        end
    end

    if(Mode ~= "NONE")
        %second plot
        figure(2);
        hold on;
        title('Points cloud');
        xlabel('x piexls');
        ylabel('y piexls');
        zlabel('time [us]');
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
    end

if(Mode ~= "NONE")
    PrintsUsage(Ysensor, Dsensor, TdiffSensor, 'Tracker');
    PrintExistingUsage('Read_timing', Nfile, TdiffRead, 'reading');
end
if(Mode2 ~= "NONE")
    PrintsUsage(Ycontroler, Dtracker, Tdiffconstroler, 'Controller');
    if SubMode > 2
        PrintsUsage(Ycontroler2, Dtracker, Tdiffconstroler2, 'Controller2');
    end
    PrintExistingUsage('hard_timing', Nfile, TdiffHard, 'command apply');
end
