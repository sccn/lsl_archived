% Field value of each object in an array or cell array.
%
% See also: getfield

% Copyright 2010 Levente Hunyadi
function values = getclassfield(objects, field)

values = cell(size(objects));
if iscell(objects)
    for k = 1 : numel(values)
        values{k} = objects{k}.(field);
    end
else
    for k = 1 : numel(values)
        values{k} = objects(k).(field);
    end
end
