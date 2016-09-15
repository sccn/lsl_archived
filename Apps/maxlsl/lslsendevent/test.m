%% test lslseneevent max patch

%% instantiate the library
disp('Loading library...');
lib = lsl_loadlib();

% make a new stream inlet
disp('Resolving marker streams');

result = {};
while isempty(result)
    result = lsl_resolve_byprop(lib,'type','Markers'); 
    drawnow
end

disp('Opening an inlet on first stream found...');
inlet = lsl_inlet(result{1});


%% send data into the outlet, sample by sample
disp('Now receiving data...');
while true
    % get data from the inlet
    [mrks,ts] = inlet.pull_sample(1);
    % and display it
    if length(ts) > 0,
    fprintf('got %s at time %.5f\n',mrks{1},ts);
    end
    drawnow
end