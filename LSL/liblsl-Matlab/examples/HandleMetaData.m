%% instantiate the library
lib = lsl_loadlib();

% create a new StreamInfo and declare some meta-data (in accordance with XDF format)
info = lsl_streaminfo(lib,'MetaTester','EEG',8,100,'cf_float32','myuid56872');
chns = info.desc().append_child('channels');
for label = {'C3','C4','Cz','FPz','POz','CPz','O1','O2'}
    ch = chns.append_child('channel');
    ch.append_child_value('label',label{1});
    ch.append_child_value('unit','microvolts');
    ch.append_child_value('type','EEG');
end
info.desc().append_child_value('manufacturer','SCCN');
cap = info.desc().append_child('cap');
cap.append_child_value('name','EasyCap');
cap.append_child_value('size','54');
cap.append_child_value('labelscheme','10-20');

% create outlet for the stream
outlet = lsl_outlet(info);


% === the following could run on another computer ===

% resolve the stream and open an inlet
lib = lsl_loadlib();
result = {};
while isempty(result)
    result = lsl_resolve_byprop(lib,'type','EEG'); end
inlet = lsl_inlet(result{1});
% get the full stream info (including custom meta-data) and dissect it
inf = inlet.info();
fprintf('The stream''s XML meta-data is: \n');
fprintf([inf.as_xml() '\n']);
fprintf(['The manufacturer is: ' inf.desc().child_value('manufacturer') '\n']);
fprintf(['The cap circumference is: ' inf.desc().child('cap').child_value('size') '\n']);
fprintf('The channel labels are as follows:\n');
ch = inf.desc().child('channels').child('channel');
for k = 1:inf.channel_count()
    fprintf(['  ' ch.child_value('label') '\n']);
    ch = ch.next_sibling();
end
