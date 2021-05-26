function plotUsage(titleName, data, axi)
    nexttile;
    plot(data);
    title(titleName);
    xlabel('time');
    ylabel('CPU used [bool]');
    axis([0 axi -0.1 1.1]);
end
