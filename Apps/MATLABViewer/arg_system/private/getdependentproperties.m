% Publicly accessible dependent properties of an object.
%
% See also: meta.property

% Copyright 2010 Levente Hunyadi
function dependent = getdependentproperties(obj)

dependent = {};
if isstruct(obj)  % structures have no dependent properties
    return;
end

try
    clazz = metaclass(obj);
catch %#ok<CTCH>
    return;  % old-style class (i.e. not defined with the classdef keyword) have no dependent properties
end

k = 0;  % number of dependent properties found
n = numel(clazz.Properties);  % maximum number of properties
dependent = cell(n, 1);
for i = 1 : n
    property = clazz.Properties{i};
    if property.Abstract || property.Hidden || ~strcmp(property.GetAccess, 'public') || ~property.Dependent
        continue;  % skip abstract, hidden, inaccessible and independent properties
    end
    
    k = k + 1;
    dependent{k} = property.Name;
end
dependent(k+1:end) = [];  % drop unused cells
