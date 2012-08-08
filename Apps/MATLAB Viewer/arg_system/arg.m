function res = arg(varargin)
% A (rich) specification of a function argument, for use in arg_define() clauses.
% Spec = arg(Names,Default,Range,Help,Options...)
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
%              Special values:
%              * unassigned: the argument is not listed in the function's workspace nor GUI unless
%                explicitly assigned
%              * mandatory: if no value is specified to this argument by the time the function is
%                called, an error is raised
%             
%             Note: If neither Default nor Range are specified, consider specifying the argument's
%             type via the Options... list.
%
%   Range : Optionally a range of admissible values (default: []).
%           * If empty, no range is enforced.
%           * If a cell array, each cell is considered one of the allowed values.
%           * If a 2-element numeric vector, the two values are considered the numeric range of the
%             data (inclusive).
%           * Note: if neither Default nor Range are specified, consider specifying the argument's
%             type via the Options... list.
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
%                 'cat' : The human-readable category of this argument, helpful to present a list of
%                         many parameters in a categorized list, and to separate "Core Parameters"
%                         from "Miscellaneous" arguments. Developers: When choosing names, every bit
%                         of consistency with other function in the toolbox helps the uses find
%                         their way (default: []).
%
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
% Examples:
%   arg_define(varargin, ...
%
%       % define a scalar numeric argument with the name myparam, and the alternative GUI name 
%       % MyParameter, a default value of 5, and some help text (consisting of an executive summary
%       % followed by a longer explanation).
%       arg({'myparam','MyParameter'},5,[],'A parameter. This is the extended help text for the parameter, as it would be displayed, e.g., in a tooltip.')
%
%       % define a string argument with the default value of 'test'
%       arg({'myparam','MyParameter'},'test',[],'Some parameter. If not otherwise specified, the type is derived from the type of the default value')
%
%       % define a scalar numeric argument which has a particular valid range (0 <= x <= 1), and a default of 0.3
%       arg({'myparam','MyParameter'},0.3,[0 1],'A parameter. Note that the set of functions that can be used to specify ranges is currently fairly limited.')
%
%       % define a numeric argument that is a vector (and has a default of [0.1 0.2 0.3])
%       arg({'myparam','MyParameter'},[0.1 0.2 0.3],[],'A parameter. The allowed shape (scalar/matrix/vector), too, is derived from the default value.')
%
%       % define a numeric argument that is a vector (and has a default of [])
%       arg({'myvec','MyVector'},[],[],'A parameter. But the shape can be overridden by passing an option.', 'shape','row')
%
%       % define a string argument that has an empty default
%       arg({'mystrng','MyString'},'',[],'A parameter. Note that the shape is automatically assumed to be row. This is one of several convenience rules supported by the arg function.')
%
%       % define a boolean argument that is by default true
%       arg({'mybool','MyBoolean'},true,[],'Flag XY. Note that generally the executive summary should be quite short - it should be displayable in a GUI dialog.')
%
%       % define a string argument that may only have one out of a set of values
%       arg({'myoption','MyOption'},'blah',{'test','blah','xyz'},'A parameter.')
%
%       % define an argument that denotes a subset of values out of some other set (of strings)
%       arg({'myset','MySetParameter'},{'brain','torso'},{'brain','torso','limbs','head','face'},'A parameter. The type that is inferred here from the default value and the range is logical.')
%
%       % as before, but this time the default set is empty
%       arg({'myset','MySetParameter'},{},{'brain','torso','limbs','head','face'},'A parameter. The type that is inferred here from the default value and the range is logical.')
%
%       % define an argument that has no default assigned, and will not show up as a variable if not explicitly assigned a value
%       arg({'myparam','MyParameter'},unassigned,[],'Flag XY. This argument is optional.')
%
%       % define an argument that has no default, but *must* be assigned a value (i.e. it is a mandatory argument)
%       arg({'myparam','MyParameter'},mandatory,[],'Important Parameter. You need to specify something for this one.')
%       
%       % define an argument that is in a specific "category"
%       arg({'mybool','MyBoolean'},true,[],'Flag XY. And some additional help text.','cat','Core Parameters')
%
%       % define an argument that must be of non-negative integer type, default 3
%       arg({'myint','Mynteger'},uint32(3),[],'Some integer. Be very careful to not go overboard with integers. Their arithmetic rules are *very* counter-intuitive!')
%
%       % define an argument with empty default, but that is of type single and is matrix-shaped
%       arg({'myval','MyValue'},[],[],'Some matrix. Note that the type also encodes whether the value is complex or sparse.','type','denserealsingle','shape','matrix')
%
% See also:
%   arg_nogui, arg_norep, arg_sub, arg_subswitch, arg_subtoggle, arg_define
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24

res = {@invoke_arg_internal,varargin};