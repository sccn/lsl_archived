function res = arg_nogui(varargin)
% Like arg(), but not displayed by GUIs.
% Spec = arg_nogui(Names,Default,Range,Options...)
%
% This type of functon argument specifier behaves like arg(), except that it will not be displayed 
% in GUIs that are generated for the function. This is mainly used for optional arguments that have
% a format that is too complex to be meaningfully edited by the user (e.g. a large matrix, or a 
% long cell array), or possibly confusing (e.g. some functions have special arguments that are 
% undocumented).
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
%   arg_define(varargin, ...
%       arg('arg1',10,[],'Some argument.'), ...
%       arg_nogui('arg_hidden',1001,[],'Hidden argument. This one will not be displayed in a GUI; reserved for special purposes.'));
%
% See also:
%   arg, arg_norep, arg_sub, arg_subswitch, arg_subtoggle, arg_define
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24

if nargin == 1
    res = {@invoke_arg_internal,[varargin {[],[],[],'displayable',false}]};
elseif nargin >= 4
    res = {@invoke_arg_internal,[varargin {'displayable',false}]};
elseif nargin == 2
    res = {@invoke_arg_internal,[varargin {[],[],'displayable',false}]};
else
    res = {@invoke_arg_internal,[varargin {[],'displayable',false}]};
end