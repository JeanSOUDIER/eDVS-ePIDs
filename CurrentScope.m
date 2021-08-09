function P = CurrentScope(nb_file)

    Power = table2array(readtable(strcat('PFE/files/scope_',nb_file,'.csv')));

    figure(10);
    plot(Power(:,1),Power(:,5),'-or');
    title('Power consumption');
    xlabel('time [s]');
    ylabel('power [W]');
    set(gcf, 'PaperSize', [20 20]);
    print(gcf, 'fig/power.pdf', '-dpdf', '-fillpage');

    P = abs(trapz(Power(:,1),Power(:,5)));
    fprintf('%f W\n',P);
end
