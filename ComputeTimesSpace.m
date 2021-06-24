function [mini maxi means] = ComputeTimesSpace(var, Ttime)
    res = [];
    for i = 1:length(var)-5000
        res = [res ; var(i+1,1)-var(i,1)];
    end
    mini = min(res);
    maxi = max(res);
    means = mean(res);
end