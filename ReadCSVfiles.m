function var = ReadCSVfiles(file, Nfile)
    var = table2array(readtable(strcat('PFE/files/',file,'_',Nfile,'.csv')));
end
