function ProperYaxisMulti(data1, data2)
    m = min(data1);
    m2 = min(data2);
    m = min(m,m2);
    pourcent = 0.2;
    if m < 0
        m = m*(1+pourcent);
    else
        m = m*(1-pourcent);
    end
    ma = max(data1);
    ma2 = max(data2);
    ma = max(ma,ma2);
    ylim([m ma*(1+pourcent)]);
end
