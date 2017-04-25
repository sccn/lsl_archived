%% change to this directory
cd 'C:\Tobii\Biometric Solutions\Biometric Manufacturers\LSL\LabRecorder';

%% load the file
s = load_xdf('C:\Tobii\Biometric Solutions\Biometric Manufacturers\LSL\Glasses & LSL\DavidMedine files\Glasses_gp_3.xdf');

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

subplot(3,1,1)
plot(diff(et(1,:)/1000000), 'linewidth', 2)
title('tobii timestamps')
ylabel('first difference (s)');
set(gca, 'fontsize',11);


xlim([0 1200]);

subplot(3,1,2)
plot(diff(et_times), 'linewidth', 2)
title('lsl timestamps')
ylabel('first difference (s)')
set(gca, 'fontsize', 11)
xlabel('sample number');
xlim([0 1200]);

subplot(3,1,3)
plot(diff(et(4,:)/1000000), 'linewidth', 2)
title('latency')
ylabel('(s)')
set(gca, 'fontsize', 11)
xlabel('sample number');
xlim([0 1200]);

figure

plot(et_times-et_times(1), (et(2,:)/1000000), 'linewidth', 2)
title('x axis')
ylabel('X (mm)');
set(gca, 'fontsize',11);
hold on 
plot((et(1,:) - et(1,1))/1000000, (et(2,:)/1000000),'r', 'linewidth', 2)
legend('show')
legend('LSL timestamps','Glasses timestamps')

