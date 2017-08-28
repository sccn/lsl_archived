% Sets public properties of a MatLab object using a name-value list.
% Properties are traversed in the order they occur in the class definition.

% Copyright 2008-2009 Levente Hunyadi
function obj = constructor(obj, varargin)

assert(isobject(obj), ...
    'Function operates on MatLab new-style objects only.');

if nargin <= 1
    return;
end

if isa(obj, 'hgsetget')
    set(obj, varargin{:});
    return;
end
    
assert(is_name_value_list(varargin), ...
    'constructor:ArgumentTypeMismatch', ...
    'A list of property name--value pairs is expected.');

% instantiate input parser object
parser = inputParser;

% query class properties using meta-class facility
metaobj = metaclass(obj);
properties = metaobj.Properties;
for i = 1 : numel(properties)
    property = properties{i};
    if is_public_property(property)
        parser.addParamValue(property.Name, obj.(property.Name));
    end
end

% set property values according to name-value list
parser.parse(varargin{:});
for i = 1 : numel(properties)
    property = properties{i};
    if is_public_property(property) && ~is_string_in_vector(property.Name, parser.UsingDefaults)  % do not set defaults
        obj.(property.Name) = parser.Results.(property.Name);
    end
end

function tf = is_name_value_list(list)
% True if the specified list is a name-value list.
%
% Input arguments:
% list:
%    a name-value list as a cell array.

validateattributes(list, {'cell'}, {'vector'});

n = numel(list);
if mod(n, 2) ~= 0
    % a name-value list has an even number of elements
    tf = false;
else
    for i = 1 : 2 : n
        if ~ischar(list{i})
            % each odd element in a name-value list must be a char array
            tf = false;
            return;
        end
    end
    tf = true;
end

function tf = is_string_in_vector(str, vector)

tf = any(strcmp(str, vector));

function tf = is_public_property(property)
% True if the property designates a public, accessible property.

tf = ~property.Abstract && ~property.Hidden && strcmp(property.GetAccess, 'public') && strcmp(property.SetAccess, 'public');
