function [res, diff] = ComputeTimes(var, Ttime)
    res = zeros(1,Ttime(:,2)-Ttime(:,1));      %create a zero vector
    diff = var(:,2)-var(:,1);                  %compute time differences
    for i = 1:length(var)
        res(:,var(i,1)-Ttime(:,1):var(i,1)-Ttime(:,1)+diff(i,1)) = ones(1,diff(i,1)+1); %create the graph 1 for computing 0 elsewhere
    end
end
