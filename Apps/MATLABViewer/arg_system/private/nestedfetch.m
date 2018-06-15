% Fetches the value of the named property of an object or structure.
% This function can deal with nested properties.
%
% Input arguments:
% obj:
%    the handle or value object the value should be assigned to
% name:
%    a property name with dot (.) separating property names at
%    different hierarchy levels
% value:
%    the value to assign to the property at the deepest hierarchy
%    level
%
% Example:
%    obj = struct('surface', struct('nested', 23));
%    value = nestedfetch(obj, 'surface.nested');
%    disp(value);  % prints 23
%
% See also: nestedassign

% Copyright 2010 Levente Hunyadi
function value = nestedfetch(obj, name)
    if ~iscell(name)
        nameparts = strsplit(name, '.');
    else
        nameparts = name;
    end
    value = nestedfetch_recurse(obj, nameparts);
end
        
function value = nestedfetch_recurse(obj, name)
    if numel(name) > 1
        value = nestedfetch_recurse(obj.(name{1}), name(2:end));
    else
        value = obj.(name{1});
    end
end
