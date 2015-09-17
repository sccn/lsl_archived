% Filter elements of array that meet a condition.

% Copyright 2010 Levente Hunyadi
function array = arrayfilter(fun, array)

validateattributes(fun, {'function_handle'}, {'scalar'});

if isobject(array)
    filter = false(size(array));
    for k = 1 : numel(filter)
        filter(k) = fun(array(k));
    end
else
    filter = arrayfun(fun, array);  % logical indicator array of elements that satisfy condition
end
array = array(filter);          % array of elements that meet condition
