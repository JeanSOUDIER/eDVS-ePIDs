function [var, res, diff, grapher] = ReadExistingCSVfiles(file, Nfile, Ttime, grapher)
    if isfile(strcat('PFE/files/Read_timing_',Nfile,'.csv')) ~= 0 %if file exist
    var = ReadCSVfiles('Read_timing', Nfile);  %read it
    grapher = grapher+1;                       %add 1 to graph size
    [res, diff] = ComputeTimes(var, Ttime);
end
