% instantiate the library
disp('Loading library...');
lib = lsl_loadlib();

% make a new stream outlet
% the name (here MyMarkerStream) is visible to the experimenter and should be chosen so that 
% it is clearly recognizable as your MATLAB software's marker stream
% The content-type should be Markers by convention, and the next three arguments indicate the 
% data format (1 channel, irregular rate, string-formatted).
% The so-called source id is an optional string that allows for uniquely identifying your 
% marker stream across re-starts (or crashes) of your script (i.e., after a crash of your script 
% other programs could continue to record from the stream with only a minor interruption).
disp('Creating a new marker stream info...');
info = lsl_streaminfo(lib,'MyMarkerStream','Markers',1,0,'cf_string','myuniquesourceid23443');

disp('Opening an outlet...');
outlet = lsl_outlet(info);

% send markers into the outlet
disp('Now transmitting data...');
markers = {'Test', 'Blah', 'Marker', 'XXX', 'Testtest', 'Test-1-2-3'};
while true
    pause(rand()*3);
    mrk = markers{min(length(markers), 1+floor(rand()*(length(markers))))};
    disp(['now sending ' mrk]);
    outlet.push_sample({mrk});   % note that the string is wrapped into a cell-array
end
