function var = ReadExistingCSVfilesNoTime(file, Nfile, Ttime)
    if isfile(strcat('PFE/files/',file,'_',Nfile,'.csv')) ~= 0 %if file exist
        var = ReadCSVfiles(file, Nfile);  %read it
    else
        var = 0;
    end
end
