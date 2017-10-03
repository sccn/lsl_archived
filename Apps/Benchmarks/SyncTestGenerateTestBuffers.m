close all;
clear;
clc;

Pin.nominal_srate = [50,100]; %,200,500,1000,2000]; %:1000:5000]; %in Hz
Pin.error = 0.01;            % in percent (100ppm)
Pin.duration = 1 * 60 * 60;  % in sec
Pin.warble = 2/1000;         % in sec
Pin.warble_freq = 1/(30*60); % in Hz
Pin.drift = 2/1000;          % in sec
Pin.jitter = 8/1000;         % in sec
Pin.chunk_lens = [1, 10, 25, 50, 100]/1000;  % in sec
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
casedir='.';
if(~isdir(casedir)) mkdir(casedir); end

for i=1 : length(Pin.nominal_srate)
    fprintf('generating for Fs=%.1f...\n',Pin.nominal_srate(i));
    Pblock.nominal_srate = Pin.nominal_srate(i);
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Initializing buffers
    zfname = strcat(casedir,'NR',num2str(Pblock.nominal_srate),'_ZMatrix');
    yfname = strcat(casedir,'NR',num2str(Pblock.nominal_srate),'_YMatrix');
    
    infname = strcat(casedir,'NR',num2str(Pblock.nominal_srate),'_Input.mat');
    outfname = strcat(casedir,'NR',num2str(Pblock.nominal_srate),'_Output.mat');
    
    save(infname,'Pin');
    Pout = zeros(1,5);
    save(outfname,'Pout','-v7.3');
    outfile = matfile(outfname,'Writable',true);
    
    for j = 1:100
        Pblock.error = randn * Pin.error;
        Pblock.actual_srate = Pblock.nominal_srate * (1 + Pblock.error/100);
        Pblock.duration = Pin.duration; % block duration in secs
        Pblock.warble_freq =  Pin.warble_freq;
        Pblock.warble = Pin.warble; % no longer random
        Pblock.drift = Pin.drift; % no longer random
        Pblock.chunk_len = max(1,round(Pblock.nominal_srate*Pin.chunk_lens(1+mod((j-1),length(Pin.chunk_lens))))); % systematic
        
        y = 0 :(1/Pblock.actual_srate):Pblock.duration;
        
        %%% warble, drift and jitter generation
        warble_drift = Pin.warble * cos(2 * pi * y * Pblock.warble_freq) + Pin.drift * (1 - cos((1:length(y))/length(y)));
        y =  y + warble_drift;
        
        offsets = ones(Pblock.chunk_len,1)*(randn(1,ceil(length(y)/Pblock.chunk_len))*Pin.jitter);
        offsets = bsxfun(@plus,offsets,-(0:Pblock.chunk_len-1)'/Pblock.actual_srate + (0:Pblock.chunk_len-1)'/Pblock.nominal_srate);
        z = y + offsets(1:length(y));
        
        Pout = [Pblock.actual_srate, Pblock.warble_freq, Pblock.warble, Pblock.drift,length(y)];
        [nrows,ncols] = size(outfile,'Pout');
        outfile.Pout(nrows+1,:) = Pout;
        %plot(z);
        %hold on;
        
        % write to file
        fileIDy = fopen([yfname '_' num2str(j) '.bin'],'w');
        fileIDz = fopen([zfname '_' num2str(j) '.bin'],'w');
        fwrite(fileIDy,y,'double');
        fwrite(fileIDz,z,'double');
        fclose(fileIDy);
        fclose(fileIDz);
    end
    
end
fprintf('done.\n');


