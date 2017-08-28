% Splits a string into a cell array of strings.
%
% Input arguments:
% string:
%    a string to split into a cell array
% adjoiner:
%    string separating each neighboring element
%
% See also: strjoin

% Copyright 2008-2009 Levente Hunyadi
function strings = strsplit(string, adjoiner)

if nargin < 2
    adjoiner = sprintf('\n');
end

ix = strfind(string, adjoiner);
strings = cell(numel(ix)+1, 1);
ix = [0 ix numel(string)+1];  % place implicit adjoiners before and after string
for k = 2 : numel(ix)
    strings{k-1} = string(ix(k-1)+1:ix(k)-1);
end