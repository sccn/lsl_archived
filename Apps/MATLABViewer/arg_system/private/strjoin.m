% Concatenates a cell array of strings.
%
% Input arguments:
% adjoiner:
%    string separating each neighboring element
% strings:
%    a cell array of strings to join
%
% See also: strsplit, cell2mat

% Copyright 2008-2009 Levente Hunyadi
function string = strjoin(adjoiner, strings)

validateattributes(adjoiner, {'char'}, {'vector'});
validateattributes(strings, {'cell'}, {'vector'});
assert(iscellstr(strings), ...
    'strjoin:ArgumentTypeMismatch', ...
    'The elements to join should be stored in a cell vector of strings (character arrays).');
if isempty(strings)
    string = '';
    return;
end

% arrange substrings into cell array of strings
concat = cell(1, 2 * numel(strings) - 1);  % must be row vector
j = 1;
concat{j} = strings{1};
for i = 2 : length(strings)
    j = j + 1;
    concat{j} = adjoiner;
    j = j + 1;
    concat{j} = strings{i};
end

% concatenate substrings preserving spaces
string = cell2mat(concat);