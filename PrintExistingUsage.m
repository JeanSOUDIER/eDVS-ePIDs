function PrintExistingUsage(fileName, Nfile, diff, name)
    if isfile(strcat('PFE/files/',fileName,'_',Nfile,'.csv')) ~= 0
        MeanTimeRead = sum(diff(1:length(diff)-1))/(length(diff)-1);
        MaxEventRateRead = 1/(MeanTimeRead*1e-3);
        fprintf(strcat('Mean time'," ",name,' %f us, %f Kevts/s\n'),MeanTimeRead,MaxEventRateRead);
    end
end
