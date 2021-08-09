function WriteInfos(filename, mini, maxi, means, mini2, maxi2, means2, P, IAE, IAE2, IAU, IAU2)
    fileID = fopen(strcat('fig/',filename,'.txt'),'w');
    fprintf(fileID,'Controller ball : min %d us, mean %d us, max %d us\nController Motor : min %d us, mean %d us, max %d us\nPower consumption : %f W\nIAE ball : %f (%e), IAE motor : %f (%e)\nIAU ball : %f (%e), IAU motor : %f (%e)',mini,means,maxi,mini2,means2,maxi2,P,IAE,IAE,IAE2,IAE2,IAU,IAU,IAU2,IAU2);
    fclose(fileID);
end
