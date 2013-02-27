function str = hlp_tostring(v)
% Get an human-readable string representation of a data structure.
% String = hlp_tostring(Data)
%
% The resulting string representations are usually executable, but there are corner cases (e.g.,
% certain anonymous function handles and large data sets), which are not supported. For
% general-purpose serialization, see hlp_serialize/hlp_deserialize.
%
% In:
%   Data : a data structure
%
% Out:
%   String : string form of the data structure
%
% Notes:
%   hlp_tostring has builtin support for displaying expression data structures.
%
% Examples:
%   % get a string representation of a data structure
%   hlp_tostring({'test',[1 2 3], struct('field','value')})
%
% See also:
%   hlp_serialize
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-04-15
%
%                                adapted from serialize.m
%                                (C) 2006 Joger Hansegord (jogerh@ifi.uio.no)

n = 15;
str = serializevalue(v,n);

%
% Main hub for serializing values
%
function val = serializevalue(v, n)
if isnumeric(v) || islogical(v)
    val = serializematrix(v, n);
elseif ischar(v)
    val = serializestring(v, n);
elseif isa(v,'function_handle')
    val = serializefunction(v, n);
elseif is_impure_expression(v)
    val = serializevalue(v.tracking.expression, n);
elseif has_canonical_representation(v)
    val = serializeexpression(v, n);
elseif is_dataset(v)
    val = serializedataset(v, n);
elseif isstruct(v)
    val = serializestruct(v, n);
elseif iscell(v)
    val = serializecell(v, n);
elseif isobject(v)
    val = serializeobject(v, n);
else
    try
        val = serializeobject(v, n);
    catch
        error('Unhandled type %s', class(v));
    end
end

%
% Serialize a string
%
function val = serializestring(v,n)
if any(v == '''')
    val = ['''' strrep(v,'''','''''') ''''];
    try
        if ~isequal(eval(val),v)
            val = ['char(' serializevalue(uint8(v), n) ')']; end
    catch
        val = ['char(' serializevalue(uint8(v), n) ')']; 
    end
else
    val = ['''' v ''''];
end

%
% Serialize a matrix and apply correct class and reshape if required
%
function val = serializematrix(v, n)
if ndims(v) < 3
    if isa(v, 'double')
        if size(v,1) == 1 && length(v) > 3 && isequal(v,v(1):v(2)-v(1):v(end))
            % special case: colon sequence
            if v(2)-v(1) == 1
                val = ['[' num2str(v(1)) ':' num2str(v(end)) ']'];
            else
                val = ['[' num2str(v(1)) ':' num2str(v(2)-v(1)) ':' num2str(v(end)) ']'];
            end
        elseif size(v,2) == 1 && length(v) > 3 && isequal(v',v(1):v(2)-v(1):v(end))
            % special case: colon sequence
            if v(2)-v(1) == 1
                val = ['[' num2str(v(1)) ':' num2str(v(end)) ']'''];
            else
                val = ['[' num2str(v(1)) ':' num2str(v(2)-v(1)) ':' num2str(v(end)) ']'''];
            end        
        else            
            val = mat2str(v, n);
        end
    else
        val = mat2str(v, n, 'class');
    end
else
    if isa(v, 'double')
        val = mat2str(v(:), n);
    else
        val = mat2str(v(:), n, 'class');
    end
    val = sprintf('reshape(%s, %s)', val, mat2str(size(v)));
end

%
% Serialize a cell
%
function val = serializecell(v, n)
if isempty(v)
    val = '{}';
    return
end
cellSep = ', ';
if isvector(v) && size(v,1) > 1
    cellSep = '; ';
end

% Serialize each value in the cell array, and pad the string with a cell
% separator.
vstr = cellfun(@(val) [serializevalue(val, n) cellSep], v, 'UniformOutput', false);
vstr{end} = vstr{end}(1:end-2);

% Concatenate the elements and add a reshape if requied
val = [ '{' vstr{:} '}'];
if ~isvector(v)
    val = ['reshape('  val sprintf(', %s)', mat2str(size(v)))];
end

%
% Serialize an expression
%
function val = serializeexpression(v, n)
if numel(v) > 1
    val = ['['];
    for k = 1:numel(v)
        val = [val serializevalue(v(k), n), ', ']; end 
    val = [val(1:end-2) ']'];
else
    if numel(v.parts) > 0
        val = [char(v.head) '('];
        for fieldNo = 1:numel(v.parts)
            val = [val serializevalue(v.parts{fieldNo}, n), ', ']; end
        val = [val(1:end-2) ')'];
    else
        val = char(v.head);
    end
end

%
% Serialize a data set
%
function val = serializedataset(v, n) %#ok<INUSD>
val = '<EEGLAB data set>';

%
% Serialize a struct by converting the field values using struct2cell
%
function val = serializestruct(v, n)
fieldNames   = fieldnames(v);
fieldValues  = struct2cell(v);
if ndims(fieldValues) > 6
    error('Structures with more than six dimensions are not supported');
end
val = 'struct(';
for fieldNo = 1:numel(fieldNames)
    val = [val serializevalue( fieldNames{fieldNo}, n) ', '];
    val = [val serializevalue( permute(fieldValues(fieldNo, :,:,:,:,:,:), [2:ndims(fieldValues) 1]) , n) ];
    val = [val ', '];
end
if numel(fieldNames)==0
    val = [val ')'];
else
    val = [val(1:end-2) ')'];
end    
if ~isvector(v)
    val = sprintf('reshape(%s, %s)', val, mat2str(size(v)));
end

%
% Serialize an object by converting to struct and add a call to the copy
% contstructor
%
function val = serializeobject(v, n)
val = sprintf('%s(%s)', class(v), serializevalue(struct(v), n));


function val = serializefunction(v, n) %#ok<INUSD>
try
    val = ['@' char(get_function_symbol(v))];
catch
    val = char(v);
end



function result___ = get_function_symbol(expression___)
% internal: some function_handle expressions have a function symbol (an @name expression), and this function obtains it
% note: we are using funny names here to bypass potential name conflicts within the eval() clause further below
if ~isa(expression___,'function_handle')
    error('the expression has no associated function symbol.'); end

string___ = char(expression___);
if string___(1) == '@'
    % we are dealing with a lambda function
    if is_symbolic_lambda(expression___)
        result___ = eval(string___(27:end-21));        
    else
        error('cannot derive a function symbol from a non-symbolic lambda function.'); 
    end
else
    % we are dealing with a regular function handle
    result___ = expression___;
end


function res = is_symbolic_lambda(x)
% internal: a symbolic lambda function is one which generates expressions when invoked with arguments (this is what exp_symbol generates)
res = isa(x,'function_handle') && ~isempty(regexp(char(x),'@\(varargin\)struct\(''head'',\{.*\},''parts'',\{varargin\}\)','once'));
