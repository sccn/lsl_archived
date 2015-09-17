% Indicator of which elements of a universal set are in a particular set.
%
% Input arguments:
% strset:
%    the particular set as a cell array of strings
% struniversal:
%    the universal set as a cell array of strings, all elements in the
%    particular set are expected to be in the universal set
%
% Output arguments:
% ind:
%    a logical vector of which elements of the universal set are found in
%    the particular set

% Copyright 2010 Levente Hunyadi
function ind = strsetmatch(strset, struniversal)

assert(iscellstr(strset), 'strsetmatch:ArgumentTypeMismatch', ...
    'The particular set is expected to be a cell array of strings.');
assert(iscellstr(struniversal), 'strsetmatch:ArgumentTypeMismatch', ...
    'The particular set is expected to be a cell array of strings.');

ind = false(size(struniversal));
for k = 1 : numel(struniversal)
    ind(k) = ~isempty(strmatch(struniversal{k}, strset, 'exact'));
end