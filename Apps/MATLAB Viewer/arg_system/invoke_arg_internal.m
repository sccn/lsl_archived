function spec = invoke_arg_internal(reptype,varargin) %#ok<INUSL>
% same type of invoke function as in arg_sub, arg_subswitch, etc. - but shared between
% arg, arg_norep, and arg_nogui
spec = hlp_microcache('arg',@invoke_arg,varargin{:});

% the function that does the actual work of building the argument specifier
function spec = invoke_arg(names,default,range,help,varargin)

% start with a base specification
spec = arg_specifier('head',@arg);

% override properties
if exist('names','var')
    spec.names = names; end
if exist('default','var')
    spec.value = default; end
if exist('range','var')
    spec.range = range; end
if exist('help','var')
    spec.help = help; end
for k=1:2:length(varargin)
    if isfield(spec,varargin{k})
        spec.(varargin{k}) = varargin{k+1}; 
    else
        error('arg_system:no_new_fields','It is not allowed to introduce fields into a specifier that are not declared in arg_specifier.');
    end
end

% do fixups & checking
if ~iscell(spec.names)
    spec.names = {spec.names}; end
if isempty(spec.names) || ~iscellstr(spec.names)
    error('The argument must have a name or cell array of names.'); end

% parse the help
if ~isempty(spec.help)
    try
        spec.help = parse_help(spec.help);
    catch e
        disp(['Problem with the help text for argument ' spec.names{1} ': ' e.message]);
        spec.help = {};
    end
elseif spec.reportable && spec.displayable
    disp(['Please specify a description for argument ' spec.names{1} ', or specify it via arg_nogui() instead.']);
end

% do type inference
[spec.type,spec.shape,spec.range,spec.value] = infer_type(spec.type,spec.shape,spec.range,spec.value);


% infer the type & range of the argument, based on provided info (note: somewhat messy)
function [type,shape,range,value] = infer_type(type,shape,range,value)
try
    if isempty(type)
        % try to auto-discover the type (or leave empty, if impossible)
        if ~isempty(value)
            type = PropertyType.AutoDiscoverType(value);
        elseif ~isempty(range)
            if isnumeric(range)
                type = PropertyType.AutoDiscoverType(range);
            elseif iscell(range)
                types = cellfun(@PropertyType.AutoDiscoverType,range,'UniformOutput',false);
                if length(unique(types)) == 1
                    type = types{1}; end
            end
        end
    end
    if isempty(shape)
        % try to auto-discover the shape
        if ~isempty(value)
            shape = PropertyType.AutoDiscoverShape(value);
        elseif ~isempty(range)
            if isnumeric(range)
                shape = 'scalar';
            elseif iscell(range)
                shapes = cellfun(@PropertyType.AutoDiscoverShape,range,'UniformOutput',false);
                if length(unique(shapes)) == 1
                    shape = shapes{1}; end
            end
        end
    end
catch
end

% rule: if in doubt, fall back to denserealdouble and/or matrix
if isempty(type)
    type = 'denserealdouble'; end
if isempty(shape)
    shape = 'matrix'; end

% rule: if both the value and the range are cell-string arrays, the type is 'logical';
%       this means that the value is a subset of the range
if iscellstr(value) && iscellstr(range)
    type = 'logical'; end

% rule: if the value is empty, but the range is a cell-string array and the type is not 'logical',
%       the value is the first range element; here, the value is exactly one out of the possible
%       strings in range (and cannot be empty)
if isempty(value) && iscellstr(range) && ~strcmp(type,'logical')
    value = range{1}; end

% rule: if the value is an empty char array, the shape is by default 'row'
if isequal(value,'') && ischar(value)
    shape = 'row'; end

% rule: if the value is []; convert to the appropriate MATLAB type (e.g., int, etc.)
if isequal(value,[])
    if strcmp(type,'cellstr')
        value = {}; 
    else
        try
            pt = PropertyType(type,shape,range);
            value = pt.ConvertFromMatLab(value);
        catch
        end
    end
end

% rule: if the value is a logical scalar and the type is logical, and the range is a cell-string 
%       array (i.e. a set of strings), the value is mapped to either the entire set or the empty set
%       (i.e. either all elements are in, or none)
if isscalar(value) && islogical(value) && strcmp(type,'logical') && iscell(range)
    if value
        value = range;
    else
        value = {};
    end
end

