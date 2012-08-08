function res = arg_norep(varargin)
% Like arg(), but not reported to outer functions (inluding GUIs or other frameworks).
% Spec = arg_norep(Names,Default,Range,Help,Options...)
%
% This type of argument specifier behaves like arg(), but is never reported outside the function. 
% Possible uses are arguments that are not directly user-editable (but which are instead supplied
% by some framework code). An example is the image that is passed to an image processing algorithm:
% The options should be visible in dialogs, but the image would be passed only when the function
% is actually being executed. 
%
% A caveat is, however, that other user options that depend on the presence of a particular image
% will behave in undesired ways when a config dialog is shown without passing an appropriate image.
% Thus, norep arguments often require that the framework code that deals with the respective
% functions is aware of the presence and function of these arguments.
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
%   Default : Optionally the default value of the argument; can be any data structure (default: []).
%
%   Range : Optionally a range of admissible values (default: []).
%           * If empty, no range is enforced.
%           * If a cell array, each cell is considered one of the allowed values.
%           * If a 2-element numeric vector, the two values are considered the numeric range of the
%             data (inclusive).
%
%   Help : The help text for this argument, optional. (default: []).
%
%   Options... : Optional name-value pairs to denote additional properties:
%                 'type' : Specify the primitive type of the parameter (default: [], indicating that
%                          it is auto-discovered from the Default and/or Range). The primitive type
%                          is one of the following strings:
%                             'logical', 'char', 'int8', 'uint8', 'int16', 'uint16', 'int32',
%                             'uint32', 'int64', 'uint64', 'denserealsingle', 'denserealdouble',
%                             'densecomplexsingle', 'densecomplexdouble', 'sparserealsingle',
%                             'sparserealdouble', 'sparsecomplexsingle', 'sparsecomplexdouble',
%                             'cellstr', 'object'.
%                          If auto-discovery was requested, but fails for some reason, the default
%                          type is set to 'denserealdouble'.
%
%                 'shape' : Specify the array shape of the parameter (default: [], indicating that
%                           it is auto-discovered from the Default and/or Range). The array shape is
%                           one of the following strings: 'scalar','row','column','matrix','empty'.
%                           If auto-discovery was requested, but fails for some reason, the default
%                           shape is set to 'matrix'.
%
% Out:
%   Spec : A cell array, that, when called as invoke_arg_internal(reptype,spec{1}{:}), yields a 
%          specification of the argument, for use by arg_define. The (internal) structure of that is 
%          as follows:
%          * Generally, this is a cell array (here: one element) of cells formatted as:
%            {Names,Assigner-Function,Default-Value}.
%          * Names is a cell array of admissible names for this argument.
%          * Assigner-Function is a function that returns the rich specifier with value assigned,
%            when called as Assigner-Function(Value).
%          * reptype is either 'rich' or 'lean', where in lean mode, the aternatives field remains
%            empty.
%
% Notes:
%   For MATLAB versions older than 2008a, type and shape checking, as well as auto-discovery, are
%   not necessarily executed.
%
% Examples:
%   function myfunction(varargin)
%   % declare an arguments, one of which is not reportable
%   arg_define(varargin, ...
%       arg_norep('image',[]), ...
%       arg('quality',10,[],'Quality setting. Controls the quality of the processing.'), ...
%       arg('flag',true,[],'Some flag. This is a flag.'));
%
% See also:
%   arg, arg_norep, arg_sub, arg_subswitch, arg_subtoggle, arg_define
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24

if nargin == 1
    res = {@invoke_arg_internal,[varargin {[],[],[],'reportable',false}]};
elseif nargin >= 4
    res = {@invoke_arg_internal,[varargin {'reportable',false}]};
elseif nargin == 2
    res = {@invoke_arg_internal,[varargin {[],[],'reportable',false}]};
else
    res = {@invoke_arg_internal,[varargin {[],'reportable',false}]};
end