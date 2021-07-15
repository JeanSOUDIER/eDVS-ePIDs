function WriteEvts(filename, vect)
    % nb evts : Dsensor / Dused / PIDbille / PIDmot
    fileID = fopen(strcat('fig/',filename,'.txt'),'w');
    fprintf(fileID,'eDVS evts : %d \nCluster evts : %d \nPID bille evts : %d \nPID mot evts : %d \n',vect(1),vect(2),vect(3),vect(4));
    fclose(fileID);
end