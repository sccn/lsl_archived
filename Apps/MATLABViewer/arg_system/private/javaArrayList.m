% Converts a MatLab array into a java.util.ArrayList.
%
% Input arguments:
% array:
%    a MatLab row or column vector (with elements of any type)
%
% Output arguments:
% list:
%    a java.util.ArrayList instance
%
% See also: javaArray

% Copyright 2010 Levente Hunyadi
function list = javaArrayList(array)
list = java.util.ArrayList;
if ~isempty(array)
    assert(isvector(array), 'javaArrayList:DimensionMismatch', ...
        'Row or column vector expected.');
    if iscell(array)  % convert cell array into ArrayList
        for k = 1 : numel(array)
            list.add(array{k});
        end
    else  % convert (numeric) array into ArrayList
        for k = 1 : numel(array)
            list.add(array(k));
        end
    end
end
