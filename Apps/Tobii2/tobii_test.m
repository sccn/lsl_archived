%% change to this directory
cd 'C:\\Users\David.Medine\matlab';

%% load the file
s = load_xdf('../xdf_files/tobii_test.xdf','HandleJitterRemoval', false);

%% hunt for the stream indeces
for n=1:length(s)
    if strcmp('EyeTracking', s{n}.info.type)
        et_idx = n;
    end
    
    
end

%
et = s{et_idx}.time_series;
et_times = s{et_idx}.time_stamps;

figure

subplot(2,1,1)
plot(diff(et(1,:)/1000000), 'linewidth', 2)
title('tobii timestamps')
ylabel('first difference (s)');
set(gca, 'fontsize',11);

xlim([0 600]);

subplot(2,1,2)
plot(diff(et_times), 'linewidth', 2)
title('lsl timestamps')
ylabel('first difference (s)')
set(gca, 'fontsize', 11)
xlabel('sample number');
xlim([0 600]);



