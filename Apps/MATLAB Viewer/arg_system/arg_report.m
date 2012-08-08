function res = arg_report(type,func,args)
% Report information of a certain Type from the given Function.
% Result = arg_report(Type,Function,Arguments)
%
% Functions that declare their arguments via arg_define() make their parameter specification
% accessible to outside functions. This can be used to display auto-generated settings dialogs, to
% record function calls, and so on.
%
% Varying amounts of meta-data can be obtained in addition to the raw parameter values, including
% just the bare-bones value struct ('vals'), the meta-data associated with the passed arguments,
% and the full set of meta-data for all possible options (for multi-option parameters), even if these
% options were not actually prompted by the Arguments.
%
% In:
%   Type : Type of information to report, can be one of the following:
%          'rich' : Report a rich declaration of the function's arguments as a struct array, with
%                   fields as in arg_specifier.
%          'lean' : Report a lean declaration of the function's arguments as a struct array, with
%                   fields as in arg_specifier, like rich, but excluding the alternatives field.
%          'vals' : Report the values of the function's arguments as a struct, possibly with
%                   sub-structs.
%
%          'properties' : Report properties of the function, if any (these can be declared via
%                         declare_properties)
%
%          'handle': Report function handles to scoped functions within the Function (i.e.,
%                    subfunctions). The named of those functions are listed as a cell string array
%                    in place of Arguments, unless there is exactly one returned function. Then,
%                    this function is returned as-is. This functionality is a nice-to-have feature
%                    for some use cases but not essential to the operation of the argument system.
%
%   Function : a function handle to a function which defines some arguments (via arg_define)
%
%   Arguments : cell array of parameters to be passed to the function; depending on the function's
%               implementation, this can affect the current value assignment (or structure) of the
%               parameters being returned If this is not a cell, it is automatically wrapped inside
%               one (note: to specify the first positional argument as [] to the function, always
%               pass it as {[]}; this is only relevant if the first argument's default is non-[]).
%
% Out:
%   Result : the reported data.
%
% Notes:
%   In all cases except 'properties', the Function must use arg_define() to define its arguments.
%
% Examples:
%   % for a function call with some arguments assigned, obtain a struct with all parameter
%   % names and values, including defaults
%   params = arg_report('vals',@myfunction,{4,10,true,'option1','xxx','option5',10})
%
%   % obtain a specification of all function arguments, with defaults, help text, type, shape, and other
%   % meta-data (with a subset of settings customized according to arguments)
%   spec = arg_report('rich',@myfunction,myarguments)
%
%   % obtain a report of properties of the function (declared via declared_properties() within the
%   % function)
%   props = arg_report('properties',@myfunction)
%
% See also:
%   arg_define, declare_properties
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24

% uniformize arguments
if ~exist('args','var')
    args = {}; end
if isequal(args,[])
    args = {}; end
if ~iscell(args)
    args = {args}; end
if ischar(func)
    func = str2func(func); end

% make sure that the direct mode is disabled for the function being called (because in direct mode
% it doesn't report)
indices = find(cellfun('isclass',args,'struct') | strcmp(args,'arg_direct'));
for k = indices(end:-1:1)
    if ischar(args{k}) && k<length(args)
        % found it in the NVPs
        args{k+1} = false;
        break;
    elseif isfield(args{k},'arg_direct')
        % found it in a struct
        args{k}.arg_direct = false;
        break;
    end
end


if any(strcmpi(type,{'rich','lean','vals','handle'}))
    % issue the report
    res = do_report(type,func,args);
elseif strcmpi(type,'properties')
    % Note: this feature is specific to BCILAB and not used in the basic 
    % distribution of the argument system.
    if isempty(args)
        % without arguments we can do a quick hash map lookup
        % (based on the MD5 hash of the file in question)
        info = functions(func);
        hash = ['h' utl_fileinfo(info.file,char(func))];
        try
            % try lookup
            persistent cached_properties; %#ok<TLEV>
            res = cached_properties.(hash);
        catch
            % fall back to actually reporting it
            res = do_report('properties',func,args);
            % and store it for the next time
            cached_properties.(hash) = res;
        end
    else
        % with arguments we don't try to cache (as the properties might be argument-dependent)
        res = do_report('properties',func,args);
    end
end


function res = do_report(type,func,args)
global tracking;
persistent have_expeval;
if isempty(have_expeval)
    have_expeval = exist('exp_eval','file'); end
try
    % the presence of one of the arg_report_*** functions in the stack communicates to the receiver
    % that a report is requested and what type of report...
    feval(['arg_report_' lower(type)],func,args,have_expeval);
catch report
    if strcmp(report.identifier,'arg_system:report_args')
        % get the ticket of the report
        ticket = sscanf(report.message((find(report.message=='=',1,'last')+1):end),'%f');
        % read out the payload
        res = tracking.arg_sys.reports{ticket};
        % and return the ticket
        tracking.arg_sys.tickets.addLast(ticket);
    else
        % other error:
        if strcmp(type,'properties')
            % almost certainly no properties clause defined
            res = {};
        else
            % genuine error: pass it on
            rethrow(report);
        end
    end
end


% --- a bit of boilerplate below (as the caller's name is relevant here) ---

function arg_report_rich(func,args,have_expeval) %#ok<DEFNU>
if have_expeval && nargout(func) > 0
    exp_eval(func(args{:}));
else
    func(args{:});
end

function arg_report_lean(func,args,have_expeval) %#ok<DEFNU>
if have_expeval && nargout(func) > 0
    exp_eval(func(args{:}));
else
    func(args{:});
end

function arg_report_vals(func,args,have_expeval) %#ok<DEFNU>
if have_expeval && nargout(func) > 0
    exp_eval(func(args{:}));
else
    func(args{:});
end

function arg_report_handle(func,args,have_expeval) %#ok<DEFNU>
if have_expeval && nargout(func) > 0
    exp_eval(func(args{:}));
else
    func(args{:});
end

function arg_report_properties(func,args,have_expeval)  %#ok<DEFNU>
if have_expeval && nargout(func) > 0
    exp_eval(func(args{:}));
else
    func(args{:});
end
