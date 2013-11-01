function res = arg_subswitch(varargin)
% Specify a function argument that can be one of several alternative structs.
% Spec = arg_subswitch(Names,Defaults,Alternatives,Help,Options...)
%
% The correct struct is chosen according to a selection rule (the mapper). Accessible to the
% function as a struct, and visible in the GUI as an expandable sub-list of arguments (with a
% drop-down list of alternative options). The chosen option (usually one out of a set of strings) is
% delivered to the Function as the special struct field 'arg_selection'.
%
% In:
%   Names : The name(s) of the argument. At least one must be specified, and if multiple are
%           specified, they must be passed in a cell array.
%           * The first name specified is the argument's "code" name, as it should appear in the
%             function's code (= the name under which arg_define() returns it to the function).
%           * The second name, if specified, is the "Human-readable" name, which is exposed in the
%             GUIs (if omitted, the code name is displayed).
%           * Further specified names are alternative names for the argument (e.g., for backwards
%             compatibility with older function syntaxes/parameter names).
%
%   Defaults : A cell array of arguments to override defaults for the Source (sources declared as
%              part of Alternatives); all syntax accepted by the (selected) Source is allowed here,
%              whereas in the case of positional arguments, the leading arg_norep() arguments of the
%              source are implicitly skipped. Note: Which one out of the several alternatives should
%              be selected is determined via the 'mapper' (which can be overridden in form of an
%              optional parameter). By default, the mapper maps the first argument to the Selector,
%              and assigns the rest to the matching Source.
%
%   Alternatives : Definition of the switchable option groups. This is a cell array of the form:
%                  {{'selector', Source}, {'selector', Source}, {'selector', Source}, ...} Each
%                  Source is either a function handle (referring to a function that exposes
%                  arguments via an arg_define() clause), or an in-line cell array of argument
%                  specifications, analogously to the more detailed explanation in arg_sub(). In the
%                  latter case (Source is a cell array), the option group may also be a 3-element
%                  cell array of the form {'selector',Source,Format} ... where Format is a format
%                  specifier as explained in arg_define().
%
%   Help : The help text for this argument (displayed inside GUIs), optional. (default: []).
%          (Developers: Please do *not* omit this, as it is the key bridge between ease of use and
%          advanced functionality.)
%
%          The first sentence should be the executive summary (max. 60 chars), any further sentences
%          are a detailed explanation (examples, units, considerations). The end of the first
%          sentence is indicated by a '. ' followed by a capital letter (beginning of the next
%          sentence). If ambiguous, the help can also be specified as a cell array of 2 cells.
%
%   Options... : Optional name-value pairs to denote additional properties:
%                 'cat' :  The human-readable category of this argument, helpful to present a list
%                          of many parameters in a categorized list, and to separate "Core
%                          Parameters" from "Miscellaneous" arguments. Developers: When choosing
%                          names, every bit of consistency with other function in the toolbox helps
%                          the uses find their way (default: []).
%
%                 'mapper' : A function that maps the value (cell array of arguments like Defaults)
%                            to a value in the domain of selectors (first output), and a potentially
%                            updated argument list (second output). The mapper is applied to the
%                            argument list prior to any parsing (i.e. it faces the raw argument
%                            list) to determine the current selection, and its second output (the
%                            potentially updated argument list) is forwarded to the Source that was
%                            selected, for further parsing.
%                           
%                            The default mapper takes the first argument in the argument list as the
%                            Selector and passes the remaining list entries to the Source. If there
%                            is only a single argument that is a struct with a field
%                            'arg_selection', this field's value is taken as the Selector, and the
%                            struct is passed as-is to the Source.
%
%                 'merge': Whether a value (cell array of arguments) assigned to this argument
%                          should completely replace all arguments of the default, or whether it
%                          should instead the two cell arrays should be concatenated ('merged'), so
%                          that defaults are only selectively overridden. Note that for
%                          concatenation to make sense, the cell array of Defaults cannot be some
%                          subset of all allowed positional arguments, but must instead either be
%                          the full set of positional arguments (and possibly some NVPs) or be
%                          specified as NVPs in the first place.
%
% Out:
%   Spec : A cell array, that, when called as spec{1}(reptype,spec{2}{:}), yields a specification of
%          the argument, for use by arg_define. Technical note: Upon assignment with a value (via
%          the assigner field), the 'children' field of the specifier struct is populated according
%          to how the selected (by the mapper) Source (from Alternatives) parses the value into
%          arguments. The additional struct field 'arg_selection 'is introduced at this point.
%
% Examples:
%   % define a function with a multiple-choice argument, with different sub-arguments for each choice
%   % (where the default is 'kmeans'; some valid calls are:
%   %  myfunction('method','em','flagXY',true)
%   %  myfunction('flagXY',true, 'method',{'em', 'myarg',1001})
%   %  myfunction({'vb', 'myarg1',1001, 'myarg2','test'},false)
%   %  myfunction({'kmeans', struct('arg2','test')})
%   function myfunction(varargin)
%   arg_define(varargin, ...
%       arg_subswitch('method','kmeans',{ ...
%            {'kmeans', {arg('arg1',10,[],'argument for kmeans.'), arg('arg2','test',[],'another argument for it.')}, ...
%            {'em', {arg('myarg',1000,[],'argument for the EM method.')}, ...
%            {'vb', {arg('myarg1',test',[],'argument for the VB method.'), arg('myarg2','xyz',[],'another argument for VB.')} ...
%           }, 'Method to use. Three methods are supported: k-means, EM and VB, and each method has optional parameters that can be specified if chosen.'), ...
%       arg('flagXY',false,[],'And some flag.'));
%
%   % define a function with a multiple-choice argument, where the arguments for the choices come 
%   % from a different function each
%   function myfunction(varargin)
%   arg_define(varargin, ...
%       arg_subswitch('method','kmeans',{{'kmeans', @kmeans},{'em', @expectation_maximization},{'vb',@variational_bayes}}, 'Method to use. Each has optional parameters that can be specified if chosen.'), ...
%       arg('flagXY',false,[],'And some flag.'));
%
%   % as before, but specify a different default and override some of the arguments for that default
%   function myfunction(varargin)
%   arg_define(varargin, ...
%       arg_subswitch('method',{'vb','myarg1','toast'},{{'kmeans', @kmeans},{'em', @expectation_maximization},{'vb',@variational_bayes}}, 'Method to use. Each has optional parameters that can be specified if chosen.'), ...
%       arg('flagXY',false,[],'And some flag.'));
%   
%   % specify a custom function to determine the format of the argument (and in particular the 
%   % mapping of assigned value to chosen selection
%       arg_subswitch('method','kmeans',{{'kmeans', @kmeans},{'em',@expectation_maximization},{'vb',@variational_bayes}}, ...
%           'Method to use. Each has optional parameters that can be specified if chosen.', 'mapper',@mymapper), ...
%
% See also:
%   arg, arg_nogui, arg_norep, arg_sub, arg_subtoggle, arg_define
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24


% we return a function that an be invoked to yield a specification (its output is cached for
% efficiency) packed in a cell array together with the remaining arguments
res = {@invoke_argsubswitch_cached,varargin};


function spec = invoke_argsubswitch_cached(varargin)
spec = hlp_microcache('arg',@invoke_argsubswitch,varargin{:});


% the function that does the actual work of building the argument specifier
function spec = invoke_argsubswitch(reptype,names,defaults,alternatives,help,varargin)

% start with a base specification
spec = arg_specifier('head',@arg_subswitch, 'type','char', 'shape','row', 'mapper',@map_argsubswitch);

% override properties
if exist('names','var')
    spec.names = names; end
if exist('help','var')
    spec.help = help; end
for k=1:2:length(varargin)
    if isfield(spec,varargin{k})
        spec.(varargin{k}) = varargin{k+1}; 
    else
        error(['arg_system:no_new_fields','It is not allowed to introduce fields (here: ' varargin{k} ') into a specifier that are not declared in arg_specifier.']);
    end
end

% do checking
if ~iscell(spec.names)
    spec.names = {spec.names}; end
if isempty(spec.names) || ~iscellstr(spec.names)
    error('The argument must have a name or cell array of names.'); end
if isempty(alternatives) 
    error('arg_system:no_options','The Alternatives argument for arg_subswitch() may not be omitted.'); end %#ok<*NODEF>
if nargin(spec.mapper) == 1
    spec.mapper = @(x,y,z) spec.mapper(x); end
% parse the help
if ~isempty(spec.help)
    try
        spec.help = parse_help(spec.help,100);
    catch e
        disp(['Problem with the help text for argument ' spec.names{1} ': ' e.message]);
        spec.help = {};
    end
elseif spec.reportable && spec.displayable
    disp(['Please specify a description for argument ' spec.names{1} ', or specify it via arg_nogui() instead.']);
end

% uniformize Alternatives syntax into {{'selector1',@function1, ...}, {'selector2',@function2, ...}, ...}
if iscellstr(alternatives(1:2:end)) && all(cellfun(@(x)iscell(x)||isa(x,'function_handle'),alternatives(2:2:end)))
    alternatives = mat2cell(alternatives,1,repmat(2,length(alternatives)/2,1)); end

% derive range
spec.range = cellfun(@(c)c{1},alternatives,'UniformOutput',false);

% turn Alternatives into a cell array of Source functions
for k=1:length(alternatives)
    sel = alternatives{k};
    selector = sel{1};
    source = sel{2};    
    if ~ischar(selector)
        error('In arg_subswitch, each selector must be a string.'); end
    if length(sel) > 2
        fmt = sel{3};
    else
        fmt = [];
    end
    % uniformize Source syntax...    
    if iscell(source)
        % args is a cell array instead of a function: we effectively turn this into a regular
        % arg_define-using function (taking & parsing values)
        source = @(varargin) arg_define(fmt,varargin,source{:});
    else
        % args is a function: was a custom format specified?
        if isa(fmt,'function_handle')
            source = @(varargin) source(fmt(varargin));
        elseif ~isempty(fmt)
            error('The only allowed form in which the Format of a Source that is a function may be overridden is as a pre-parser (given as a function handle)'); end
    end
    alternatives{k} = source;
end
sources = alternatives;

% wrap the defaults into a cell if necessary (note: this is convenience syntax)
if ~iscell(defaults)
    if ~(isstruct(defaults) || ischar(defaults))
        error(['It is not allowed to use anything other than a cell, a struct, or a (selector) string as default for an arg_subswitch argument (here:' spec.names{1} ')']); end
    defaults = {defaults};
end
% find out what index and value set the default configuration maps to; this is relevant for the
% merging option: in this case, we need to pull up the correct default and merge it with the passed
% value
[default_sel,default_val] = spec.mapper(defaults,spec.range,spec.names);
default_idx = find(strcmp(default_sel,spec.range));

% create the regular assigner...
spec.assigner = @(spec,value) assign_argsubswitch(spec,value,reptype,sources,default_idx,default_val);

% and assign the default itself
if strcmp(reptype,'rich')
    spec = assign_argsubswitch(spec,defaults,'build',sources,0,{});
else
    spec = assign_argsubswitch(spec,defaults,'lean',sources,0,{});
end



function spec = assign_argsubswitch(spec,value,reptype,sources,default_idx,default_val)
% wrap the value into a cell if necessary (note: this is convenience syntax)
if ~iscell(value)
    if ~(isstruct(value) || ischar(value))
        error(['It is not allowed to assign anything other than a cell, a struct, or a (selector) string to an arg_subswitch argument (here:' spec.names{1} ')']); end
    value = {value};
end
% run the mapper to get the selection according to the value (selectors is here for error checking);
% also update the value
[selection,value] = spec.mapper(value,spec.range,spec.names);
% find the appropriate index in the selections...
idx = find(strcmp(selection,spec.range));
% if we should build the set of alternatives, do so now....
if strcmp(reptype,'build')
    for n=setdiff(1:length(sources),idx)
        arg_sel = arg_nogui('arg_selection',spec.range{n});
        spec.alternatives{n} = [arg_report('rich',sources{n}) arg_sel{1}([],arg_sel{2}{:})]; 
    end
    reptype = 'rich';
end
% build children and override the appropriate item in the aternatives
arg_sel = arg_nogui('arg_selection',spec.range{idx});
if spec.merge && idx == default_idx
    spec.children = [arg_report(reptype,sources{idx},[default_val value]) arg_sel{1}([],arg_sel{2}{:})];
else
    spec.children = [arg_report(reptype,sources{idx},value) arg_sel{1}([],arg_sel{2}{:})];
end
spec.alternatives{idx} = spec.children;
% and set the value of the selector field itself to the current selection
spec.value = selection;




function [selection,args] = map_argsubswitch(args,selectors,names)
if isempty(args)
    selection = selectors{1};
elseif isfield(args{1},'arg_selection')
    selection = args{1}.arg_selection;
else
    [selection,args] = deal(args{1},args(2:end));
end
% Note: If this error is triggered, a value was passed for an argument which has a flexible
% structure (chosen out of a set of possibilities), but the possibility which was chosen according
% to the passed value does not match any of the specified ones. For a value that is a cell array of
% arguments, the choice is (by default) made based on the first element in the cell. For a value
% that is a structure of arguments, the choice is made based on the 'arg_selection' field. The error
% is usually resolved by reviewing the argument specification of the offending function carefully,
% and comparing the passed value to the Alternatives declared in the arg_subswitch() clause in which
% the offending argument is declared.
if ~any(strcmpi(selection,selectors))
    error(['The chosen selector argument (' selection ') does not match any of the possible options (' sprintf('%s, ',selectors{1:end-1}) selectors{end} ') in the function argument ' names{1} '.']); end
