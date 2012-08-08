function res = arg_define(vals,varargin)
% Declare function arguments with optional defaults and built-in GUI support.
% Struct = arg_define(Values, Specification...)
% Struct = arg_define(Format, Values, Specification...)
%
% This is essentially an improved replacement for the parameter declaration line of a function.
% Assigns Values (a cell array of values, typically the "varargin" of the calling function,
% henceforth named the "Function") to fields in the output Struct, with parsing implemented
% according to a Specification of argument names and their order (optionally with a custom argument
% Format description).
%
% By default, values can be a list of a fixed number of positional arguments (i.e., the typical
% MATLAB calling format), optionally followed by a list of name-value pairs (NVPs, e.g., as the
% format accepted by figure()), in which, as well, instead of any given NVP, a struct may be
% passed (thus, one may pass a mix of 'name',value,struct,'name',value,'name',value, ...
% parameters). Alternatively, by default the entire list of positional arguments can instead be be
% specified as a list of NVPs/structs. Only names that are allowed by the Specification may be used,
% if positional syntax is allowed by the Format (which is the default).
%
% The special feature over hlp_varargin2struct()-like functionality is that arguments defined via
% arg_define can be reported to outside functions (if triggered by arg_report()). The resulting
% specification can be rendered in a GUI or be processed otherwise.
%
% In:
%   Format : Optional format description (default: [0 Inf]):
%            * If this is a number (say, k), it indicates that the first k arguments are specified
%              in a positional manner, and the following arguments are specified as list of
%              name-value pairs and/or structs.
%            * If this is a vector of two numbers [0 k], it indicates that the first k arguments MAY
%              be specified in a positional manner (the following arguments must be be specified as
%              NVPs/structs) OR alternatively, all arguments can be specified as NVPs / structs.
%              Only names that are listed in the specification may be used as names (in NVPs and
%              structs) in this case.
%            * If this is a function handle, the function is used to transform the Values prior to
%              any other processing into a new Values cell array. The function may specify a new
%              (numeric) Format as its second output argument (if not specified, this is 0).
%
%   Values : A cell array of values passed to the function (usually the calling function's
%            "varargin"). Interpreted according to the Format and the Specification.
%
%   Specification... : The specification of the calling function's arguments; this is a sequence of
%                      arg(), arg_norep(), arg_nogui(), arg_sub(), arg_subswitch(), arg_subtoggle()
%                      specifiers. The special keywords mandatory and unassigned can be used in the
%                      declaration of default values, where "mandatory" declares that this argument
%                      must be assigned some value via Values (otherwise, an error is raised before
%                      the arg is passed to the Function) and "unassigned" declares that the
%                      variable will not be assigned unless passed as an argument (akin to the
%                      default behavior of regular MATLAB function arguments).
%
% Out:
%   Struct : A struct with values assigned to fields, according to the Specification and Format.
%
%            If this is not captured by the Function in a variable, the contents of Struct are
%            instead assigned to the Function's workspace (default practice) -- but note that this
%            only works for variable names are *not& also names of functions in the path (due to a
%            flaw in MATLAB's treatment of identifiers). Thus, it is good advice to use long/expressive
%            variable names to avoid this situation, or possibly CamelCase names.
%
% See also:
%   arg, arg_nogui, arg_norep, arg_sub, arg_subswitch, arg_subtoggle
%
% Notes:
%   1) If the Struct output argument is omitted by the user, the arguments are not returned as a
%      struct but instead directly copied into the Function's workspace.
%
%   2) Someone may call the user's Function with the request to deliver the parameter specification, 
%      instead of following the normal execution flow. arg_define() automatically handles this task
%      by throwing an exception of the type 'arg_system:report_args' using arg_issuereport(), which 
%      is to be caught by the requesting function.
%
% Performance Tips:
%   1) If a struct with a field named 'arg_direct' is passed (and is set to true), or a name-value
%      pair 'arg_direct',true is passed, then all type checking, specification parsing, fallback to
%      default values and reporting functionality are skipped. This is a fast path to call a function,
%      and it usually requires that all of its arguments are passed. The function arg_report allows
%      to get a struct of all function arguments that can be used subsequently as part of a direct 
%      call.
%
%      Please make sure not to pass multiple occurrences of 'arg_direct' with conflicting values to
%      arg_define, as the behavior will then be undefined.
%
%   2) The function is about 2x as fast (in direct mode) if arguments are returned as a struct instead
%      of being written into the caller's workspace.
%
% Examples:
%   function myfunction(varargin)
%
%   % begin a default argument declaration and declare a few arguments; The arguments can be passed either:
%   % - by position: myfunction(4,20); including the option to leave some values at their defaults, e.g. myfunction(4) or myfunction()
%   % - by name: myfunction('test',4,'blah',20); myfunction('blah',21,'test',4); myfunction('blah',22);
%   % - as a struct: myfunction(struct('test',4,'blah',20))
%   % - as a sequence of either name-value pairs or structs: myfunction('test',4,struct('blah',20)) (note that this is not ambiguous, as the struct would come in a place where only a name could show up otherwise
%   arg_define(varargin, ...
%       arg('test',3,[],'A test.'), ...
%       arg('blah',25,[],'Blah.'));
%
%   % a special syntax that is allowed is passing a particular parameter multiple times - in which case only the last specification is effective
%   % myfunction('test',11, 'blah',21, 'test',3, struct('blah',15,'test',5), 'test',10) --> test will be 10, blah will be 15
%
%   % begin an argument declaration which allows 0 positional arguments (i.e. everything must be passed by name
%   arg_define(0,varargin, ...
%
%   % begin an argument declaration which allows exactly 1 positional arguments, i.e. the first one must be passed by position and the other one by name (or struct) 
%   % valid calls would be: myfunction(3,'blah',25); myfunction(3); myfunction(); (the last one assumes the default for both)
%   arg_define(1,varargin, ...
%       arg('test',3,[],'A test.'), ...
%       arg('blah',25,[],'Blah.'));
%
%   % begin an argument decalration which allows either 2 positional arguments or 0 positional arguments (i.e. either the first two are passed by position, or all are passed by name)
%   % some valid calls are: myfunction(4,20,'flag',true); myfunction(4,20); myfunction(4,20,'xyz','test','flag',true); myfunction(4); myfunction('flag',true,'test',4,'blah',21); myfunction('flag',true)
%   arg_define([0 2],varargin, ...
%       arg('test',3,[],'A test.'), ...
%       arg('blah',25,[],'Blah.'), ...
%       arg('xyz','defaultstr',[],'XYZ.'), ...
%       arg('flag',false,[],'Some flag.'));
%
%   % begin an argument declaration in which the formatting of arguments is completely arbitrary, and a custom function takes care of bringing them into a form understood by 
%   % the arg_define implementation. This function takes a cell array of arguments (in any formatting), and returns a cell array of a standard formatting (e.g. name-value pairs, or structs)
%   arg_define(@myparser,varargin, ...
%       arg('test',3,[],'A test.'), ...
%       arg('blah',25,[],'Blah.'));
%
%   % return the arguments as fields in a struct (here: opts), instead of directly in the workspace
%   opts = arg_define(varargin, ...
%       arg('test',3,[],'A test.'), ...
%       arg('blah',25,[],'Blah.'));
%
%   % note: in the current implementation, the only combinations of allowed argument numbers are: arg_define(...); arg_define(0, ...); arg_define(X, ...); arg_define([0 X], ...); arg_define(@somefunc, ...);
%   %       the implicit default is arg_define([0 Inf], ...)
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24

global tracking;
if isempty(tracking)
    arg_init; end

% --- get Format, Values and Specification ---

if iscell(vals)
    % no Format specifier was given: use default
	fmt = [0 Inf];
	spec = varargin;
    try
        % quick checks for direct (fast) mode
        if isfield(vals{end},'arg_direct')
            direct_mode = vals{end}.arg_direct;
        elseif strcmp(vals{1},'arg_direct')
            direct_mode = vals{2};
        else
            % figure it out later
            direct_mode = false;
        end
        structs = cellfun('isclass',vals,'struct');
    catch
        % vals was empty: default behavior
        direct_mode = false;            
    end
else
    % a Format specifier was given as the first argument (instead of vals as the first argument) ...
    if isempty(vals)
        % ... but it was empty: use default behavior
        fmt = [0 Inf];
    else
        % ... and was nonempty: use it
        fmt = vals;
    end
    % shift the remaining two args
    vals = varargin{1};
    spec = varargin(2:end);
    if isa(fmt,'function_handle')
        % Format was a function: run it
        if nargout(fmt) == 1
            vals = fmt(vals);
            fmt = 0;
        else
            [vals,fmt] = feval(fmt,vals);
        end
    end
    direct_mode = false;
end


% --- if not yet known, determine conclusively if we are in direct mode (specificationless and therefore fast) ---

% this mode is only applicable when all arguments can be passed as NVPs/structs
if ~direct_mode && any(fmt == 0)
    % search for an arg_direct argument
    structs = cellfun('isclass',vals,'struct');
    indices = find(structs | strcmp(vals,'arg_direct'));
    for k = indices(end:-1:1)
        if ischar(vals{k}) && k<length(vals)
            % found it in the NVPs
            direct_mode = vals{k+1};
            break;
        elseif isfield(vals{k},'arg_direct')
            % found it in a struct
            direct_mode = vals{k}.arg_direct;
            break;
        end
    end
end

if direct_mode

    % --- direct mode: quickly collect NVPs from the arguments and produce a result ---
    
    % obtain flat NVP list
    if any(structs(1:2:end))
        vals = flatten_structs(vals); end
    
    if nargout
        % get names & values
        names = vals(1:2:end);
        values = vals(2:2:end);
        
        % use only the last assignment for each name
        [s,indices] = sort(names);
        indices(strcmp(s(1:end-1),s(2:end))) = [];
        
        % build & return a struct
        res = cell2struct(values(indices),names(indices),2);        
    else
        % place the arguments in the caller's workspace
        for k=1:2:length(vals)
            assignin('caller',vals{k},vals{k+1}); end
    end

    % also return the arguments in NVP form
    assignin('caller','arg_nvps',vals);
    
else    
    
    % --- full parsing mode: determine the reporting type ---
    
    % usually, the reporting type is 'none', except if called (possibly indirectly) by
    % arg_report('type', ...): in this case, the reporting type is 'type' reporting is a special way to
    % call arg_define, which requests the argument specification, so that it can be displayed by GUIs,
    % etc.
    %
    % * 'none' normal execution: arg_define returns a Struct of Values to the Function or assigns the
    %          Struct's fields to the Function's workspace
    % * 'rich' arg_define yields a rich specifier list to arg_report(), basically an array of specifier
    %          structs (see arg_specifier for the field names)
    % * 'lean' arg_define yields a lean specifier list to arg_report(), basically an array of specifier
    %          structs but without alternatives for multi-option specifiers
    % * 'vals' arg_define yields a struct of values to arg_report(), wich can subsequently be used as
    %          the full specification of arguments to pass to the Function
    
    try
        throw; %#ok<LTARG>                      % faster than error()
    catch context
        names = {context.stack(3:min(6,end)).name}; % function names at the considered levels of indirection...
        matches = find(strncmp(names,'arg_report_',11)); % ... which start with 'arg_report_'
        if isempty(matches)
            reporting_type = 'none';            % no report requested (default case)
        else
            % the reporting type is the suffix of the deepest arg_report_* function in the call stack
            reporting_type = names{matches(end)}(11+1:end);
        end
    end

    % --- deal with 'handle' and 'properties' reports ---
    
    if strcmp(reporting_type,'handle')        
        % very special report type: 'handle'--> this asks for function handles to nested / scoped
        % functions. unfortunately, these cannot be obtained using standard MATLAB functionality.
        if ~iscellstr(vals)
            error('The arguments passed for handle report must denote function names.'); end
        unresolved = {};
        for f=1:length(vals)
            % resolve each function name in the caller scope
            funcs{f} = evalin('caller',['@' vals{f}]); 
            % check if the function could be retrieved
            tmp = functions(funcs{f});
            if isempty(tmp.file)
                unresolved{f} = vals{f}; end      
        end
        if ~isempty(unresolved)
            % search the remaining ones in the specification
            for f=find(~cellfun('isempty',unresolved))
                funcs{f} = hlp_microcache('findfunction',@find_function_cached,spec,vals{f}); end
        end
        % report it
        if length(funcs) == 1
            funcs = funcs{1}; end
        arg_issuereport(funcs);
    elseif strcmp(reporting_type,'properties')        
        % 'properties' report, but no properties were declared
        arg_issuereport(struct());
    end

    % --- one-time evaluation of the Specification list into a struct array ---

    % evaluate the specification or retrieve it from cache
    [spec,all_names,joint_names,remap] = hlp_microcache('spec',@evaluate_spec,spec,reporting_type,nargout==0);
    
    % --- transform vals to a pure list of name-value pairs (NVPs) ---

    if length(fmt) == 2
        if fmt(1) ~= 0 || fmt(2) <= 0
            % This error is thrown when the first parameter to arg_define() was not a cell array (i.e., not varargin),
            % so that it is taken to denote the optional Format parameter. 
            % Format is usually a numeric array that specifies the number of positional arguments that are 
            % accepted by the function, and if numeric, it can only be either a number or a two-element array
            % that contains 0 and a non-zero number.
            error('For two-element formats, the first entry must be 0 and the second entry must be > 0.'); 
        end

        % there are two possible options: either 0 arguments are positional, or k arguments are
        % positional; assuming at first that 0 arguments are positional, splice substructs into one
        % uniform NVP list (this is because structs are allowed instead of individual NVPs)
        if any(cellfun('isclass',vals(1:2:end),'struct'))
            nvps = flatten_structs(vals);
        else
            nvps = vals;
        end

        % check if all the resulting names are in the set of allowed names (a disambiguation
        % condition in this case)
        if iscellstr(nvps(1:2:end))
            try
                disallowed_nvp = fast_setdiff(nvps(1:2:end),[joint_names {'arg_selection','arg_direct'}]);
            catch
                disallowed_nvp = setdiff(nvps(1:2:end),[joint_names {'arg_selection','arg_direct'}]);
            end
        else
            disallowed_nvp = {'or the sequence of names and values was confused'};
        end

        if isempty(disallowed_nvp)
            % the assumption was correct: 0 arguments are positional
            fmt = 0;
        else
            % the assumption was violated: k arguments are positional, and we enfore strict naming 
            % for the remaining ones (i.e. names must be in the Specification).
            strict_names = true;
            fmt = fmt(2);
        end
    elseif fmt == 0
        % 0 arguments are positional
        nvps = flatten_structs(vals);
    elseif fmt > 0
        % k arguments are positional, the rest are NVPs (no need to enforce strict naming here)
        strict_names = false;
    else
        % This error refers to the optional Format argument.
        error('Negative or NaN formats are not allowed.');
    end

    % (from now on fmt holds the determined # of positional arguments)
    
    if fmt > 0
        % the first k arguments are positional

        % Find out if we are being called by another arg_define; in this case, this definition
        % appears inside an arg_sub/arg_*, and the values passed to the arg_define are part of the
        % defaults declaration of one of these. If these defaults are specified positionally, the
        % first k arg_norep() arguments in Specification are implicitly skipped.
        if ~strcmp(reporting_type,'none') && any(strcmp('arg_define',{context.stack(2:end).name}));
            % we implicitly skip the leading non-reportable arguments in the case of positional
            % assignment (assuming that these are supplied by the outer function), by shifting the
            % name/value assignment by the appropriate number of places
            shift_positionals = min(fmt,find([spec.reportable],1)-1);
        else
            shift_positionals = 0;
        end

        % get the effective number of positional arguments
        fmt = min(fmt,length(vals)+shift_positionals);

        % the NVPs begin only after the k'th argument (defined by the Format)
        nvps = vals(fmt+1-shift_positionals:end);

        % splice in any structs
        if any(cellfun('isclass',nvps(1:2:end),'struct'))
            nvps = flatten_structs(nvps); end

        % do minimal error checking...
        if ~iscellstr(nvps(1:2:end))
            % If you are getting this error, the order of names and values passed as name-value pairs
            % to the function in question was likely mixed up. The error mentions structs because it 
            % is also allowed to pass in a struct in place of any 'name',value pair.
            error('Some of the specified arguments that should be names or structs, are not.'); 
        end

        if strict_names
            % enforce strict names
            try
                disallowed_pos = fast_setdiff(nvps(1:2:end),[joint_names {'arg_selection','arg_direct'}]);
            catch
                disallowed_pos = setdiff(nvps(1:2:end),[joint_names {'arg_selection','arg_direct'}]);
            end
            if ~isempty(disallowed_pos)
                % If you are getting this error, it is most likely due to a mis-typed argument name
                % in the list of name-value pairs passed to the function in question. 
                %
                % Because some functions may support also positional arguments, it is also possible
                % that something that was supposed to be the value for one of the positional
                % arguments was interpreted as part of the name-value pairs lit that may follow the
                % positional arguments of the function. This error is likely because the wrong number
                % of positional arguments was passed (a safer alternative is to instead pass everything 
                % by name).
                error(['Some of the specified arguments do not appear in the argument specification; ' format_cellstr(disallowed_pos) '.']); 
            end
        end

        try
            % remap the positionals (everything up to the k'th argument) into an NVP list, using the
            % code names
            poss = [cellfun(@(x)x{1},all_names(shift_positionals+1:fmt),'UniformOutput',false); vals(1:fmt-shift_positionals)];
        catch
            if strict_names
                % maybe the user intended to pass 0 positionals, but used some disallowed names
                error(['Apparently, some of the used argument names are not known to the function: ' format_cellstr(disallowed_nvp) '.']);
            else
                error(['The first ' fmt ' arguments must be passed by position, and the remaining ones must be passed by name (either in name-value pairs or structs).']);
            end
        end
        % ... and concatenate them with the remaining NVPs into one big NVP list
        nvps = [poss(:)' nvps];
    end


    % --- assign values to names using the assigner functions of the spec ---

    for k=1:2:length(nvps)
        if isfield(remap,nvps{k})
            idx = remap.(nvps{k});
            spec(idx) = spec(idx).assigner(spec(idx),nvps{k+1});
        else
            % append it to the spec (note: this might need some optimization... it would be better 
            % if the spec automatically contained the arg_selection field)
            tmp = arg_nogui(nvps{k},nvps{k+1});
            spec(end+1) = tmp{1}([],tmp{2}{:});
        end
    end
    
    
    % --- if requested, yield a 'vals', 'lean' or 'rich' report  ---

    if ~strcmp(reporting_type,'none')        
        % but deliver only the reportable arguments, and only if the values are not unassigned
        tmp = spec([spec.reportable] & ~strcmp(unassigned,{spec.value}));
        if strcmp(reporting_type,'vals')
            tmp = arg_tovals(tmp); end
        arg_issuereport(tmp);
    end

    
    % --- otherwise post-process the outputs and create a result struct to pass to the Function ---

    % generate errors for mandatory arguments that were not assigned
    missing_entries = strcmp('__arg_mandatory__',{spec.value});
    if any(missing_entries)
        missing_names = cellfun(@(x)x{1},{spec(missing_entries).names},'UniformOutput',false);
        error(['The arguments ' format_cellstr(missing_names) ' were unspecified but are mandatory.']);
    end

    % strip non-returned arguments, and convert it all to a struct of values
    res = arg_tovals(spec);

    % also emit a final NVPs list
    tmp = [fieldnames(res) struct2cell(res)]';
    try
        assignin('caller','arg_nvps',tmp(:)');
    catch
        % this operation might be disallowed under some circumstances
    end
    
    % if requested, place the arguments in the caller's workspace
    if nargout==0
        try
            for fn=fieldnames(res)'
                assignin('caller',fn{1},res.(fn{1})); end
        catch e
            if strcmp(e.identifier,'MATLAB:err_static_workspace_violation')
                error('In a function with nested functions you need to capture the outputs of arg_define into a struct.'); 
            else
                rethrow(e);
            end
        end
    end
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



% evaluate a specification into a struct array
function [spec,all_names,joint_names,remap] = evaluate_spec(spec,reporting_type,require_namecheck)
if strcmp(reporting_type,'rich')
    subreport_type = 'rich';
else
    subreport_type = 'lean';
end

% evaluate the functions to get (possibly arrays of) specifier structs
for k=1:length(spec)
    spec{k} = spec{k}{1}(subreport_type,spec{k}{2}{:}); end

% concatenate the structs to one big struct array
spec = [spec{:}];

% make sure that spec has the correct fields, even if empty
if isempty(spec)
    spec = arg_specifier; 
    spec = spec([]);
end

% obtain the argument names and the joined names
all_names = {spec.names};
joint_names = [all_names{:}];

% create a name/index remapping table
remap = struct();
for n=1:length(all_names)
    for k=1:length(all_names{n})
        remap.(all_names{n}{k}) = n; end
end

% check for duplicate argument names in the Specification
sorted_names = sort(joint_names);
duplicates = joint_names(strcmp(sorted_names(1:end-1),sorted_names(2:end)));  
if ~isempty(duplicates)
    error(['The names ' format_cellstr(duplicates) ' refer to multiple arguments.']); end

% if required, check for name clashes with functions on the path
% (this is due to a glitch in MATLAB's handling of variables that were assigned to a function's scope
% from the outside, which are prone to clashes with functions on the path...)
if require_namecheck && strcmp(reporting_type,'none')
    try
        check_names(cellfun(@(x)x{1},all_names,'UniformOutput',false));
    catch e
        disp_once('The function check_names failed; reason: %s',e.message);
    end
end



% check for name clashes (once)
function check_names(code_names)
persistent name_table;
if ~isstruct(name_table)
    name_table = struct(); end
for name_cell = fast_setdiff(code_names,fieldnames(name_table))
    current_name = name_cell{1};
    existing_func = which(current_name);
    if ~isempty(existing_func)
        if ~exist('function_caller','var')
            function_caller = hlp_getcaller(4); 
            if function_caller(1) == '@'
                function_caller = hlp_getcaller(14); end
        end
        if isempty(strfind(existing_func,'Java method'))
            [path_part,file_part,ext_part] = fileparts(existing_func);
            if ~any(strncmp('@',hlp_split(path_part,filesep),1))
                % If this happens, it means that there is a function in one of the directories in
                % MATLAB's path which has the same name as an argument of the specification. If this 
                % argument variable is copied into the function's workspace by arg_define, most MATLAB
                % versions will (incorrectly) try to call that function instead of accessing the 
                % variable. I hope that they handle this issue at some point. One workaround is to use
                % a longer argument name (that is less likely to clash) and, if it should still be
                % usable for parameter passing, to retain the old name as a secondary or ternary
                % argument name (using a cell array of names in arg()). The only really good
                % solution at this point is to generally assign the output of arg_define to a
                % struct.
                disp([function_caller ': The argument name "' current_name '" clashes with the function "' [file_part ext_part] '" in directory "' path_part '"; it is strongly recommended that you either rename the function or remove it from the path.']); 
            end
        else
            % these Java methods are probably spurious "false positives" of the which() function
        	disp([function_caller ': There is a Java method named "' current_name '" on your path; if you experience any name clash with it, please report this issue.']);
        end
    end
    name_table.(current_name) = existing_func;
end



% recursively find a function handle by name in a specification
% the first occurrence of a handle to a function with the given name is returned
function r = find_function(spec,name)
r = [];
for k=1:length(spec)
    if isa(spec(k).value,'function_handle') && strcmp(char(spec(k).value),name)
        r = spec(k).value; 
        return;
    elseif ~isempty(spec(k).alternatives)
        for n = 1:length(spec(k).alternatives)
            r = find_function(spec(k).alternatives{n},name);
            if ~isempty(r)
                return; end
        end
    elseif ~isempty(spec(k).children)
        r = find_function(spec(k).children,name);
        if ~isempty(r)
            return; end
    end
end



% find a function handle by name in a specification
function f = find_function_cached(spec,name)
% evaluate the functions to get (possibly arrays of) specifier structs
for k=1:length(spec)
	spec{k} = spec{k}{1}('rich',spec{k}{2}{:}); end
% concatenate the structs to one big struct array
spec = [spec{:}];
% now search the function in it
f = find_function(spec,name);



% format a non-empty cell-string array into a string
function x = format_cellstr(x)
x = ['{' sprintf('%s, ',x{1:end-1}) x{end} '}'];
