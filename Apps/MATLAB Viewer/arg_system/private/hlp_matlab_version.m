function v = hlp_matlab_version()
% Get the MATLAB version in a numeric format that can be compared with <, >, etc.
persistent vers;
try
    v = vers(1);
catch
    v = strsplit(version,'.'); v = str2num(v{1})*100 + str2num(v{2});
    vers = v;
end

% Split a string according to some delimiter(s). Not as fast as hlp_split (and doesn't fuse 
% delimiters), but doesn't need bsxfun().
function strings = strsplit(string, splitter)
ix = strfind(string, splitter);
strings = cell(1,numel(ix)+1);
ix = [0 ix numel(string)+1];
for k = 2 : numel(ix)
    strings{k-1} = string(ix(k-1)+1:ix(k)-1); end
strings = strings(~cellfun('isempty',strings));
