function res = hlp_varargin2struct(args, varargin)
% Convert a list of name-value pairs into a struct with values assigned to names.
% struct = hlp_varargin2struct(Varargin, Defaults)
%
% In:
%   Varargin : cell array of name-value pairs and/or structs (with values assigned to names)
%
%   Defaults : optional list of name-value pairs, encoding defaults; multiple alternative names may 
%              be specified in a cell array
%
% Example:
%   function myfunc(x,y,z,varargin)
%   % parse options, and give defaults for some of them: 
%   options = hlp_varargin2struct(varargin, 'somearg',10, 'anotherarg',{1 2 3}); 
%
% Notes:
%   * mandatory args can be expressed by specifying them as ..., 'myparam',mandatory, ... in the defaults
%     an error is raised when any of those is left unspecified
%
%   * the following two parameter lists are equivalent (note that the struct is specified where a name would be expected, 
%     and that it replaces the entire name-value pair):
%     ..., 'xyz',5, 'a',[], 'test','toast', 'xxx',{1}. ...
%     ..., 'xyz',5, struct( 'a',{[]},'test',{'toast'} ), 'xxx',{1}, ...     
%
%   * names with dots are allowed, i.e.: ..., 'abc',5, 'xxx.a',10, 'xxx.yyy',20, ...
%
%   * some parameters may have multiple alternative names, which shall be remapped to the 
%     standard name within opts; alternative names are given together with the defaults,
%     by specifying a cell array of names instead of the name in the defaults, as in the following example:
%     ... ,{'standard_name','alt_name_x','alt_name_y'}, 20, ...
%
% Out: 
%   Result : a struct with fields corresponding to the passed arguments (plus the defaults that were
%            not overridden); if the caller function does not retrieve the struct, the variables are
%            instead copied into the caller's workspace.
%
% Examples:
%   % define a function which takes some of its arguments as name-value pairs
%   function myfunction(myarg1,myarg2,varargin)
%   opts = hlp_varargin2struct(varargin, 'myarg3',10, 'myarg4',1001, 'myarg5','test');
%
%   % as before, but this time allow an alternative name for myarg3
%   function myfunction(myarg1,myarg2,varargin)
%   opts = hlp_varargin2struct(varargin, {'myarg3','legacyargXY'},10, 'myarg4',1001, 'myarg5','test');
%
%   % as before, but this time do not return arguments in a struct, but assign them directly to the
%   % function's workspace
%   function myfunction(myarg1,myarg2,varargin)
%   hlp_varargin2struct(varargin, {'myarg3','legacyargXY'},10, 'myarg4',1001, 'myarg5','test');
%
% See also:
%   hlp_struct2varargin, arg_define
%
%                               Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                               2010-04-05

% a struct was specified as first argument
if isstruct(args)
    args = {args}; end

% --- handle defaults ---
if ~isempty(varargin)
    % splice substructs into the name-value list
    if any(cellfun('isclass',varargin(1:2:end),'struct'))
        varargin = flatten_structs(varargin); end    
    
    defnames = varargin(1:2:end);
    defvalues = varargin(2:2:end);
    
    % make a remapping table for alternative default names...
    for k=find(cellfun('isclass',defnames,'cell'))
        for l=2:length(defnames{k})
                name_for_alternative.(defnames{k}{l}) = defnames{k}{1}; end
        defnames{k} = defnames{k}{1};
    end
    
    % create default struct
    if [defnames{:}]~='.'
        % use only the last assignment for each name
        [s,indices] = sort(defnames(:)); 
        indices( strcmp(s((1:end-1)'),s((2:end)'))) = [];
        % and make the struct
        res = cell2struct(defvalues(indices),defnames(indices),2);
    else
        % some dot-assignments are contained in the defaults
        try
            res = struct();
            for k=1:length(defnames)
                if any(defnames{k}=='.')
                    eval(['res.' defnames{k} ' = defvalues{k};']);
                else
                    res.(defnames{k}) = defvalues{k};
                end
            end
        catch
            error(['invalid field name specified in defaults: ' defnames{k}]);
        end
    end
else
    res = struct();
end

% --- handle overrides ---
if ~isempty(args)
    % splice substructs into the name-value list
    if any(cellfun('isclass',args(1:2:end),'struct'))
        args = flatten_structs(args); end
    
    % rewrite alternative names into their standard form...
    if exist('name_for_alternative','var')
        for k=1:2:length(args)
            if isfield(name_for_alternative,args{k})
                args{k} = name_for_alternative.(args{k}); end
        end
    end
    
    % override defaults with arguments...
    try
        if [args{1:2:end}]~='.'
            for k=1:2:length(args)
                res.(args{k}) = args{k+1}; end
        else
            % some dot-assignments are contained in the overrides
            for k=1:2:length(args)
                if any(args{k}=='.')
                    eval(['res.' args{k} ' = args{k+1};']);
                else
                    res.(args{k}) = args{k+1};
                end
            end
        end
    catch
        if ischar(args{k})
            error(['invalid field name specified in arguments: ' args{k}]);
        else
            error(['invalid field name specified for the argument at position ' num2str(k)]);
        end
    end
end

% check for missing but mandatory args
% note: the used string needs to match mandatory.m
missing_entries = strcmp('__arg_mandatory__',struct2cell(res)); 
if any(missing_entries)
    fn = fieldnames(res)';
    fn = fn(missing_entries);
    error(['The parameters {' sprintf('%s, ',fn{1:end-1}) fn{end} '} were unspecified but are mandatory.']);
end

% copy to the caller's workspace if no output requested
if nargout == 0
    for fn=fieldnames(res)'
        assignin('caller',fn{1},res.(fn{1})); end
end


% substitute any structs in place of a name-value pair into the name-value list
function args = flatten_structs(args)
k = 1;
while k <= length(args)
    if isstruct(args{k})
        tmp = [fieldnames(args{k}) struct2cell(args{k})]';
        args = [args(1:k-1) tmp(:)' args(k+1:end)];
        k = k+numel(tmp);
    else
        k = k+2;
    end
end
