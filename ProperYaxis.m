function ProperYaxis(data)
    m = min(data);
    pourcent = 0.2;
    if m < 0
        m = m*(1+pourcent);
    else
        m = m*(1-pourcent);
    end
    ylim([m max(data)*(1+pourcent)]);
end
