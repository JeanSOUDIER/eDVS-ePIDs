function WriteInfos(filename, mini, maxi, means, mini2, maxi2, means2, P)
    fileID = fopen(strcat('fig/',filename,'.txt'),'w');
    fprintf(fileID,'Controller ball : min %d us, mean %d us, max %d us\nController Motor : min %d us, mean %d us, max %d us\nPower consumption : %f W',mini,means,maxi,mini2,means2,maxi2,P);
    fclose(fileID);
end
