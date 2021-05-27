function [var, res, diff, grapher] = ReadExistingCSVfiles(file, Nfile, Ttime, grapher)
    if isfile(strcat('PFE/files/',file,'_',Nfile,'.csv')) ~= 0 %if file exist
        var = ReadCSVfiles(file, Nfile);  %read it
        grapher = grapher+1;                       %add 1 to graph size
        [res, diff] = ComputeTimes(var, Ttime);
    else
        var = 0;
        res = 0;
        diff = 0;
    end
end
