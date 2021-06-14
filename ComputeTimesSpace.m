function [mini maxi] = ComputeTimesSpace(var, Ttime)
    res = [];
    for i = 1:length(var)-6500
        res = [res ; var(i+1,1)-var(i,1)];
    end
    mini = min(res);
    maxi = max(res);
end