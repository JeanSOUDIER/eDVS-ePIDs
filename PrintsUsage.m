function PrintsUsage(time, data, diff, name)
    loadTracker = sum(time)/(data(length(data),4)-data(1,4));
    fprintf(strcat(name,' load %f %%\n'),loadTracker);
    MeanTimeTracker = sum(diff(1:length(diff)-1))/(length(diff)-1);
    MaxEventRateTracker = 1/(MeanTimeTracker*1e-3);
    fprintf(strcat('Mean time'," ",name,' %f us, %f Kevts/s\n'),MeanTimeTracker,MaxEventRateTracker);
end
